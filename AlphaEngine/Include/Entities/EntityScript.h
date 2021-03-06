#ifndef ENTITY_SCRIPT_H
#define ENTITY_SCRIPT_H

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

#include <map>
#include <memory>

#include "Scripting/LuaScript.h"

namespace alpha
{
    class EntityComponent;
    class LuaTable;

    class EntityScript : public LuaScript
    {
    public:
        explicit EntityScript(std::shared_ptr<Asset> asset);
        virtual ~EntityScript();

        bool HasComponent(const std::string & name);
        const std::map<std::string, std::shared_ptr<LuaVar> > & GetComponentVars();

    private:
        std::shared_ptr<LuaTable> m_components;
    };
}

#endif // ENTITY_SCRIPT_H
