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

            Vec3  hatW   = _pos - ref.point;
            Float sinSqr = (_radius * _radius) / hatW.lengthSqr();
            Float cosMax = Math::sqrtSafe(1.0 - sinSqr); // std::sqrt(std::max(0.0, 1.0 - A));

            // Sample a direction on the solid angle cone
            Vec3  wi    = sampleUniformCone(rand, cosMax);
            Frame frame = Frame(Normal(normalize(hatW)));

            sample->wi  = frame.toWorld(wi);
            sample->pdf = pdfUniformCone(cosMax);

            /*Float B = dot(-hatW, sample->wi);
            Float C = hatW.lengthSqr() - _radius * _radius;
            Float detSq = B * B - C;
            if (detSq >= 0.0) {
                Float det = std::sqrt(detSq);

                Float t0 = -B - det;
                Float t1 = -B + det;
                if (t1 < t0)
                    std::swap(t0, t1);

                Point3 pt = ref.point + t0 * sample->wi;
                Vec3 refToPt = pt - ref.point;

                sample->dist = refToPt.length();
                sample->normal = normalize(Normal(pt - _pos));
            } else {
                std::cout << "CASF";
                sample->pdf = 0;
            }*/

            // Build a ray into the sphere from the reference
            Ray r = ref.spawnRay(sample->wi);

            // Check for visibility
            SurfaceEvent intr;
            if (intersectRay(r, &intr)) {
                sample->dist   = (intr.point - r.origin()).length();
                sample->normal = intr.normal;
            } else {
                std::cout << "CASF";
                sample->pdf = 0;
            }
        }

        Float pdfDirect(const DirectSample& sample) const {
            Vec3 _refToPos = _pos - sample.ref->point;

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

            evt.point = hitp;

           if (std::abs(centerToPt.x) < 0.001 && std::abs(centerToPt.y) < 0.001)
                centerToPt.x = _radius * F_EPSILON;

            Float theta = Math::acosSafe(centerToPt.z / _radius);
            Float phi   = std::atan2(centerToPt.y, centerToPt.x);
            if (phi < 0)
                phi += 2 * PI;
         
            // Set UV coordinates
            evt.uv.x = phi * INV2PI;
            evt.uv.y = theta * INVPI;

            Vec3   dpdu   = 2 * PI * Vec3(-centerToPt.y, centerToPt.x, 0);
            Normal normal = Normal(normalize(centerToPt));

            Float zr = std::sqrt(centerToPt.x * centerToPt.x + centerToPt.y * centerToPt.y);

            Float invzr  = 1.0 / zr;
            Float cosPhi = centerToPt.x * invzr;
            Float sinPhi = centerToPt.y * invzr;

            Vec3 dpdv = PI * Vec3(centerToPt.z * cosPhi, centerToPt.z * sinPhi, -_radius * std::sin(theta));
            
            evt.sFrame = Frame(normalize(dpdu), normalize(dpdv), normal);
            evt.wo = evt.sFrame.toLocal(-ray.dir());

            /*if (zr > 0) {
                Float invzr = 1.0 / zr;
                Float cosPhi = centerToPt.x * invzr;
                Float sinPhi = centerToPt.y * invzr;

                dpdv = PI * Vec3(centerToPt.z * cosPhi, centerToPt.z * sinPhi, -_radius * std::sin(theta));

                evt.sFrame = Frame(normalize(dpdu), normalize(dpdv), normal);
                evt.wo = evt.sFrame.toLocal(-ray.dir());
            } else {
                dpdv = PI * Vec3(0, centerToPt.z, -_radius * std::sin(theta));

                evt.sFrame = Frame(normal);
                evt.wo = evt.sFrame.toLocal(-ray.dir());
            }*/

            

            //evt.setGeoFrame(Frame(normal));
        }


    private:
        Point3 _pos;
        Float  _radius;
    };

}