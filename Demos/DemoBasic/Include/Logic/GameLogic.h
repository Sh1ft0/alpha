#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

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

#include "Logic/LogicSystem.h"

namespace alpha {
    class AssetSystem;
}

class GameLogic : public alpha::LogicSystem
{
public:
    GameLogic();
    virtual ~GameLogic();

    virtual bool VInitialize(std::shared_ptr<alpha::AssetSystem> pAssets);

private:
    std::shared_ptr<alpha::Entity> m_test;
    std::shared_ptr<alpha::Entity> m_test2;
};

#endif // GAME_LOGIC_H
