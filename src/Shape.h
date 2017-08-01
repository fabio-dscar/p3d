#pragma once

#include <Records.h>
#include <Transform.h>

namespace Photon {

    // Forward declarations
    class Ray;
    class Bounds3;
    class Light;
    class AreaLight;
    class BSDF;

    class Shape {
    public:
        Shape();
        Shape(const Transform& objToWorld);
        Shape(const AreaLight* light);

        void setTransform(const Transform& objToWorld);
        void setTransform(const Transform& objToWorld, const Transform& worldToObj);

        const BSDF* bsdf() const;
        void setBsdf(const BSDF* bsdf);

        bool isLight() const;
        void setLight(const AreaLight* light);
        const AreaLight* areaLight() const;
        
        virtual bool intersectRay(const Ray& ray, SurfaceEvent* evt) const;
        virtual bool isOccluded(const Ray& ray) const;
        virtual void computeSurfaceEvent(const Ray& ray, SurfaceEvent& evt) const;

        virtual Bounds3 bbox() const;
        virtual Float area() const;

        // -------------------------------------------------------------
        //      Shape sampling
        // -------------------------------------------------------------
        virtual void  samplePosition(const Point2& rand, PositionSample* sample) const;
        virtual Float pdfPosition(const PositionSample& sample) const;

        virtual void  sampleDirect(const Point2& rand, DirectSample* sample) const;
        virtual Float pdfDirect(const DirectSample& sample) const;

    protected:
        Transform _objToWorld;
        Transform _worldToObj;
        //std::shared_ptr<Transform> _objToWorld;
        //std::shared_ptr<Transform> _worldToObj;

    private:
        const BSDF* _bsdf;
        const AreaLight* _light;
        bool _twoSided;
    };

}