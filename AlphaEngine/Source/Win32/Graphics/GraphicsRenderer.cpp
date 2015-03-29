/**
Copyright 2014-2015 Jason R. Wendlandt

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <Windows.h>

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <vector>

#include "Graphics/GraphicsRenderer.h"
#include "Graphics/GraphicsWindow.h"
#include "Graphics/RenderSet.h"
#include "Graphics/Renderable.h"
#include "Graphics/Camera.h"
#include "Math/Matrix.h"
#include "Math/Matrix_Conversions.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Assets/Asset.h"
#include "Toolbox/Logger.h"

using namespace DirectX;

namespace alpha
{
    const std::string GraphicsRenderer::sk_shader_extension = "hlsl";

    D3D_DRIVER_TYPE         m_driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL       m_featureLevel = D3D_FEATURE_LEVEL_11_1;
    ID3D11Device*           m_pd3dDevice = nullptr;
    ID3D11Device1*          m_pd3dDevice1 = nullptr;
    ID3D11DeviceContext*    m_pImmediateContext = nullptr;
    ID3D11DeviceContext1*   m_pImmediateContext1 = nullptr;
    IDXGISwapChain*         m_pSwapChain = nullptr;
    IDXGISwapChain1*        m_pSwapChain1 = nullptr;
    ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
    ID3D11Texture2D*        m_pDepthStencil = nullptr;
    ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

    GraphicsRenderer::GraphicsRenderer() { }
    GraphicsRenderer::~GraphicsRenderer() { }

    bool GraphicsRenderer::Initialize()
    {
        m_pWindow = new GraphicsWindow();
        if (!m_pWindow->Initialize())
        {
            LOG_ERR("Failed to create and open game window.");
            return false;
        }
        if (FAILED(this->InitializeDevice()))
        {
            LOG_ERR("Failed to initialize graphics device.");
            return false;
        }
        return true;
    }

    bool GraphicsRenderer::Shutdown()
    {
        // close game window
        if (!m_pWindow->Shutdown())
        {
            LOG_ERR("Failed to close game window.");
        }
        delete m_pWindow;

        // cleanup graphics device
        this->CleanupDevice();

        return true;
    }

    bool GraphicsRenderer::Update(double currentTime, double elapsedTime)
    {
        // let any windows messages through
        return m_pWindow->Update(currentTime, elapsedTime);
    }

    void GraphicsRenderer::Render(std::shared_ptr<Camera> pCamera, std::vector<RenderSet *> renderables)
    {
        // 1. pre-render
        this->PreRender(renderables);

        // clear ... zap!
        m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::Black);
        m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        for (RenderSet * render_set : renderables)
        {
            this->RenderRenderable(pCamera, render_set);
        }

        // swap buffer that we just drew to.
        m_pSwapChain->Present(0, 0);
    }

    void GraphicsRenderer::SetBasicShaders(std::shared_ptr<Asset> psShader, std::shared_ptr<Asset> vsShader)
    {
        m_psDefaultShader = psShader;
        m_vsDefaultShader = vsShader;
    }

    HRESULT GraphicsRenderer::InitializeDevice()
    {
        HRESULT hr = S_OK;
        HWND hwnd = m_pWindow->GetHWND();

        RECT rc;
        GetClientRect(hwnd, &rc);
        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;

        UINT createDeviceFlags = 0;
#ifdef ALPHA_DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            m_driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext);

            if (hr == E_INVALIDARG)
            {
                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1, D3D11_SDK_VERSION, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext);
            }

            if (SUCCEEDED(hr))
            {
                break;
            }
        }
        if (FAILED(hr))
        {
            return hr;
        }

        // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
        IDXGIFactory1* dxgiFactory = nullptr;
        {
            IDXGIDevice* dxgiDevice = nullptr;
            hr = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
            if (SUCCEEDED(hr))
            {
                IDXGIAdapter* adapter = nullptr;
                hr = dxgiDevice->GetAdapter(&adapter);
                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
                    adapter->Release();
                }
                dxgiDevice->Release();
            }
        }
        if (FAILED(hr))
        {
            return hr;
        }

        // Create swap chain
        IDXGIFactory2* dxgiFactory2 = nullptr;
        hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
        if (dxgiFactory2)
        {
            // DirectX 11.1 or later
            hr = m_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&m_pd3dDevice1));
            if (SUCCEEDED(hr))
            {
                (void)m_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&m_pImmediateContext1));
            }

            DXGI_SWAP_CHAIN_DESC1 sd;
            ZeroMemory(&sd, sizeof(sd));
            sd.Width = width;
            sd.Height = height;
            sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = 1;

            hr = dxgiFactory2->CreateSwapChainForHwnd(m_pd3dDevice, hwnd, &sd, nullptr, nullptr, &m_pSwapChain1);
            if (SUCCEEDED(hr))
            {
                hr = m_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&m_pSwapChain));
            }

            dxgiFactory2->Release();
        }
        else
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd;
            ZeroMemory(&sd, sizeof(sd));
            sd.BufferCount = 1;
            sd.BufferDesc.Width = width;
            sd.BufferDesc.Height = height;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = hwnd;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;

            hr = dxgiFactory->CreateSwapChain(m_pd3dDevice, &sd, &m_pSwapChain);
        }

        dxgiFactory->Release();

        if (FAILED(hr))
        {
            return hr;
        }

        // Create a render target view
        ID3D11Texture2D* pBackBuffer = nullptr;
        hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
        if (FAILED(hr))
        {
            return hr;
        }

        hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
        pBackBuffer->Release();
        if (FAILED(hr))
        {
            return hr;
        }

        // Create depth stencil texture
        D3D11_TEXTURE2D_DESC descDepth;
        ZeroMemory(&descDepth, sizeof(descDepth));
        descDepth.Width = width;
        descDepth.Height = height;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDepth.SampleDesc.Count = 1;
        descDepth.SampleDesc.Quality = 0;
        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        descDepth.CPUAccessFlags = 0;
        descDepth.MiscFlags = 0;
        hr = m_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &m_pDepthStencil);
        if (FAILED(hr))
            return hr;

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        ZeroMemory(&descDSV, sizeof(descDSV));
        descDSV.Format = descDepth.Format;
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
        if (FAILED(hr))
            return hr;

        m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

        // Setup the viewport
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)width;
        vp.Height = (FLOAT)height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        m_pImmediateContext->RSSetViewports(1, &vp);

        // Initialize the view matrix
        /*
        XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -20.0f, 0.0f);
        XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        m_View = XMMatrixLookAtLH(Eye, At, Up);

        // Initialize the projection matrix
        m_Projection = XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
        */

        return S_OK;
    }

    void GraphicsRenderer::CleanupDevice()
    {
        if (m_pImmediateContext) m_pImmediateContext->ClearState();

        if (m_pDepthStencil) m_pDepthStencil->Release();
        if (m_pDepthStencilView) m_pDepthStencilView->Release();
        if (m_pRenderTargetView) m_pRenderTargetView->Release();
        if (m_pSwapChain1) m_pSwapChain1->Release();
        if (m_pSwapChain) m_pSwapChain->Release();
        if (m_pImmediateContext1) m_pImmediateContext1->Release();
        if (m_pImmediateContext) m_pImmediateContext->Release();
        if (m_pd3dDevice1) m_pd3dDevice1->Release();
        if (m_pd3dDevice) m_pd3dDevice->Release();

        /*
        m_pd3dDevice = nullptr;
        m_pd3dDevice1 = nullptr;
        m_pImmediateContext = nullptr;
        m_pImmediateContext1 = nullptr;
        m_pSwapChain = nullptr;
        m_pSwapChain1 = nullptr;
        m_pRenderTargetView = nullptr;
        */
    }

    void GraphicsRenderer::PreRender(std::vector<RenderSet *> renderSets)
    {
        for (RenderSet * rs : renderSets)
        {
            this->PrepRenderable(rs);
        }
    }

    void GraphicsRenderer::PrepRenderable(RenderSet * renderSet)
    {
        HRESULT hr = S_OK;

        auto renderables = renderSet->GetRenderables();

        for (Renderable * renderable : renderables)
        {
            // create vertex shader
            ID3DBlob* pVSBlob = nullptr;
            if (renderable->m_pVertexShader == nullptr)
            {
                renderable->m_pVertexShader = this->CreateVertexShaderFromAsset(m_vsDefaultShader, "VS", &pVSBlob);
            }

            // vertex input layout
            if (renderable->m_pInputLayout == nullptr && pVSBlob != nullptr)
            {
                renderable->m_pInputLayout = this->CreateInputLayoutFromVSBlob(&pVSBlob);
                pVSBlob->Release();
            }

            // pixel shader
            if (renderable->m_pPixelShader == nullptr)
            {
                renderable->m_pPixelShader = this->CreatePixelShaderFromAsset(m_psDefaultShader, renderSet->m_psEntryPoint);
            }

            // vertex buffer
            if (renderable->m_pVertexBuffer == nullptr || renderable->m_pIndexBuffer == nullptr || renderable->m_pConstantBuffer == nullptr)
            {
                D3D11_BUFFER_DESC bd;
                ZeroMemory(&bd, sizeof(bd));
                bd.Usage = D3D11_USAGE_DEFAULT;
                bd.ByteWidth = sizeof(Vertex) * renderable->vertices.size();
                bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                bd.CPUAccessFlags = 0;

                D3D11_SUBRESOURCE_DATA InitData;
                ZeroMemory(&InitData, sizeof(InitData));
                InitData.pSysMem = &renderable->vertices[0];

                hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, &renderable->m_pVertexBuffer);
                //if (FAILED(hr))
                //    return hr;

                // index buffer
                bd.Usage = D3D11_USAGE_DEFAULT;
                bd.ByteWidth = sizeof(WORD) * renderable->indices.size();
                bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
                bd.CPUAccessFlags = 0;
                InitData.pSysMem = &renderable->indices[0];
                hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, &renderable->m_pIndexBuffer);
                //if (FAILED(hr))
                //    return hr;

                // constant buffer
                bd.Usage = D3D11_USAGE_DEFAULT;
                bd.ByteWidth = sizeof(ConstantBuffer);
                bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                bd.CPUAccessFlags = 0;
                hr = m_pd3dDevice->CreateBuffer(&bd, nullptr, &renderable->m_pConstantBuffer);
                //if (FAILED(hr))
                //    return hr;
            }
        }
    }

    void GraphicsRenderer::RenderRenderable(std::shared_ptr<Camera> pCamera, RenderSet * renderSet)
    {
        // Setup our lighting parameters
        Vector4 vLightDirs[2] =
        {
            Vector4(-0.577f, 0.577f, -0.577f, 1.0f),
            Vector4(0.0f, 0.0f, -1.0f, 1.0f),
        };
        Vector4 vLightColors[2] =
        {
            Vector4(0.5f, 0.5f, 0.5f, 1.0f),
            Vector4(0.5f, 0.0f, 0.0f, 1.0f)
        };

        auto renderables = renderSet->GetRenderables();

        for (auto renderable : renderables)
        {
            // set input layout
            m_pImmediateContext->IASetInputLayout(renderable->m_pInputLayout);

            // Set vertex buffer
            UINT stride = sizeof(Vertex);
            UINT offset = 0;
            m_pImmediateContext->IASetVertexBuffers(0, 1, &renderable->m_pVertexBuffer, &stride, &offset);

            // Set index buffer
            m_pImmediateContext->IASetIndexBuffer(renderable->m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

            // Set primitive topology
            m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // update constant buffer
            ConstantBuffer cb;
            cb.mWorld = DirectX::XMMatrixTranspose(MatrixToXMMATRIX(renderSet->worldTransform));
            cb.mView = DirectX::XMMatrixTranspose(MatrixToXMMATRIX(pCamera->GetView()));
            cb.mProjection = DirectX::XMMatrixTranspose(MatrixToXMMATRIX(pCamera->GetProjection()));
            cb.vLightDir[0] = vLightDirs[0];
            cb.vLightDir[1] = vLightDirs[1];
            cb.vLightColor[0] = vLightColors[0];
            cb.vLightColor[1] = vLightColors[1];
            cb.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
            cb.vOutputColor = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
            m_pImmediateContext->UpdateSubresource(renderable->m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

            // render object
            m_pImmediateContext->VSSetShader(renderable->m_pVertexShader, nullptr, 0);
            m_pImmediateContext->VSSetConstantBuffers(0, 1, &renderable->m_pConstantBuffer);
            m_pImmediateContext->PSSetShader(renderable->m_pPixelShader, nullptr, 0);
            m_pImmediateContext->PSSetConstantBuffers(0, 1, &renderable->m_pConstantBuffer);

            m_pImmediateContext->DrawIndexed(renderable->indices.size(), 0, 0);
        }
    }

    bool GraphicsRenderer::CompileShaderFromAsset(std::shared_ptr<Asset> asset, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
    {
        HRESULT hr = S_OK;
        ID3DBlob* pErrorBlob = nullptr;
        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
        std::vector<unsigned char> data = asset->GetData();

#ifdef ALPHA_DEBUG
        // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
        // Setting this flag improves the shader debugging experience, but still allows 
        // the shaders to be optimized and to run exactly the way they will run in 
        // the release configuration of this program.
        dwShaderFlags |= D3DCOMPILE_DEBUG;

        // Disable optimizations to further improve shader debugging
        dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        hr = D3DCompile(&data[0], sizeof(char) * data.size(), nullptr, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
        if (FAILED(hr))
        {
            if (pErrorBlob)
            {
                LOG_ERR("SHADER Failed to compile shader, ", reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
                pErrorBlob->Release();
            }
            return false;
        }

        if (pErrorBlob)
        {
            pErrorBlob->Release();
        }

        return true;
    }

    ID3D11VertexShader * GraphicsRenderer::CreateVertexShaderFromAsset(std::shared_ptr<Asset> vsAsset, const std::string & sEntryPoint, ID3DBlob** ppVSBlobOut)
    {
        // TODO: store created shader, and retrieve if used again.

        HRESULT hr = S_OK;
        //ID3DBlob* pVSBlob = nullptr;
        ID3D11VertexShader * vs = nullptr;

        // compile shader
        if (!this->CompileShaderFromAsset(vsAsset, sEntryPoint.c_str(), "vs_4_0", ppVSBlobOut))
        {
            LOG_WARN("GraphicsRenderer > Failed to compile pixel shader");
            return nullptr;
        }

        // Create the vertex shader
        hr = m_pd3dDevice->CreateVertexShader((*ppVSBlobOut)->GetBufferPointer(), (*ppVSBlobOut)->GetBufferSize(), nullptr, &vs);
        if (FAILED(hr))
        {
            LOG_WARN("GraphicsRenderer > Failed to create pixel shader.");
            (*ppVSBlobOut)->Release();
            return nullptr;
        }

        return vs;
    }

    ID3D11PixelShader * GraphicsRenderer::CreatePixelShaderFromAsset(std::shared_ptr<Asset> psAsset, const std::string & sEntryPoint)
    {
        // TODO: store created shader, and retrieve if used again.

        HRESULT hr = S_OK;
        ID3DBlob* pPSBlob = nullptr;
        ID3D11PixelShader * ps = nullptr;

        // Compile the pixel shader
        pPSBlob = nullptr;
        if (!this->CompileShaderFromAsset(psAsset, sEntryPoint.c_str(), "ps_4_0", &pPSBlob))
        {
            LOG_WARN("GraphicsRenderer > Failed to compile pixel shader");
            return nullptr;
        }

        // Create the pixel shader
        hr = m_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &ps);
        pPSBlob->Release();
        if (FAILED(hr))
        {
            LOG_WARN("GraphicsRenderer > Failed to create pixel shader.");
            //pPSBlob->Release();
            return nullptr;
        }

        return ps;
    }

    ID3D11InputLayout * GraphicsRenderer::CreateInputLayoutFromVSBlob(ID3DBlob ** const pVSBlob)
    {
        // TODO: store created vertex shader with layout data, and retrieve if used again.

        HRESULT hr = S_OK;
        ID3D11InputLayout * vsLayout = nullptr;

        // Define the input layout
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT numElements = ARRAYSIZE(layout);

        // Create the input layout
        hr = m_pd3dDevice->CreateInputLayout(layout, numElements, (*pVSBlob)->GetBufferPointer(), (*pVSBlob)->GetBufferSize(), &vsLayout);
        //pVSBlob->Release();
        if (FAILED(hr))
        {
            return nullptr;
        }

        return vsLayout;
    }
}
