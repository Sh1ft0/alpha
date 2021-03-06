#ifndef ALPHA_HID_PLATFORM_TRANSLATOR_H
#define ALPHA_HID_PLATFORM_TRANSLATOR_H

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
#include <map>

#include "HID/HIDTypes.h"
#include "HID/HIDConstants.h"

namespace alpha
{
    class HIDPlatformTranslator
    {
    public:
        HIDPlatformTranslator();
        virtual ~HIDPlatformTranslator();

        HIDAction * TranslateKeyboardCode(const unsigned int & code);
        HIDAction * TranslateMouseCode(const unsigned int & code);

    private:
        HIDAction * TranslateCode(const HID & device, const unsigned int & code);
        /** Creates a maping of a raw input to a engine code name. */
        void CreateHIDAction(HID device, unsigned int code, const std::string & name);

        /** Maps platform specific codes to generic engine inputs. */
        std::map<unsigned int, HIDAction *> m_actions[2];
    };
}

#endif // ALPHA_HID_PLATFORM_TRANSLATOR_H
