#ifndef LOGIC_SYSTEM_H
#define LOGIC_SYSTEM_H

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
#include "AlphaSystem.h"

#include "Events/EventDataSubscriber.h"
#include "Events/EventDataPublisher.h"
#include "Events/EventData_EntityCreated.h"
#include "Events/EventData_HIDKeyAction.h"

namespace alpha
{
    class AssetSystem;
    class AudioSystem;
    class EntityFactory;
    class Entity;
    class HIDContextManager;
    class StateMachine;
    class Sound;

    class LogicSystem : public AlphaSystem
    {
    public:
        LogicSystem();
        virtual ~LogicSystem();

        virtual bool VInitialize();
        virtual bool VShutdown();

        /** Set AssetSystem
         * This must be set before any entities can be created.
         */
        void SetAssetSystem(std::shared_ptr<AssetSystem> pAssets);
        /** Allow controller to attach audio system to logic layer. */
        void SetAudioSystem(std::weak_ptr<AudioSystem> pAudio);

        /** Entity life-cycle methods */
        std::shared_ptr<Entity> GetEntity(const unsigned long entityId);
        std::shared_ptr<Entity> CreateEntity(const char * resource);
        void DestroyEntity(const unsigned long entityId);

        /** Audio life-cycle methods */
        std::weak_ptr<Sound> CreateSound(const char * resource);

        /** HID Bindings */


        /** event subscriptions */
        void SubscribeToEntityCreated(std::shared_ptr<AEventDataSubscriber> pSubscriber);

        /** Retrieve the HIDKeyAction subscriber so it can be 'subscribed' to the publisher */
        std::shared_ptr<AEventDataSubscriber> GetHIDKeyActionSubscriber() const;

    private:
        virtual bool VUpdate(double currentTime, double elapsedTime);

        /** Handle HID Key Action events from subscription */
        void ReadHIDKeyActionSubscription();
        
        EntityFactory *m_pEntityFactory;
        std::map<unsigned long, std::shared_ptr<Entity> > m_entities;

        /** Asset management system handle. */
        std::shared_ptr<AssetSystem> m_pAssets;

        /** Handle to the audio system, allows logic to create and manage sounds in a game */
        std::weak_ptr<AudioSystem> m_pAudio;

        /** Publisher for new entities created */
        EventDataPublisher<EventData_EntityCreated> m_pubEntityCreated;

        /** Subscriber for HIDKeyAction events */
        std::shared_ptr<EventDataSubscriber<EventData_HIDKeyAction>> m_subHIDKeyAction;

        /** HID Context Manager */
        HIDContextManager * m_pHIDContextManager;
    };
}

#endif // LOGIC_SYSTEM_H
