namespace Photon {
    namespace Math {

        inline Quat operator*(Float scalar, const Quat& quat) {
            return quat * scalar;
        }

        inline Quat operator/(Float scalar, const Quat& quat) {
            return quat / scalar;
        }

        inline Quat normalize(const Quat& quat) {
            Float dotq = dot(quat, quat);
            return quat / std::sqrt(dotq);
        }

        inline Float dot(const Quat& quat1, const Quat& quat2) {
            return dot(quat1.vec, quat2.vec) + quat1.w * quat2.w;
        }

        inline Quat slerp(Float t, const Quat& quat1, const Quat& quat2) {
            Float dotp = dot(quat1, quat2);

            if (dotp > 0.999) {
                return normalize((1.0 - t) * quat1 + t * quat2);
            } else {
                Float clampCos = clamp(dotp, -1, 1);
                Float theta = acosSafe(clampCos);
                Quat aux = normalize(quat2 - quat1 * dotp);

                return aux * std::sin(theta * t) + quat1 * std::cos(theta * t);
            }
        }
    }
}
