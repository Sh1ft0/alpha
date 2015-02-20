#ifndef TRANSFORM_H
#define TRANSFORM_H

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

#include "Math/Vector3.h"

namespace alpha
{
    struct Vector3;
    struct Quaternion;

    struct Matrix
    {
    public:
        Matrix();
        virtual ~Matrix();

        void Rotate(const Quaternion & rotation);
        void Translate(const Vector3 & position);
        void Scale(const Vector3 & scale);
    };

    Matrix operator* (const Matrix& left, const Matrix& right);
}

#endif // TRANSFORM_H