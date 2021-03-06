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

#include "HID/DemoContext.h"

DemoContext::DemoContext()
{
    // make context mappings for demo
    MapState("STRAFE_LEFT", { "KA_A", "KA_LEFT" });
    MapState("STRAFE_RIGHT", { "KA_D", "KA_RIGHT" });
    MapState("MOVE_FORWARD", { "KA_W", "KA_UP" });
    MapState("MOVE_BACK", { "KA_S", "KA_DOWN" });
}