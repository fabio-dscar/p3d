#pragma once

#include <Vector.h>
#include <Spectral.h>

namespace Photon {

    class BSDFSample;  // Forward declaration

    enum Transport {
        RADIANCE   = 0,
        IMPORTANCE = 1
    };

    enum BSDFType : uint32 {
        REFLECTION = 0x1,
        REFRACTION = 0x2,
        DIFFUSE    = 0x4,
        SPECULAR   = 0x8,
        GLOSSY     = 0x10,
        NONE       = 0x20,
        ALL        = (REFLECTION | REFRACTION | DIFFUSE | SPECULAR | GLOSSY)
    };
    
    class BSDF {
    public:
        BSDF() : _type(BSDFType::NONE) { }
        BSDF(BSDFType type) : _type(type) { }

        virtual Float evalPdf(const BSDFSample& sample) const;
        virtual Color eval   (const BSDFSample& sample) const = 0;
        virtual Color sample (const Point2& rand, BSDFSample* sample) const;
        
        virtual Float eta() const;

        BSDFType type() const;
        bool isType(BSDFType type) const;

    protected:
        const BSDFType _type;
    };

    inline bool hasType(BSDFType toTest, BSDFType type) {
        return (toTest & type) == type;
    }

    class NullBSDF : public BSDF {
    public:
        NullBSDF() : BSDF(BSDFType::NONE) { }

        Float evalPdf(const BSDFSample& sample) const;
        Color eval(const BSDFSample& sample) const;
        Color sample(const Point2& rand, BSDFSample* sample) const;
    };
}