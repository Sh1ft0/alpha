/**
Copyright 2014 Jason R. Wendlandt

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

#include "Entities/Entity.h"

namespace alpha
{
    Entity::Entity(unsigned long entityId, std::shared_ptr<EntityScript> script)
        : m_entityId(entityId)
        , m_script(script)
    { }
    Entity::~Entity() { }

    bool Entity::VUpdate(float /*fCurrentTime*/, float /*fElapsedTime*/)
    {
        return true;
    }

    unsigned long Entity::GetId() const
    {
        return m_entityId;
    }

    std::shared_ptr<EntityScript> Entity::GetScript() const
    {
        return m_script;
    }
}
