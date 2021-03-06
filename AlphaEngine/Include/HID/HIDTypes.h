#ifndef ALPHA_HID_TYPES_H
#define ALPHA_HID_TYPES_H

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

#include <string>

namespace alpha
{
    /**
    * Defines an engine input code, common across all platforms.
    * e.g: 46 = KB_A
    */
    struct HIDAction
    {
        HIDAction(unsigned int actionRaw, std::string actionName);

        unsigned int raw;
        std::string name;
    };

    /** Define supported device types, so they can be easily referenced */
    typedef enum HID
    {
        HID_KEYBOARD,
        HID_MOUSE,
    } HID;

    /** POD class allows tracking of mouse position at a given point in time. */
    struct MousePosition
    {
        MousePosition();

        float xRelativePos;
        float yRelativePos;

        float xAbsolutePos;
        float yAbsolutePos;
    };
}

#endif // ALPHA_HID_TYPES_H
