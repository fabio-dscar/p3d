#pragma once

#include <PhotonMath.h>
#include <Vector.h>

namespace Photon {

    /* --------------------------------------------------------------------
           Uniform sampling shapes
    -----------------------------------------------------------------------*/
    inline Point3 sampleUniformHemisphere(const Point2& rand) {
        Float r   = std::sqrt(std::max((Float)0, (Float)1.0 - rand[0] * rand[0]));
        Float phi = 2.0 * PI * rand[1];
        return Point3(r * std::cos(phi), r * std::sin(phi), rand[0]);
    }

    inline Float pdfUniformHemisphere() {
        return INV2PI;
    }

    inline Point3 sampleUniformSphere(const Point2& rand) {
        /*Float z = 1 - 2 * rand[0];
        Float r = std::sqrt(std::max((Float)0, (Float)1 - z * z));
        Float phi = 2 * PI * rand[1];
        return Point3(r * std::cos(phi), r * std::sin(phi), z);*/
        Float r   = 2.0 * std::sqrt(rand.x * (1.0 - rand.x));
        Float rz  = (1.0 - 2.0 * rand.x);
        Float phi = 2.0 * PI * rand.y;

        return Point3(r * std::cos(phi),
                      r * std::sin(phi),
                      rz);
    }

    inline Float pdfUniformSphere() { 
        return INV4PI; 
    }

    inline Point2 sampleUniformTriangle(const Point2& rand) {
        Float su0 = std::sqrt(rand[0]);
        return Point2(1 - su0, rand[1] * su0);
    }

    inline Vec3 sampleUniformSphericalCap(const Point2& rand, Float cosThetaMax) {
        Float phi = rand.y * 2.0 * PI;
        /*float z   = rand.y * (1.0 - cosThetaMax) + cosThetaMax;
        float r   = std::sqrt(std::max(1.0 - z * z, 0.0));
        
        return Vec3(r * std::sin(phi), r * std::cos(phi), z);*/
        Float z = 1.0 - rand.x + rand.x * cosThetaMax;
        Float sinTheta = std::sin(std::acos(z));
        
        //return Vec3(r * std::sin(phi), r * std::cos(phi), z);
        return Vec3(sinTheta * std::cos(phi), sinTheta * std::sin(phi), z);

    }

    inline Float pdfUniformSphericalCap(Float cosThetaMax) {
        return INV2PI / (1.0 - cosThetaMax);
    }

    inline Point2 sampleUniformDisk(const Point2& rand) {
        Float phi = rand.x * 2.0 * PI;
        Float r   = std::sqrt(rand.y);
        return Point2(r * std::cos(phi), r * std::sin(phi));
    }

    /* --------------------------------------------------------------------
            MIS Heuristics
    -----------------------------------------------------------------------*/
    inline Float powerHeuristicBetaTwo(Float pi, Float pj, int ni, int nj) {
        Float i = pi * ni;
        Float j = pj * nj;
        return (i * i) / (i * i + j * j);
    }

    inline Float balanceHeuristic(Float pi, Float pj, int ni, int nj) {
        return (ni * pi) / (ni * pi + nj * pj);
    }

}