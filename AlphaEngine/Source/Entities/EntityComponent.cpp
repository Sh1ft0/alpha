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

#include <functional>
#include <string>

#include "Entities/EntityComponent.h"

#include "Scripting/LuaVar.h"
#include "Math/Vector3.h"
#include "Toolbox/Logger.h"

namespace alpha
{
    EntityComponent::~EntityComponent() { }

    void EntityComponent::SetParent(const std::shared_ptr<EntityComponent> & parent)
    {
        m_parent = parent;
    }

    void EntityComponent::Attach(unsigned int component_id, std::shared_ptr<EntityComponent> component)
    {
        auto it = m_components.find(component_id);
        if (it == m_components.end())
        {
            m_components[component_id] = component;
            return;
        }
        LOG_WARN("  <EntityComponent> Attempt to add a component type that already exists: type: ", component->VGetName());
    }

    const std::map<unsigned int, std::shared_ptr<EntityComponent> > EntityComponent::GetComponents() const
    {
        return m_components;
    }

    unsigned int EntityComponent::GetID() const
    {
        return GetIDFromName(this->VGetName());
    }

    unsigned int EntityComponent::GetIDFromName(const std::string & name)
    {
        static std::hash<std::string> string_hash;
        return string_hash(name);
    }

    SceneComponent::~SceneComponent() { }

    void SceneComponent::VInitialize(std::shared_ptr<LuaVar> var)
    {
        // var should represent the transform table from the SceneComponent
        // not the top level scene component.

        if (var->GetVarType() != VT_TABLE)
        {
            LOG_ERR("Script variable data does not represent a valid transform table.");
            return;
        }

        std::shared_ptr<LuaTable> table = std::dynamic_pointer_cast<LuaTable>(var);

        // get the position, scale, and rotation tables.
        std::shared_ptr<LuaTable> position = std::dynamic_pointer_cast<LuaTable>(table->Get("position"));
        std::shared_ptr<LuaTable> rotation = std::dynamic_pointer_cast<LuaTable>(table->Get("rotation"));
        std::shared_ptr<LuaTable> scale = std::dynamic_pointer_cast<LuaTable>(table->Get("scale"));

        // get x, y, z values for each position, scale, and rotation.
        m_vPosition.x = this->GetAxis(position, "x");
        m_vPosition.y = this->GetAxis(position, "y");
        m_vPosition.z = this->GetAxis(position, "z");

        m_vScale.x = this->GetAxis(scale, "x");
        m_vScale.y = this->GetAxis(scale, "y");
        m_vScale.z = this->GetAxis(scale, "z");

        LOG(" .... test position = (", this->m_vPosition.x, ", ", this->m_vPosition.y, ", ", this->m_vPosition.z, ")");
        LOG(" .... test scale = (", this->m_vScale.x, ", ", this->m_vScale.y, ", ", this->m_vScale.z, ")");
    }

    const Vector3 & SceneComponent::GetPosition() const
    {
        return m_vPosition;
    }

    const Vector3 & SceneComponent::GetScale() const
    {
        return m_vScale;
    }

    float SceneComponent::GetAxis(std::shared_ptr<LuaTable> table, const std::string axis)
    {
        std::shared_ptr<LuaStatic<double> > var = std::dynamic_pointer_cast<LuaStatic<double>>(table->Get(axis));
        return static_cast<float>(var->GetValue());
    }
}
