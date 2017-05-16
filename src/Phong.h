#pragma once

#include <BSDF.h>
#include <Records.h>
#include <Sampling.h>
#include <Frame.h>

namespace Photon {

    class Phong : public BSDF {
    public:
        Phong(Color kd, Color ks, Float alpha) 
            : BSDF(BSDFType(REFLECTION | GLOSSY | DIFFUSE)), 
            _kd(kd), _ks(ks), _alpha(alpha) { 

            _factor = (alpha + 2.0) * INV2PI;
            _factorPdf = (alpha + 1.0) / (2.0 * PI);
        }

        Float evalPdf(const BSDFSample& sample) const {
            if (!hasType(sample.type, GLOSSY) &&
                !hasType(sample.type, DIFFUSE)) {
                return 0;
            }

            const Vec3 wi = sample.wi;
            const Vec3 wo = sample.wo;

            bool diff = hasType(sample.type, DIFFUSE);
            bool spec = hasType(sample.type, GLOSSY);
            bool both = diff && spec;

            Float diffPdf = 0;
            Float specPdf = 0;
            Float sampleSpec = _ks.lum() / (_ks.lum() + _kd.lum());

            if (diff)
                diffPdf += pdfCosHemisphere(Frame::cosTheta(wi));

            if (spec) {
                const Vec3 refl = Frame::reflect(sample.wo);

                Float dotSpec = dot(sample.wi, refl);
                if (dotSpec > 0)
                    specPdf += _factorPdf * std::pow(dotSpec, _alpha);
            }

            if (both)
                return sampleSpec * specPdf + (1 - sampleSpec) * diffPdf;
            else if (diff)
                return diffPdf;
            else
                return specPdf;
        }

        Color eval(const BSDFSample& sample) const {
            if (!hasType(sample.type, GLOSSY) &&
                !hasType(sample.type, DIFFUSE)) {
                return Color::BLACK;
            }

            const Vec3 wi = sample.wi;
            const Vec3 wo = sample.wo;

            if (!Frame::onPositiveHemisphere(wi) ||
                !Frame::onPositiveHemisphere(wo)) {
                return Color::BLACK;
            }

            Color f = Color::BLACK;

            if (hasType(sample.type, GLOSSY)) {
                const Vec3 refl = Frame::reflect(sample.wi);

                Float dotSpec = dot(sample.wo, refl);
                if (dotSpec > 0)
                    f += _ks * _factor * std::pow(dotSpec, _alpha);
            }

            if (hasType(sample.type, DIFFUSE))
                f += _kd * INVPI;

            return f;
        }

        Color sample(const Point2& rand, BSDFSample* sample) const {
            if (!hasType(sample->type, GLOSSY) &&
                !hasType(sample->type, DIFFUSE)) {
                return 0;
            }

            Point2 u = rand;

            bool diff = hasType(sample->type, DIFFUSE);
            bool both = diff && hasType(sample->type, GLOSSY);

            Float sampleSpec = _ks.lum() / (_ks.lum() + _kd.lum());
            if (!both)
                sampleSpec = diff ? 0 : 1;

            if (sampleSpec > u.x) {
                u.x = u.x / sampleSpec; // Reuse sample

                sample->type = BSDFType(REFLECTION | GLOSSY);

                const Vec3 refl = Frame::reflect(sample->wo);

                // Sample Phong specular lobe
                Float sinAlpha = std::sqrt(1 - std::pow(u.y, 2 / (_alpha + 1)));
                Float cosAlpha = std::pow(u.y, 1 / (_alpha + 1));
                Float phi = (2.0 * PI) * u.x;
                Vec3 localDir = Vec3(
                    sinAlpha * std::cos(phi),
                    sinAlpha * std::sin(phi),
                    cosAlpha
                );

                Frame frame = Frame(Normal(refl));  
                sample->wi  = frame.toWorld(localDir);

            } else {
                u.x = (u.x - sampleSpec) / (1 - sampleSpec); // Reuse sample

                sample->type = BSDFType(REFLECTION | DIFFUSE);
                sample->wi   = sampleCosHemisphere(u);
            }

            if (!Frame::onPositiveHemisphere(sample->wi))
                return Color::BLACK;

            sample->pdf = evalPdf(*sample);
            if (sample->pdf == 0)
                return Color::BLACK;
            
            return eval(*sample);
        }

    private:
        Color _kd;
        Color _ks;
        Float _alpha;
        Float _factor;
        Float _factorPdf;
    };

}