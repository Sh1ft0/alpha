#ifndef VECTOR_3_H
#define VECTOR_3_H

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

namespace alpha
{
    struct Vector3
    {
        float x, y, z;

        Vector3();
        Vector3(float fx, float fy, float fz);
        Vector3(const Vector3 & vec);

        void Normalize();
        float Length() const;

        float Distance(const Vector3 & right);
        float Dot(const Vector3 & right);
        Vector3 Cross(const Vector3 & right);

        bool operator == (const Vector3& right) const;

        Vector3 & operator=(const Vector3 & right);
        Vector3 & operator+=(const Vector3 & right);
        Vector3 & operator-=(const Vector3 & right);
        Vector3 & operator*=(const Vector3 & right);
        Vector3 & operator*=(float right);
        Vector3 & operator/=(float right);

        /** Interpolate between two points in space with the given time slice */
        static Vector3 Lerp(const Vector3 & start, const Vector3 & end, float t);
    };

    Vector3 operator+(const Vector3 & left, const Vector3 & right);
    Vector3 operator-(const Vector3 & left, const Vector3 & right);
    Vector3 operator*(const Vector3 & left, const Vector3 & right);
    Vector3 operator*(const Vector3 & left, float right);
    Vector3 operator/(const Vector3 & left, const Vector3 & right);
    Vector3 operator*(float left, const Vector3 & right);
}

#endif // VECTOR_3_H
