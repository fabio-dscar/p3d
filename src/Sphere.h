#pragma once

#include <Vector.h>
#include <Shape.h>
#include <Bounds.h>
#include <Sampling.h>
#include <Frame.h>

namespace Photon {

    class Sphere : public Shape {
    public:
        Sphere(const Point3& position, Float radius)
            : _pos(position), _radius(radius) { }
        
        const Point3& pos() const;
        Float radius() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const;
        bool isOccluded(const Ray& ray) const;

        Bounds3 bbox() const {
            Vec3 radVec(_radius + F_EPSILON);

            return Bounds3(_pos - radVec, _pos + radVec);
        }

        Float area() const {
            return 4.0 * PI * _radius * _radius;
        }

        RayEvent samplePosition(const RayEvent& ref, const Point2& rand, Float* pdf) const {
            Vec3 hatW = _pos - ref.point();
            Float A = (_radius * _radius) / hatW.lengthSqr();
            Float cosThetaMax = std::sqrt(std::max(0.0, 1.0 - A));

            Float q2 = pdfUniformSphericalCap(cosThetaMax);
            Vec3 wi = sampleUniformSphericalCap(rand, cosThetaMax);

            hatW.normalize();
            Frame frame = Frame(Normal(hatW));
            Vec3 twi = frame.toWorld(wi);

            Ray r = Ray(ref.point(), twi);
            SurfaceEvent evt;
            if (intersectRay(r, &evt)) {
                Float rayLenSqr = (evt.point() - r.origin()).lengthSqr();
                *pdf = q2 * dot(evt.wo(), evt.normal()) / rayLenSqr;
            } else {
                *pdf = 0;
            }

            return evt;
        }

        Float pdfPosition(const RayEvent& evt) const {
            return 1.0 / area();
        }

    private:
        Point3 _pos;
        Float _radius;
    };

}