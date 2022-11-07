#pragma once

#include <cstdint>

#include <cstddef>
#include <cstring>
#include <exception>
#include <filesystem>
#include <functional>
#include <map>
#include <math.h>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <math.h>

#ifndef WIN32
#define fprintf_s fprintf
#define printf_s printf
#endif

namespace Universe
{
    using String     = std::string;
    using WideString = std::wstring;

    using StringView     = std::string_view;
    using WideStringView = std::wstring_view;

    using StringVector     = std::vector<String>;
    using WideStringVector = std::vector<WideString>;

    using StringSet     = std::unordered_set<String>;
    using WideStringSet = std::unordered_set<WideString>;

    using StringMap     = std::unordered_map<String, String>;
    using WideStringMap = std::unordered_map<WideString, WideString>;

    template <typename K, typename V>
    using Map = std::map<K, V>;

    template <typename K, typename V>
    using UnorderedMap = std::unordered_map<K, V>;

    template <typename T>
    using Vector = std::vector<T>;

    template <typename T>
    using Array = std::vector<T>;

    template <typename T, typename T2>
    using Pair = std::pair<T, T2>;

    using Boolean = bool;
    using Integer = int32_t;
    using Float   = float;
    using Double  = double;

    using Path = std::filesystem::path;

    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

    template <typename T>
    using Unique = std::unique_ptr<T>;

    template <typename T>
    using Shared = std::shared_ptr<T>;

    template <typename T>
    using Weak = std::weak_ptr<T>;

    template <typename T>
    using Ref = std::reference_wrapper<T>;

    // MakeUnique
    template <typename T>
    inline Unique<T> MakeUnique(T* ptr)
    {
        MakeUnique<T>(ptr);
        return std::make_unique<T>(ptr);
    }
    template <typename T>
    inline Unique<T> MakeUnique(const T* ptr)
    {
        MakeUnique<T>(ptr);
        return std::make_unique<T>(ptr);
    }

    // std::runtime_error
    class RuntimeError : public std::runtime_error {
    public:
        RuntimeError(const String& message)
            : std::runtime_error(message)
        {}
    };

    // std::logic_error
    class Exception : public std::logic_error {
    public:
        Exception(const String& message)
            : std::logic_error(message.c_str())
        {}
    };

    using Void = void;

    template <typename T>
    using Function = std::function<T>;

    using VoidConstPtr = const void*;

    using VoidConstPtrConst = const void* const;

    using TextStream   = std::stringstream;
    using BinaryStream = std::stringstream;

    using hash_t = std::size_t;

    using errno_t = int;
}

namespace Universe::Math
{
    class Vector2 {
    public:
        float x, y;

        Vector2(void) {}
        Vector2(float x, float y)
        {
            this->x = x;
            this->y = y;
        }

        // (0,1,0) means no rotation. So get right vector and its atan
        inline float Heading(void) const { return atan2f(-x, y); }
        inline float Magnitude2D(void) const { return sqrtf(x * x + y * y); }
        inline float MagnitudeSqr2D(void) const { return x * x + y * y; }
        inline void  Normalise2D(void)
        {
            float sq      = MagnitudeSqr2D();
            float invsqrt = sq / sqrtf(y);
            x *= invsqrt;
            y *= invsqrt;
        }

        static inline float Distance2D(const Vector2& v1, const Vector2& v2)
        {
            float x = v2.x - v1.x;
            float y = v2.y - v1.y;
            return sqrtf(x * x + y * y);
        }
    };

    class Vector3 : public Vector2 {
    public:
        float z;

        Vector3(void) {}
        Vector3(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        inline float Magnitude(void) const { return sqrtf(x * x + y * y + z * z); }
        inline float MagnitudeSqr(void) const { return x * x + y * y + z * z; }
        inline void  Normalise(void)
        {
            float sq = MagnitudeSqr();
            if (sq > 0.0f)
            {
                float invsqrt = sq / sqrtf(y);
                x *= invsqrt;
                y *= invsqrt;
                z *= invsqrt;
            }
            else
                x = 1.0f;
        }

        const Vector3& operator+=(Vector3 const& right)
        {
            x += right.x;
            y += right.y;
            z += right.z;
            return *this;
        }

        const Vector3& operator-=(Vector3 const& right)
        {
            x -= right.x;
            y -= right.y;
            z -= right.z;
            return *this;
        }

        const Vector3& operator*=(float right)
        {
            x *= right;
            y *= right;
            z *= right;
            return *this;
        }

        const Vector3& operator/=(float right)
        {
            x /= right;
            y /= right;
            z /= right;
            return *this;
        }

        const Vector3& operator-() { return Vector3(-x, -y, -z); }

        const bool operator==(Vector3 const& right) { return x == right.x && y == right.y && z == right.z; }
        const bool operator!=(Vector3 const& right) { return x != right.x || y != right.y || z != right.z; }

        inline bool IsZero(void) const { return x == 0.0f && y == 0.0f && z == 0.0f; }

        inline float        DotProduct(const Vector3& v2) { return x * v2.x + y * v2.y + z * v2.z; }
        static inline float Distance(const Vector3& v1, const Vector3& v2);
    };

    inline Vector3 operator+(const Vector3& left, const Vector3& right)
    {
        return Vector3(left.x + right.x, left.y + right.y, left.z + right.z);
    }
    inline Vector3 operator-(const Vector3& left, const Vector3& right)
    {
        return Vector3(left.x - right.x, left.y - right.y, left.z - right.z);
    }
    inline Vector3 operator*(const Vector3& left, float right)
    {
        return Vector3(left.x * right, left.y * right, left.z * right);
    }
    inline Vector3 operator*(float left, const Vector3& right)
    {
        return Vector3(left * right.x, left * right.y, left * right.z);
    }
    inline Vector3 operator/(const Vector3& left, float right)
    {
        return Vector3(left.x / right, left.y / right, left.z / right);
    }

    inline float Vector3::Distance(const Vector3& v1, const Vector3& v2)
    {
        return (v2 - v1).Magnitude();
    }

    class Matrix4x4 {
    public:
        union
        {
            float f[4][4];
            struct
            {
                float rx, ry, rz, rw;
                float fx, fy, fz, fw;
                float ux, uy, uz, uw;
                float px, py, pz, pw;
            };
        };
        void* m_secondMatrix;
        bool  m_hasSecondMatrix;

        Matrix4x4(void) {}
        Matrix4x4(Matrix4x4 const& m) { *this = m; }
        Matrix4x4(float scale) { SetScale(scale); }
        ~Matrix4x4(void) {}

        inline void operator=(Matrix4x4 const& rhs) { memcpy(this, &rhs, sizeof(f)); }

        inline void CopyOnlyMatrix(const Matrix4x4& other) { memcpy(this, &other, sizeof(f)); }

        inline void ResetOrientation(void)
        {
            rx = 1.0f;
            ry = rz = 0.0f;
            fx      = 0.0f;
            fy      = 1.0f;
            fz      = 0.0f;
            ux = uy = 0.0f;
            uz      = 1.0f;
        }

        inline void SetScale(float s)
        {
            rx = s;
            ry = rz = 0.0f;

            fx = 0.0f;
            fy = s;
            fz = 0.0f;

            ux = uy = 0.0f;
            uz      = s;

            px = py = pz = 0.0f;
        }

        inline void SetTranslate(float x, float y, float z)
        {
            rx = 1.0f;
            ry = 0.0f;
            rz = 0.0f;

            fx = 0.0f;
            fy = 1.0f;
            fz = 0.0f;

            ux = 0.0f;
            uy = 0.0f;
            uz = 1.0f;

            px = x;
            py = y;
            pz = z;
        }

        inline void SetTranslate(const Vector3& trans) { SetTranslate(trans.x, trans.y, trans.z); }
        inline void Translate(float x, float y, float z) { px += x, py += y, pz += z; }
        inline void Translate(const Vector3& trans) { Translate(trans.x, trans.y, trans.z); }

        inline Vector3 GetPosition(void) { return Vector3(px, py, pz); }
        inline Vector3 GetRight(void) { return Vector3(rx, ry, rz); }
        inline Vector3 GetForward(void) { return Vector3(fx, fy, fz); }
        inline Vector3 GetUp(void) { return Vector3(ux, uy, uz); }

        inline void SetPosition(Vector3 v)
        {
            px = v.x;
            py = v.y;
            pz = v.z;
        }
        inline void SetRight(Vector3 v)
        {
            rx = v.x;
            ry = v.y;
            rz = v.z;
        }
        inline void SetForward(Vector3 v)
        {
            fx = v.x;
            fy = v.y;
            fz = v.z;
        }
        inline void SetUp(Vector3 v)
        {
            ux = v.x;
            uy = v.y;
            uz = v.z;
        }

        inline void Scale(float scale)
        {
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    f[i][j] *= scale;
        }

        inline void SetRotateXOnly(float angle)
        {
            float c = cosf(angle);
            float s = sinf(angle);

            rx = 1.0f;
            ry = rz = 0.0f;

            fx = 0.0f;
            fy = c;
            fz = s;

            ux = 0.0f;
            uy = -s;
            uz = c;
        }

        inline void SetRotateYOnly(float angle)
        {
            float c = cosf(angle);
            float s = sinf(angle);

            rx = c;
            ry = 0.0f;
            rz = -s;

            fx = 0.0f;
            fy = 1.0f;
            fz = 0.0f;

            ux = s;
            uy = 0.0f;
            uz = c;
        }

        inline void SetRotateZOnly(float angle)
        {
            float c = cosf(angle);
            float s = sinf(angle);

            rx = c;
            ry = s;
            rz = 0.0f;

            fx = -s;
            fy = c;
            fz = 0.0f;

            ux = uy = 0.0f;
            uz      = 1.0f;
        }

        inline void SetRotateX(float angle)
        {
            SetRotateXOnly(angle);
            px = py = pz = 0.0f;
        }

        inline void SetRotateY(float angle)
        {
            SetRotateYOnly(angle);
            px = py = pz = 0.0f;
        }

        inline void SetRotateZ(float angle)
        {
            SetRotateZOnly(angle);
            px = py = pz = 0.0f;
        }

        inline void SetRotate(float xAngle, float yAngle, float zAngle)
        {
            float cX = cosf(xAngle), sX = sinf(xAngle);
            float cY = cosf(yAngle), sY = sinf(yAngle);
            float cZ = cosf(zAngle), sZ = sinf(zAngle);

            rx = cZ * cY - (sZ * sX) * sY;
            ry = (cZ * sX) * sY + sZ * cY;
            rz = -cX * sY;

            fx = -sZ * cX;
            fy = cZ * cX;
            fz = sX;

            ux = (sZ * sX) * cY + cZ * sY;
            uy = sZ * sY - (cZ * sX) * cY;
            uz = cX * cY;

            px = py = pz = 0.0f;
        }

        inline void RotateX(float x)
        {
            float c = cosf(x), s = sinf(x);

            float ry = this->ry, rz = this->rz;
            float uy = this->fy, uz = this->fz;
            float ay = this->uy, az = this->uz;
            float py = this->py, pz = this->pz;

            this->ry = c * ry - s * rz;
            this->rz = c * rz + s * ry;
            this->fy = c * uy - s * uz;
            this->fz = c * uz + s * uy;
            this->uy = c * ay - s * az;
            this->uz = c * az + s * ay;
            this->py = c * py - s * pz;
            this->pz = c * pz + s * py;
        }

        inline void RotateY(float y)
        {
            float c = cosf(y), s = sinf(y);

            float rx = this->rx, rz = this->rz;
            float ux = this->fx, uz = this->fz;
            float ax = this->ux, az = this->uz;
            float px = this->px, pz = this->pz;

            this->rx = c * rx + s * rz;
            this->rz = c * rz - s * rx;
            this->fx = c * ux + s * uz;
            this->fz = c * uz - s * ux;
            this->ux = c * ax + s * az;
            this->uz = c * az - s * ax;
            this->px = c * px + s * pz;
            this->pz = c * pz - s * px;
        }

        inline void RotateZ(float z)
        {
            float c = cosf(z), s = sinf(z);

            float ry = this->ry, rx = this->rx;
            float uy = this->fy, ux = this->fx;
            float ay = this->uy, ax = this->ux;
            float py = this->py, px = this->px;

            this->rx = c * rx - s * ry;
            this->ry = c * ry + s * rx;
            this->fx = c * ux - s * uy;
            this->fy = c * uy + s * ux;
            this->ux = c * ax - s * ay;
            this->uy = c * ay + s * ax;
            this->px = c * px - s * py;
            this->py = c * py + s * px;
        }

        inline void Rotate(float x, float y, float z)
        {
            float cX = cosf(x), sX = sinf(x);
            float cY = cosf(y), sY = sinf(y);
            float cZ = cosf(z), sZ = sinf(z);

            float rx = this->rx, ry = this->ry;
            float rz = this->rz, ux = this->fx;
            float uy = this->fy, uz = this->fz;
            float ax = this->ux, ay = this->uy;
            float az = this->uz, px = this->px;
            float py = this->py, pz = this->pz;

            float x1 = cZ * cY - (sZ * sX) * sY;
            float x2 = (cZ * sX) * sY + sZ * cY;
            float x3 = -cX * sY;
            float y1 = -sZ * cX;
            float y2 = cZ * cX;
            float y3 = sX;
            float z1 = (sZ * sX) * cY + cZ * sY;
            float z2 = sZ * sY - (cZ * sX) * cY;
            float z3 = cX * cY;

            this->rx = x1 * rx + y1 * ry + z1 * rz;
            this->ry = x2 * rx + y2 * ry + z2 * rz;
            this->rz = x3 * rx + y3 * ry + z3 * rz;
            this->fx = x1 * ux + y1 * uy + z1 * uz;
            this->fy = x2 * ux + y2 * uy + z2 * uz;
            this->fz = x3 * ux + y3 * uy + z3 * uz;
            this->ux = x1 * ax + y1 * ay + z1 * az;
            this->uy = x2 * ax + y2 * ay + z2 * az;
            this->uz = x3 * ax + y3 * ay + z3 * az;
            this->px = x1 * px + y1 * py + z1 * pz;
            this->py = x2 * px + y2 * py + z2 * pz;
            this->pz = x3 * px + y3 * py + z3 * pz;
        }
    };

    class RGBA {
    public:
        union
        {
            uint32_t color32;
            struct
            {
                uint8_t r, g, b, a;
            };
            struct
            {
                uint8_t red, green, blue, alpha;
            };
        };

        RGBA(void) {}
        RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
            : r(r)
            , g(g)
            , b(b)
            , a(a)
        {}

        bool operator==(const RGBA& right)
        {
            return this->r == right.r && this->g == right.g && this->b == right.b && this->a == right.a;
        }

        bool operator!=(const RGBA& right)
        {
            return !(*this == right);
        }

        RGBA& operator=(const RGBA& right)
        {
            this->r = right.r;
            this->g = right.g;
            this->b = right.b;
            this->a = right.a;
            return *this;
        }
    };

    class BGRA {
    public:
        union
        {
            uint32_t color32;
            struct
            {
                uint8_t b, g, r, a;
            };
            struct
            {
                uint8_t blue, green, red, alpha;
            };
        };

        BGRA(void) {}
        BGRA(uint8_t b, uint8_t g, uint8_t r, uint8_t a)
            : b(b)
            , g(g)
            , r(r)
            , a(a)
        {}

        bool operator==(const BGRA& right)
        {
            return this->r == right.r && this->g == right.g && this->b == right.b && this->a == right.a;
        }

        bool operator!=(const BGRA& right)
        {
            return !(*this == right);
        }

        BGRA& operator=(const BGRA& right)
        {
            this->r = right.r;
            this->g = right.g;
            this->b = right.b;
            this->a = right.a;
            return *this;
        }
    };
} // namespace Universe::Math