#pragma once

#include <PhotonMath.h>
#include <Vector.h>
#include <Transform.h>

namespace Photon {
    namespace Math {

        class Quat {
        public:
            Vec3  vec;
            Float w;

            Quat() : vec(0, 0, 0), w(1) { }
            Quat(Float x, Float y, Float z, Float w)
                : vec(x, y, z), w(w) { }
            Quat(const Vec3& vec, Float w)
                : vec(vec), w(w) { }
            Quat(const Transform& transform);

            Quat  operator+ (const Quat& quat) const;
            Quat& operator+=(const Quat& quat);

            Quat  operator- (const Quat& quat) const;
            Quat& operator-=(const Quat& quat);

            Quat  operator* (Float scalar) const;
            Quat& operator*=(Float scalar);

            Quat  operator/ (Float scalar) const;
            Quat& operator/=(Float scalar);

            Transform transform() const;
        };

        inline Quat operator*(Float scalar, const Quat& quat);
        inline Quat operator/(Float scalar, const Quat& quat);

        inline Quat normalize(const Quat& quat);

        inline Float dot(const Quat& quat1, const Quat& quat2);

        inline Quat slerp(Float t, const Quat& quat1, const Quat& quat2);
    }
}

#include <Quat.inl>