#ifndef RENDERER_H
#define RENDERER_H

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

//#include "AlphaSystem.h"

namespace alpha
{
    class GraphicsWindow;

    class GraphicsRenderer
    {
    public:
        GraphicsRenderer();
        virtual ~GraphicsRenderer();

        bool Initialize();
        bool Update(double currentTime, double elapsedTime);
        bool Shutdown();

        void Render();

    private:
        //virtual bool VUpdate(double currentTime, double elapsedTime);

        // DirectX methods
        //static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        //HRESULT InitializeWindow();
        HRESULT InitializeDevice();
        void CleanupDevice();

        GraphicsWindow *m_pWindow;
    };
}

#endif // RENDERER_H
