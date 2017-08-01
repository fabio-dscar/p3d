#pragma once

#include <BSDF.h>
#include <Records.h>
#include <Sampling.h>
#include <Frame.h>

namespace Photon {

    class Phong : public BSDF {
    public:
        Phong(Color kd, Color ks, Float alpha);

        Float evalPdf(const BSDFSample& sample) const;
        Color eval(const BSDFSample& sample) const;
        Color sample(const Point2& rand, BSDFSample* sample) const;

    private:
        Color _kd;
        Color _ks;
        Float _alpha;
        Float _factor;
        Float _factorPdf;
    };

}