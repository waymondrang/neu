#pragma once

#include <math.h>
#include <cmath>
#include <cstdint>
#include <iostream>

using Real = float;

template <typename T> struct TVector2;
template <typename T> struct TVector3;
template <typename T> struct TVector4;

template <typename T>
struct TVector2
{
    TVector2() {}

    template <typename T2>
    TVector2(T2 x, T2 y) : x(T(x)), y(T(y)) {}

    template <typename T2>
    TVector2(const TVector2<T2> &v) : x(T(v.x)), y(T(v.y)) {}

    T &operator[](int i)
    {
        return *(&x + i);
    }

    T operator[](int i) const
    {
        return *(&x + i);
    }

    T *ptr()
    {
        return &x;
    }

    const T *ptr() const
    {
        return &x;
    }

    T x, y;
};

template <typename T>
struct TVector3
{
    TVector3() {}

    template <typename T2>
    TVector3(T2 x, T2 y, T2 z) : x(T(x)), y(T(y)), z(T(z)) {}

    template <typename T2>
    TVector3(const TVector3<T2> &v) : x(T(v.x)), y(T(v.y)), z(T(v.z)) {}

    template <typename T2>
    TVector3(const TVector4<T2> &v) : x(T(v.x)), y(T(v.y)), z(T(v.z)) {}

    T &operator[](int i)
    {
        return *(&x + i);
    }

    T operator[](int i) const
    {
        return *(&x + i);
    }

    T *ptr()
    {
        return &x;
    }

    const T *ptr() const
    {
        return &x;
    }

    T x, y, z;
};

template <typename T>
struct TVector4
{
    TVector4() {}

    template <typename T2>
    TVector4(T2 x, T2 y, T2 z, T2 w) : x(T(x)), y(T(y)), z(T(z)), w(T(w)) {}

    template <typename T2>
    TVector4(const TVector4<T2> &v) : x(T(v.x)), y(T(v.y)), z(T(v.z)), w(T(v.w)) {}

    template <typename T2>
    TVector4(const TVector3<T2> &v, T2 w) : x(T(v.x)), y(T(v.y)), z(T(v.z)), w(T(w)) {}

    T &operator[](int i)
    {
        return *(&x + i);
    }

    T operator[](int i) const
    {
        return *(&x + i);
    }

    T *ptr()
    {
        return &x;
    }

    const T *ptr() const
    {
        return &x;
    }

    T x, y, z, w;
};

using Vector2f = TVector2<float>;
using Vector2d = TVector2<double>;
using Vector2i = TVector2<int>;
using Vector2 = TVector2<Real>;
using Vector3uc = TVector3<uint8_t>;
using Vector3i = TVector3<int>;
using Vector3ui = TVector3<uint32_t>;
using Vector3f = TVector3<float>;
using Vector3d = TVector3<double>;
using Vector3 = TVector3<Real>;
using Vector4f = TVector4<float>;
using Vector4d = TVector4<double>;
using Vector4 = TVector4<Real>;

template <typename T>
inline TVector2<T> operator+(const TVector2<T> &v0, const TVector2<T> &v1)
{
    return TVector2<T>(v0.x + v1.x, v0.y + v1.y);
}

template <typename T>
inline TVector2<T> operator+(const TVector2<T> &v, Real s)
{
    return TVector2<T>(v.x + s, v.y + s);
}

template <typename T>
inline TVector2<T> operator+(Real s, const TVector2<T> &v)
{
    return TVector2<T>(s + v.x, s + v.y);
}

template <typename T>
inline TVector2<T> &operator+=(TVector2<T> &v0, const TVector2<T> &v1)
{
    v0.x += v1.x;
    v0.y += v1.y;
    return v0;
}

template <typename T>
inline TVector2<T> operator-(const TVector2<T> &v0, const TVector2<T> &v1)
{
    return TVector2<T>(v0.x - v1.x, v0.y - v1.y);
}

template <typename T>
inline TVector2<T> operator-(const TVector2<T> &v, Real s)
{
    return TVector2<T>(v.x - s, v.y - s);
}

template <typename T>
inline TVector2<T> operator-(Real s, const TVector2<T> &v)
{
    return TVector2<T>(s - v.x, s - v.y);
}

template <typename T>
inline TVector2<T> &operator-=(TVector2<T> &v0, const TVector2<T> &v1)
{
    v0.x -= v1.x;
    v0.y -= v1.y;
    return v0;
}

template <typename T>
inline TVector2<T> operator*(const T &s, const TVector2<T> &v)
{
    return TVector2<T>(s * v[0], s * v[1]);
}

template <typename T>
inline TVector2<T> operator*(const TVector2<T> &v, const T &s)
{
    return TVector2<T>(v[0] * s, v[1] * s);
}

template <typename T>
inline TVector2<T> operator/(const TVector2<T> &v, const T &s)
{
    return TVector2<T>(v[0] / s, v[1] / s);
}

template <typename T>
inline T dot(const TVector2<T> &v0, const TVector2<T> &v1)
{
    return v0[0] * v1[0] + v0[1] * v1[1];
}

template <typename T>
inline T length_squared(const TVector2<T> &v)
{
    return dot(v, v);
}

template <typename T>
inline T length(const TVector2<T> &v)
{
    return sqrt(length_squared(v));
}

template <typename T>
inline TVector2<T> normalize(const TVector2<T> &v0)
{
    auto l = length(v0);
    if (l <= 0)
    {
        return TVector2<T>{0, 0};
    }
    else
    {
        return v0 / l;
    }
}

template <typename T>
inline TVector3<T> operator+(const TVector3<T> &v0, const TVector3<T> &v1)
{
    return TVector3<T>(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
}

template <typename T>
inline TVector3<T> operator+(const TVector3<T> &v, const T &s)
{
    return TVector3<T>(v.x + s, v.y + s, v.z + s);
}

template <typename T>
inline TVector3<T> operator+(const T &s, const TVector3<T> &v)
{
    return TVector3<T>(s + v.x, s + v.y, s + v.z);
}

template <typename T>
inline TVector3<T> &operator+=(TVector3<T> &v0, const TVector3<T> &v1)
{
    v0.x += v1.x;
    v0.y += v1.y;
    v0.z += v1.z;
    return v0;
}

template <typename T>
inline bool operator==(const TVector3<T> &v0, const TVector3<T> &v1)
{
    return (v0.x == v1.x && v0.y == v1.y && v0.z == v1.z);
}


template <typename T>
inline TVector3<T> operator-(const TVector3<T> &v0, const TVector3<T> &v1)
{
    return TVector3<T>(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
}

template <typename T>
inline TVector3<T> operator-(Real s, const TVector3<T> &v)
{
    return TVector3<T>(s - v.x, s - v.y, s - v.z);
}

template <typename T>
inline TVector3<T> operator-(const TVector3<T> &v, Real s)
{
    return TVector3<T>(v.x - s, v.y - s, v.z - s);
}

template <typename T>
inline TVector3<T> operator-(const TVector3<T> &v)
{
    return TVector3<T>(-v.x, -v.y, -v.z);
}

template <typename T>
inline TVector3<T> &operator-=(TVector3<T> &v0, const TVector3<T> &v1)
{
    v0.x -= v1.x;
    v0.y -= v1.y;
    v0.z -= v1.z;
    return v0;
}

template <typename T>
inline TVector3<T> operator*(const T &s, const TVector3<T> &v)
{
    return TVector3<T>(s * v[0], s * v[1], s * v[2]);
}

template <typename T>
inline TVector3<T> operator*(const TVector3<T> &v, const T &s)
{
    return TVector3<T>(v[0] * s, v[1] * s, v[2] * s);
}

template <typename T>
inline TVector3<T> operator*(const TVector3<T> &v0, const TVector3<T> &v1)
{
    return TVector3<T>(v0[0] * v1[0], v0[1] * v1[1], v0[2] * v1[2]);
}

template <typename T>
inline TVector3<T> &operator*=(TVector3<T> &v, const T &s)
{
    v[0] *= s;
    v[1] *= s;
    v[2] *= s;
    return v;
}

template <typename T>
inline TVector3<T> &operator*=(TVector3<T> &v0, const TVector3<T> &v1)
{
    v0[0] *= v1[0];
    v0[1] *= v1[1];
    v0[2] *= v1[2];
    return v0;
}

template <typename T>
inline TVector3<T> operator/(const TVector3<T> &v, const T &s)
{
    T inv_s = T(1) / s;
    return TVector3<T>(v[0] * inv_s, v[1] * inv_s, v[2] * inv_s);
}

template <typename T>
inline TVector3<T> operator/(const T &s, const TVector3<T> &v)
{
    return TVector3<T>(s / v[0], s / v[1], s / v[2]);
}

template <typename T>
inline TVector3<T> operator/(const TVector3<T> &v0, const TVector3<T> &v1)
{
    return TVector3<T>(v0[0] / v1[0], v0[1] / v1[1], v0[2] / v1[2]);
}

template <typename T>
inline TVector3<T> &operator/=(TVector3<T> &v, const T &s)
{
    T inv_s = T(1) / s;
    v *= inv_s;
    return v;
}

template <typename T>
inline T dot(const TVector3<T> &v0, const TVector3<T> &v1)
{
    return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

template <typename T>
inline T length_squared(const TVector3<T> &v)
{
    return dot(v, v);
}

template <typename T>
inline T length(const TVector3<T> &v)
{
    return sqrt(length_squared(v));
}

template <typename T>
inline TVector3<T> normalize(const TVector3<T> &v0)
{
    auto l = length(v0);
    if (l <= 0)
    {
        return TVector3<T>{0, 0, 0};
    }
    else
    {
        return v0 / l;
    }
}

template <typename T>
inline TVector3<T> cross(const TVector3<T> &v0, const TVector3<T> &v1)
{
    return TVector3<T>{
        v0[1] * v1[2] - v0[2] * v1[1],
        v0[2] * v1[0] - v0[0] * v1[2],
        v0[0] * v1[1] - v0[1] * v1[0]};
}

template <typename T>
inline TVector4<T> operator+(const TVector4<T> &v0, const TVector4<T> &v1)
{
    return TVector4<T>(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.w + v1.w);
}

template <typename T>
inline TVector4<T> operator+(const TVector4<T> &v, Real s)
{
    return TVector4<T>(v.x + s, v.y + s, v.z + s, v.w + s);
}

template <typename T>
inline TVector4<T> operator+(Real s, const TVector4<T> &v)
{
    return TVector4<T>(s + v.x, s + v.y, s + v.z, s + v.w);
}

template <typename T>
inline TVector4<T> &operator+=(TVector4<T> &v0, const TVector4<T> &v1)
{
    v0.x += v1.x;
    v0.y += v1.y;
    v0.z += v1.z;
    v0.w += v1.w;
    return v0;
}

template <typename T>
inline TVector4<T> operator-(const TVector4<T> &v0, const TVector4<T> &v1)
{
    return TVector4<T>(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w);
}

template <typename T>
inline TVector4<T> operator-(const TVector4<T> &v, Real s)
{
    return TVector4<T>(v.x - s, v.y - s, v.z - s, v.z - v.w);
}

template <typename T>
inline TVector4<T> operator-(Real s, const TVector4<T> &v)
{
    return TVector4<T>(s - v.x, s - v.y, s - v.z, s - v.w);
}

template <typename T>
inline TVector4<T> &operator-=(TVector4<T> &v0, const TVector4<T> &v1)
{
    v0.x -= v1.x;
    v0.y -= v1.y;
    v0.z -= v1.z;
    v0.w -= v1.w;
    return v0;
}

template <typename T>
inline TVector4<T> operator*(const T &s, const TVector4<T> &v)
{
    return TVector4<T>(s * v[0], s * v[1], s * v[2], s * v[3]);
}

template <typename T>
inline TVector4<T> operator*(const TVector4<T> &v, const T &s)
{
    return TVector4<T>(v[0] * s, v[1] * s, v[2] * s, v[3] * s);
}

template <typename T>
inline TVector4<T> operator/(const TVector4<T> &v, const T &s)
{
    return TVector4<T>(v[0] / s, v[1] / s, v[2] / s, v[3] / s);
}

template <typename T>
inline T dot(const TVector4<T> &v0, const TVector4<T> &v1)
{
    return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2] + v0[3] * v1[3];
}

template <typename T>
inline T length_squared(const TVector4<T> &v)
{
    return dot(v, v);
}

template <typename T>
inline T length(const TVector4<T> &v)
{
    return sqrt(length_squared(v));
}

template <typename T>
inline TVector4<T> normalize(const TVector4<T> &v0)
{
    auto l = length(v0);
    if (l <= 0)
    {
        return TVector4<T>{0, 0, 0, 0};
    }
    else
    {
        return v0 / l;
    }
}

template <typename T>
inline bool isnan(const TVector2<T> &v)
{
    return isnan(v[0]) || isnan(v[1]);
}

template <typename T>
inline bool isnan(const TVector3<T> &v)
{
    return isnan(v[0]) || isnan(v[1]) || isnan(v[2]);
}

template <typename T>
inline bool isnan(const TVector4<T> &v)
{
    return isnan(v[0]) || isnan(v[1]) || isnan(v[2]) || isnan(v[3]);
}

template <typename T>
inline bool isfinite(const TVector2<T> &v)
{
    return isfinite(v[0]) || isfinite(v[1]);
}

template <typename T>
inline bool isfinite(const TVector3<T> &v)
{
    return isfinite(v[0]) || isfinite(v[1]) || isfinite(v[2]);
}

template <typename T>
inline bool isfinite(const TVector4<T> &v)
{
    return isfinite(v[0]) || isfinite(v[1]) || isfinite(v[2]) || isfinite(v[3]);
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const TVector2<T> &v)
{
    return os << "(" << v[0] << ", " << v[1] << ")";
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const TVector3<T> &v)
{
    return os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const TVector4<T> &v)
{
    return os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
}

/**
 * Rotates a vector about an axis.
 *
 * @param vector The vector to rotate
 * @param angle The angle to rotate by
 * @param axis The axis to rotate about
 */
template <typename T>
inline TVector3<T> rotate(const TVector3<T> &vector, const float angle, const TVector3<T> &axis)
{
    TVector3<T> v_c = axis * dot(axis, vector);
    TVector3<T> v_1 = vector - v_c;
    TVector3<T> v_2 = cross(axis, v_1);

    return v_c + v_1 * cos(angle) + v_2 * sin(angle);
}
