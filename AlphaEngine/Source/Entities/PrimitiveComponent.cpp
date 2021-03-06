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

#include "Entities/PrimitiveComponent.h"

#include "Scripting/LuaVar.h"

namespace alpha
{
    const std::string PrimitiveComponent::sk_name = "primitive";

    PrimitiveComponent::~PrimitiveComponent() { }

    //! Provides logic for how to initialize a transform component from Lua script data
    void PrimitiveComponent::VInitialize(std::shared_ptr<LuaVar> var)
    {
        std::shared_ptr<LuaTable> table = std::dynamic_pointer_cast<LuaTable>(var);
        std::shared_ptr<LuaVar> transform = table->Get("transform");
        if (transform != nullptr)
        {
            SceneComponent::VInitialize(transform);
        }
    }

    bool PrimitiveComponent::VUpdate(float /*fCurrentTime*/, float /*fElapsedTime*/)
    {
        return true;
    }

    std::string PrimitiveComponent::VGetName() const
    {
        return PrimitiveComponent::sk_name;
    }
}
