#pragma once

#include <BSDF.h>

namespace Photon {

    enum DistributionType {
        PHONG = 0,
        BECKMANN = 1,
        GGX = 2
    };

    class MicrofacetDist {
    public:
        MicrofacetDist() : _type(GGX), _alphaUV(1e-5, 1e-5), _phongExp(0) {}
        MicrofacetDist(DistributionType type, const Vec2& alpha);

        DistributionType type() const;

        Float evalD(const Vec3& wh) const;
        Float evalG1(const Vec3& w, const Vec3& wh) const;
        Float evalG(const Vec3& wi, const Vec3& wo, const Vec3& wh) const;
        Float evalLambda(const Vec3& w) const;
        Float evalPdf(const Vec3& wi, const Vec3& wh) const;

        Normal sample(const Vec3& wi, const Point2& rand) const;

    private:
        Float unitToAlpha(Float rough);
        
        Normal samplePhong(const Vec3& wi, const Point2& rand) const;
        Normal sampleVisible(const Vec3& wi, const Point2& rand) const;

        Vec2 sampleP22(Float theta, const Point2& rand) const;

        DistributionType _type;
        Vec2 _alphaUV;
        Vec2 _phongExp;
    };

    Float computeBeckmannExp(const Vec3& wh, const Vec2& alphaUV);
    Float interpolateAlpha(const Vec3& w, const Vec2& alphaUV);

    Float distPhong(const Vec3& wh, const Vec2& alphaUV, const Vec2& phongExp);
    Float distPhong(const Vec3& wh, const Vec2& alphaUV);
    Float distBeckmann(const Vec3& wh, const Vec2& alphaUV);
    Float distGGX(const Vec3& wh, const Vec2& alphaUV);
    
    Float lambdaBeckmann(const Vec3& w, const Vec2& alphaUV);
    Float lambdaGGX(const Vec3& w, const Vec2& alphaUV);

}