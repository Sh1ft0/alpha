// Copyright 2014-2015 Jason R. Wendlandt
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#version 330 core
out vec4 color;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;

void main ()
{
    // static vars
    //float ambientStrength = 0.9f;
    //color = vec4(objectColor * ambientStrength, 1.0f);
    color = vec4(objectColor, 1.0f);
}
