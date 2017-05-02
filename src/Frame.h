#pragma once

#include <Vector.h>

namespace Photon {

    class Frame {
    public:
        Frame();
        Frame(const Normal& norm);
        Frame(const Vec3& x, const Vec3& y, const Vec3& z);

        Vec3 toWorld(const Vec3& vec) const;
        Vec3 toLocal(const Vec3& vec) const;

        Normal normal() const {
            return Normal(_z);
        }

        Vec3 x() const {
            return _x;
        }

        Vec3 y() const {
            return _y;
        }

        Vec3 z() const {
            return _z;
        }

        bool consistent();

        static Vec3 reflect(const Vec3& wi);
        static Vec3 reflect(const Vec3& wi, const Vec3& n);

        static Vec3 refract(const Vec3& wi, Float eta, Float cosT);
        static Vec3 refract(const Vec3& wi, Float intEta, Float extEta, Float cosT);
        static Vec3 refract(const Vec3& wi, const Normal& n, Float eta, Float cosT);
        static Vec3 refract(const Vec3& wi, const Normal& n, Float eta);
        //static bool refract(const Vec3& wi, Vec3* wt, const Normal& n, Float eta);

        static bool sameSide(const Vec3& w1, const Vec3& w2);
        static bool onPositiveHemisphere(const Vec3& w);

        // Frame spherical functions
        static Float cosTheta(const Vec3& w);
        static Float cosThetaSqr(const Vec3& w);
        static Float absCosTheta(const Vec3& w);

        static Float sinTheta(const Vec3& w);
        static Float sinThetaSqr(const Vec3& w);

        static Float tanTheta(const Vec3& w);
        static Float tanThetaSqr(const Vec3& w);

        static Float cosPhi(const Vec3& w);
        static Float sinPhi(const Vec3& w);

        static Float cosPhiSqr(const Vec3& w);
        static Float sinPhiSqr(const Vec3& w);

        static Float cosAng(const Vec3& w1, const Vec3& w2);

    private:
        Vec3 _x, _y, _z;
    };

};