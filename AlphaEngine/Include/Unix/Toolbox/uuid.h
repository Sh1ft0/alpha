#ifndef TOOLBOX_UUID_H
#define TOOLBOX_UUID_H

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

#include <uuid/uuid.h>
#include <string>

namespace alpha
{
    /**
     * function GenerateUUID
     *
     * Generates a universally unique identifer, converts it to a string, then hashes the string.
     */
    unsigned int GenerateUUID(void);
}

#endif // TOOLBOX_UUID_H
