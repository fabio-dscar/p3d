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

        void  samplePosition(const Point2& rand, PositionSample* sample) const {
            sample->pos = Point3(0);
        }

        Float pdfPosition(const PositionSample& sample) const {
            return 1.0 / area();
        }


        void  sampleDirect(const Point2& rand, DirectSample* sample) const {
            const RayEvent& ref = *sample->ref;

            Vec3  hatW   = _pos - ref.point();
            Float sinSqr = (_radius * _radius) / hatW.lengthSqr();
            Float cosMax = Math::sqrtSafe(1.0 - sinSqr); // std::sqrt(std::max(0.0, 1.0 - A));

            // Sample a direction on the solid angle cone
            Vec3  wi    = sampleUniformCone(rand, cosMax);
            Frame frame = Frame(Normal(normalize(hatW)));

            sample->wi  = frame.toWorld(wi);
            sample->pdf = pdfUniformCone(cosMax);

            // Build a ray into the sphere from the reference
            Ray r = Ray(ref.point(), sample->wi);

            // Check for visibility
            SurfaceEvent intr;
            if (intersectRay(r, &intr)) {
                sample->dist   = (intr.point() - r.origin()).length();
                sample->normal = intr.normal();
            } else {
                std::cout << "CASF";
                sample->pdf = 0;
            }
        }

        Float pdfDirect(const DirectSample& sample) const {
            Vec3 _refToPos = _pos - sample.ref->point();

            if (_refToPos.lengthSqr() <= _radius * _radius)
                return 0;

            // Compute general sphere PDF
            Float sinMaxSqr = _radius * _radius / _refToPos.lengthSqr();
            Float cosMax    = std::sqrt(std::max((Float)0, 1 - sinMaxSqr));

            return pdfUniformCone(cosMax);
        }
       
        void computeSurfaceEvent(const Ray& ray, SurfaceEvent& evt) const {
            Point3 hitp = ray.hitPoint();
            Vec3 centerToPt = hitp - _pos;

            hitp = _pos + _radius * normalize(centerToPt);

            evt.setPoint(hitp);

            Float theta = Math::acosSafe(centerToPt.z / _radius);
            Float phi   = std::atan2(centerToPt.y, centerToPt.x);
            phi += (phi < 0) ? 2 * PI : 0;
         
            // Set UV coordinates
            evt.setUV(Point2(phi * (INVPI * 0.5), theta * INVPI));

            Vec3   dpdu   = 2 * PI * Vec3(-centerToPt.y, centerToPt.x, 0);
            Normal normal = Normal(normalize(centerToPt));

            Float zr = std::sqrt(centerToPt.x * centerToPt.x + centerToPt.y * centerToPt.y);

            Vec3 dpdv;
            if (zr == 0) {
                Float invzr = 1.0 / zr;
                Float cosPhi = centerToPt.x * invzr;
                Float sinPhi = centerToPt.y * invzr;

                dpdv = PI * Vec3(centerToPt.z * cosPhi, centerToPt.z * sinPhi, -_radius * std::sin(theta));

                Frame geoFrame(normalize(dpdu), normalize(dpdv), normal);
                evt.setGeoFrame(geoFrame);
            } else {
                dpdv = PI * Vec3(0, centerToPt.z, -_radius * std::sin(theta));

                Frame geoFrame(normal);
                evt.setGeoFrame(geoFrame);
            }

            

            //evt.setGeoFrame(Frame(normal));
        }


    private:
        Point3 _pos;
        Float  _radius;
    };

}