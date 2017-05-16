#pragma once

#include <Vector.h>

#include <Ray.h>
#include <BSDF.h>

namespace Photon {

    class RayEvent;
    class SurfaceEvent;

    class PositionSample {
    public:
        Point3 pos;
        Float  pdf;
        Frame  frame;
        //Normal normal;

        PositionSample() : pdf(0) {}
        PositionSample(const RayEvent& evt) : pdf(0) {
            pos   = evt.point;
            frame = Frame(evt.normal);
        }

        PositionSample(const SurfaceEvent& evt) : pdf(0) {
            pos   = evt.point;
            frame = evt.sFrame;
        }

        Normal normal() const {
            return frame.normal();
        }
    };

    class DirectionSample {
    public:
        Vec3  wo;
        Float pdf;

        DirectionSample() : pdf(0) {}
    };

    class DirectSample {
    public:
        Vec3            wi;
        Float           pdf;
        Float           dist;
        Normal          normal;
        const RayEvent* ref;

        DirectSample() : ref(nullptr), pdf(0), dist(0) {}
        DirectSample(const RayEvent& ref) : ref(&ref), pdf(0), dist(0) {}
        DirectSample(const RayEvent& ref, const RayEvent& local) : ref(&ref), pdf(0) {
          
            wi     = -local.wo;
            normal = local.normal;
            dist   = (ref.point - local.point).length();
        }

        Point3 hitPoint() const {
            return ref->point + dist * wi;
        }
    };
        
    class BSDFSample {
    public:
        Vec3      wi;
        Vec3      wo;
        Float     pdf;
        Float     eta;
        BSDFType  type;
        Transport transp;
        const SurfaceEvent* evt;

        BSDFSample(const SurfaceEvent& evt)
            : wo(evt.wo),
              pdf(0),
              eta(1.0),
              evt(&evt),
              type(BSDFType::ALL),
              transp(Transport::RADIANCE) {}

        BSDFSample(const SurfaceEvent& evt, const Vec3& worldWi, Transport mode)
            : wo(evt.wo),
              pdf(0),
              eta(1.0),
              evt(&evt),
              type(BSDFType::ALL),
              transp(mode) {
        
            // Set wi in shading frame space
            wi = evt.sFrame.toLocal(worldWi);
        }

        BSDFSample(const DirectSample& sample) 
            : wo(sample.ref->wo),
              pdf(0),
              eta(1.0),
              evt((SurfaceEvent*)sample.ref),
              type(BSDFType::ALL),
              transp(Transport::RADIANCE) {

            // Set wi in shading frame space
            wi = evt->sFrame.toLocal(sample.wi);
        }
    };

}