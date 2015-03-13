#ifndef ALPHA_EVENT_DATA_SET_ACTIVE_CAMERA_H
#define ALPHA_EVENT_DATA_SET_ACTIVE_CAMERA_H

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

#include <memory>

#include "Events/EventData.h"

namespace alpha
{
    class CameraComponent;

    /**
     * Event to tell the graphics system that a new camera component
     * should drive the renderer camera for the scene.
     */
    class EventData_SetActiveCamera : public EventData
    {
        /** The type name that defines this event type */
        static const std::string sk_name;

    public:
        explicit EventData_SetActiveCamera(std::weak_ptr<CameraComponent> pCameraComponent);
        virtual std::string VGetTypeName() const;

        /** Retrieve the camera component to set as the active camera */
        std::weak_ptr<CameraComponent> GetCameraComponent() const;

    private:
        // non-copyable
        EventData_SetActiveCamera(const EventData_SetActiveCamera&);
        EventData_SetActiveCamera & operator=(const EventData_SetActiveCamera&);

        /** Weak pointer to the entity camera component to be set as the active camera */
        std::weak_ptr<CameraComponent> m_pCameraComponent;
    };
}

#endif // ALPHA_EVENT_DATA_SET_ACTIVE_CAMERA_H
