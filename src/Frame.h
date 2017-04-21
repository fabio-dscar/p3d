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

        bool consistent();

        static Vec3 reflect(const Vec3& wi);

        static bool refract(const Vec3& wi, Vec3* wtr, const Normal& normal, Float eta) {
            /*Float etaSqr = eta * eta;

            // Compute $\cos \theta_\roman{t}$ using Snell's law
            Float cosThetaI  = dot(normal, wi);
            Float sin2ThetaI = std::max((Float)0, 1.0 - cosThetaI * cosThetaI);
            Float sin2ThetaT = etaSqr * sin2ThetaI;

            // Handle total internal reflection for transmission
            if (sin2ThetaT >= 1) 
                return false;

            Float cosThetaT = std::sqrt(1 - sin2ThetaT);
            *wtr = eta * -wi + (eta * cosThetaI - cosThetaT) * Vec3(normal);

            return true;*/

            // Compute $\cos \theta_\roman{t}$ using Snell's law
            Float cosThetaI = dot(normal, wi);
            Float sin2ThetaI = std::max(Float(0), Float(1 - cosThetaI * cosThetaI));
            Float sin2ThetaT = eta * eta * sin2ThetaI;

            // Handle total internal reflection for transmission
            if (sin2ThetaT >= 1) 
                return false;

            Float cosThetaT = std::sqrt(1 - sin2ThetaT);

            *wtr = Vec3(eta * -wi + (eta * cosThetaI - cosThetaT) * Vec3(normal));

            return true;
        }

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