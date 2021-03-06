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

#include "Graphics/Renderable.h"

namespace alpha
{
    Renderable::Renderable(std::vector<Vertex> vertexList, std::vector<unsigned int> indexList)
        : vertices(vertexList)
        , indices(indexList)
        , m_vertexBuffer(0)
        , m_vertexAttribute(0)
        , m_elementBuffer(0)
    { }
    Renderable::~Renderable() { }
}
