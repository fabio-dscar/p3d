#pragma once

#include <BSDF.h>
#include <Texture.h>

namespace Photon {

    class Bump : public BSDF {
    public:
        Bump(const BSDF& bsdf) 
            : BSDF(bsdf.type()), _bump(nullptr) { }

        Float evalPdf(const BSDFSample& sample) const {

        }

        Color eval(const BSDFSample& sample) const {

        }

        Color sample(const Point2& rand, BSDFSample* sample) const {
            
        }

    private:
        std::shared_ptr<Texture<Float>> _bump;
        const BSDF* _bsdf;
    };

}