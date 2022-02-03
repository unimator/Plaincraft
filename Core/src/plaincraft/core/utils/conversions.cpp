/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin Gorka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "conversions.hpp"

namespace plaincraft_core
{
    rp3d::Vector3 FromGlm(const Vector3d &vector)
    {
        return rp3d::Vector3(vector.x, vector.y, vector.z);
    }

    Vector3d FromRP3D(const rp3d::Vector3 &vector)
    {
        return Vector3d(vector.x, vector.y, vector.z);
    }

    rp3d::Quaternion FromGlm(const Quaternion &quaternion)
    {
        return rp3d::Quaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
    }

    Quaternion FromRP3D(const rp3d::Quaternion &quaternion)
    {
        return Quaternion(quaternion.w, quaternion.x, quaternion.y, quaternion.z);
    }
}