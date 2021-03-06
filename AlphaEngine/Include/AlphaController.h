#ifndef ALPHA_CONTROLLER_H
#define ALPHA_CONTROLLER_H

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

#include <chrono>
#include "Toolbox/Logger.h"

namespace alpha
{
    class AlphaSystem;
    class AudioSystem;
    class EventManager;
    class LogicSystem;
    class GraphicsSystem;
    class AssetSystem;
    class StateMachine;
    class AState;
    class AGameState;
    class ThreadSystem;
    class HIDSystem;

    /** 
     * AlphaController
     * This is the main engine controller which handles the lifecycle of the
     * engine, the interaction between engine sub-systems, and the main loop.
     *
     * Sub-systems:
     *     ThreadSystem
     *     LogicSystem
     *     GraphicsSystem
     *     AssetSystem
     *     AudioSystem
     *     HIDSystem
     *
     * Other:
     *     EventManager
     *     StateMachine (game state machine)
     */
	class AlphaController
	{
	public:
        AlphaController();
        virtual ~AlphaController();

        /** Create the state machine and set the starting state. */
        void SetGameState(std::shared_ptr<AGameState> state);

        /** Begin controller execution. */
        void Execute(std::shared_ptr<AGameState> state);
        
    private:
        // non-copyable
        AlphaController(const AlphaController&);
        AlphaController & operator=(const AlphaController&);

        bool Initialize(std::shared_ptr<AGameState> state);
        bool InitializeSystem(AlphaSystem * pSystem);
        bool Update();
        bool Shutdown();
        bool ShutdownSystem(AlphaSystem * pSystem);

        /** Main event management system */
        EventManager * m_pEventManager;

        /** Threading pool system */
        ThreadSystem * m_pThreads;
        /** game logic system */
        LogicSystem * m_pLogic;
        /** Graphics render system */
        GraphicsSystem * m_pGraphics;
        /** Asset Management System */
        AssetSystem * m_pAssets;
        /** Audio management system, manages FMOD lifecycle. */
        AudioSystem * m_pAudio;
        /** Human input management system. */
        HIDSystem * m_pInput;

        /** The game state machine; manages current state and transition to next game state. */
        StateMachine * m_pGameStateMachine;

        /** main loop timer variables */
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
        double m_timeLastFrame = 0.0f;
        double m_timeAccumulator = 0.0f;
        double sk_maxUpdateTime = 1.0f / 60.0f;
	};
}

#endif // ALPHA_CONTROLLER_H
