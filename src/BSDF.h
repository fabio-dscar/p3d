#pragma once

#include <Vector.h>
#include <Spectral.h>
#include <Texture.h>
//#include <Records.h>

namespace Photon {

    /*struct ShadingInfo {
        Color  ks;
        Color  kd;
        Normal n;
        Float  alpha;
        Float  eta;
    };*/

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
        ALL        = (REFLECTION | REFRACTION | DIFFUSE | SPECULAR | GLOSSY)
    }; 

    /*class BSDFSample {
    public:
        Vec3          wi;
        //Vec3          wo;
        //Point2        rand;
        Float         pdf;
        BSDFType      type;
        Transport     transp;
        const SurfaceEvent* evt;

        BSDFSample(const SurfaceEvent& evt) 
            : pdf(0),
              evt(&evt),
              type(BSDFType::ALL), 
              transp(Transport::RADIANCE) { }
    };*/

    class BSDFSample;

    class BSDF {
    public:
        BSDF(BSDFType type) : _type(type), _bump(nullptr) { }

        virtual Float evalPdf(const BSDFSample& sample) const;
        virtual Color eval   (const BSDFSample& sample) const = 0;
        virtual Color sample (const Point2& rand, BSDFSample* sample) const;
        
        bool isType(BSDFType type) const {
            return (_type & type) == type;
        }

    protected:
        std::shared_ptr<Texture<Float>> _bump;
        const BSDFType _type;
    };

    inline bool hasType(BSDFType toTest, BSDFType type) {
        return (toTest & type) == type;
    }
}