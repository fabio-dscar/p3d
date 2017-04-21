#pragma once

#include <Vector.h>
#include <Light.h>
#include <Sampling.h>

namespace Photon {

    class PointLight : public Light {
    public:
        PointLight() : Light() { }
        PointLight(const Point3& pos) : Light(pos) { }
        PointLight(const Point3& pos, const Color& color) : Light(pos, color) { }

        virtual bool isDelta() const {
            return true;
        }

        void sampleLi(const RayEvent& ref, const Point2& rand, Point3* pos, Float* pdf) const {
            *pos = _pos;
            *pdf = 1;
        }

        Color evalL(const SurfaceEvent& it, const Vec3& wo) const {
            return Color::BLACK;
        }

        Color samplePosition(const Point2& rand, PositionSample* sample) const {
            sample->pos = _pos;
            sample->pdf = 1.0;
            return (4 * PI) * _Le;
        }

        Float pdfPosition(const PositionSample& sample) const {
            return 1.0;
        }

        Color sampleDirect(const Point2& rand, DirectSample* sample) const {
            Vec3 refToPos = _pos - sample->ref->point();
            
            sample->wi     = normalize(refToPos);
            sample->dist   = refToPos.length();
            sample->pdf    = 1.0;
            sample->normal = Normal(-sample->wi);

            Float invDist = 1.0 / sample->dist;

            return (invDist * invDist) * _Le;
        }

        Float pdfDirect(const DirectSample& sample) const {
            return 1.0;
        }

        Color sampleEmitDirection(const Point2& rand, const PositionSample& pos, DirectionSample* sample) const {
            sample->wo  = sampleUniformSphere(rand).posVec();
            sample->pdf = INV4PI;
            
            return Color(1);
        }

        Float pdfEmitDirection(const PositionSample& pos, const DirectionSample& sample) const {
            return INV4PI;
        }

    private:
        Point3 _pos;
    };

}