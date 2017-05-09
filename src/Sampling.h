#pragma once

#include <PhotonMath.h>
#include <Vector.h>

namespace Photon {

    /* --------------------------------------------------------------------
           Uniform sampling shapes
    -----------------------------------------------------------------------*/
    inline Point2 sampleUniformDisk(const Point2& rand) {
        Float phi = rand.x * 2.0 * PI;
        Float r = std::sqrt(rand.y);

        return Point2(r * std::cos(phi), r * std::sin(phi));
    }

    inline Float pdfUniformDisk() {
        return 1 / PI;
    }

    inline Point2 sampleConcentricDisk(const Point2& rand) {
        Point2 newRand = (Float)2.0 * rand;
        Point2 offset = Point2(newRand.x - 1.0, newRand.y - 1.0);

        if (offset.x == 0 && offset.y == 0)
            return Point2(0, 0);

        Float theta, r;
        if (std::abs(offset.x) > std::abs(offset.y)) {
            r = offset.x;
            theta = PIOVER4 * (offset.y / offset.x);
        } else {
            r = offset.y;
            theta = PIOVER2 - PIOVER4 * (offset.x / offset.y);
        }

        return r * Point2(std::cos(theta), std::sin(theta));
    }

    inline Float pdfConcentricDisk() {
        return 1 / PI;
    }

    inline Vec3 sampleUniformHemisphere(const Point2& rand) {
        Float r   = Math::sqrtSafe(1.0 - rand.x * rand.x);
        Float phi = 2.0 * PI * rand.y;

        return Vec3(r * std::cos(phi), r * std::sin(phi), rand.x);
    }

    inline Float pdfUniformHemisphere() {
        return INV2PI;
    }

    inline Vec3 sampleCosHemisphere(const Point2& rand) {
        Point2 p = sampleConcentricDisk(rand);

        return Vec3(p.x, p.y, Math::sqrtSafe(1 - p.x * p.x - p.y * p.y));
    }

    inline Float pdfCosHemisphere(Float cos) {
        return INVPI * cos;
    }

    inline Vec3 sampleCosHemispherePowN(const Point2& rand, uint32 N) {
        Float r = std::pow(Math::clamp(1.0 - rand.x * rand.x, 0, 1), (Float)1.0 / (N+1));
        Float phi = 2.0 * PI * rand.x;

        return Vec3(r * std::cos(phi), r * std::sin(phi), rand.x);
    }

    inline Float pdfCosHemispherePowN(Float cos, uint32 N) {
        return INV2PI * (N + 1) * std::pow(cos, N);
    }

    inline Point3 sampleUniformSphere(const Point2& rand) {
        Float rz  = (1.0 - 2.0 * rand.x);
        Float r   = Math::sqrtSafe(1.0 - rz * rz);
        Float phi = 2.0 * PI * rand.y;

        return Point3(r * std::cos(phi), r * std::sin(phi), rz);
    }

    inline Float pdfUniformSphere() { 
        return INV4PI; 
    }

    inline Point2 sampleUniformTriangle(const Point2& rand) {
        Float sqrt = std::sqrt(rand.x);
        return Point2(1 - sqrt, rand.y * sqrt);
    }

    inline Vec3 sampleUniformCone(const Point2& rand, Float cosMax) {
        Float cos = 1.0 - rand.x + rand.x * cosMax;
        Float sin = std::sqrt(1 - cos * cos);
        Float phi = 2.0 * PI * rand.y;

        return Vec3(std::cos(phi) * sin, std::sin(phi) * sin, cos);
    }

    inline Float pdfUniformCone(Float cosMax) {
        return INV2PI / (1.0 - cosMax);
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