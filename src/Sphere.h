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
            const Vec3 radVec(_radius + F_EPSILON);
            return Bounds3(_pos - radVec, _pos + radVec);
        }

        Float area() const {
            return INV4PI * _radius * _radius;
        }

        void samplePosition(const Point2& rand, PositionSample* sample) const {
            Point3 pos = sampleUniformSphere(rand);

            sample->pos   = _radius * pos + _pos;
            sample->pdf   = 1.0 / area();
            sample->frame = Frame(Normal(pos.x, pos.y, pos.z));
        }

        Float pdfPosition(const PositionSample& sample) const {
            return 1.0 / area();
        }

        void sampleDirect(const Point2& rand, DirectSample* sample) const {
            const RayEvent& ref = *sample->ref;

            Vec3  hatW   = _pos - ref.point;
            Float sinSqr = (_radius * _radius) / hatW.lengthSqr();

            if (sinSqr < 1) {
                // Reference is outside sphere

                Float cosMax = Math::sqrtSafe(1.0 - sinSqr);

                // Sample a direction on the solid angle cone
                const Vec3  wi = sampleUniformCone(rand, cosMax);
                const Frame frame = Frame(Normal(hatW));

                sample->wi  = frame.toWorld(wi);
                sample->pdf = pdfUniformCone(cosMax);

                // Build a ray into the sphere from the reference
                const Ray r = ref.spawnRay(sample->wi);

                // Check for visibility
                if (intersectRay(r, nullptr)) {
                    const Point3 hitp = r.hitPoint();

                    sample->dist   = (hitp - r.origin()).length();
                    sample->normal = Normal(normalize(hitp - _pos));
                } else {
                    sample->pdf = 0;
                }

            } else {
                // Reference is inside sphere

                // Sample a position on the sphere
                PositionSample pos;
                samplePosition(rand, &pos);

                sample->normal = pos.frame.normal();
                sample->wi = pos.pos - sample->ref->point;

                Float distSqr = sample->wi.lengthSqr();

                sample->dist = std::sqrt(distSqr);
                sample->wi.normalize();

                // Convert to solid angle pdf
                Float dot = absDot(sample->normal, -sample->wi);
                sample->pdf = pos.pdf * distSqr / dot;
            }
        }

        Float pdfDirect(const DirectSample& sample) const {
            const Vec3 refToPos = _pos - sample.ref->point;
            Float wLenSqr = refToPos.lengthSqr();
            Float radSqr  = _radius * _radius;

            if (wLenSqr <= radSqr) {
                // Reference is inside sphere
                Float pdfPos = 1.0 / area();

                // Convert area pdf to solid angle density
                Float distSqr = sample.dist * sample.dist;
                Float dot = absDot(sample.normal, -sample.wi);

                return pdfPos * distSqr / dot;
            } else {
                // Compute pdf on cone of directions
                Float sinMaxSqr = radSqr / wLenSqr;
                Float cosMax    = Math::sqrtSafe(1 - sinMaxSqr);

                return pdfUniformCone(cosMax);
            }
        }
       
        void computeSurfaceEvent(const Ray& ray, SurfaceEvent& evt) const {
            Vec3 centerToPt  = ray.hitPoint() - _pos;
            const Vec3 wCenterToPt = normalize(centerToPt);

            // Reproject hit point into sphere
            evt.point = _pos + _radius * wCenterToPt;

            // Deal with sphere parameterization singularity
            if (std::abs(centerToPt.x) < 1e-4 && std::abs(centerToPt.y) < 1e-4)
                centerToPt.x = _radius * F_EPSILON;

            // Compute parameterization angles
            Float theta = Math::acosSafe(centerToPt.z / _radius);
            Float phi   = std::atan2(centerToPt.y, centerToPt.x);
            if (phi < 0)
                phi += 2 * PI;
         
            // Calculate UV coordinates
            evt.uv.x = phi * INV2PI;
            evt.uv.y = theta * INVPI;

            Normal normal = Normal(wCenterToPt);

            Float zr = std::sqrt(centerToPt.x * centerToPt.x + centerToPt.y * centerToPt.y);
            Float invzr  = 1.0 / zr;
            Float cosPhi = centerToPt.x * invzr;
            Float sinPhi = centerToPt.y * invzr;

            // Compute tangent vectors
            Vec3 dpdu = 2 * PI * Vec3(-centerToPt.y, centerToPt.x, 0);
            Vec3 dpdv = PI * Vec3(centerToPt.z * cosPhi, centerToPt.z * sinPhi, -_radius * std::sin(theta));
            
            // Build the shading frame using the partial derivatives
            evt.sFrame = Frame(dpdu, dpdv, normal);
            evt.wo = evt.sFrame.toLocal(-ray.dir());
        }


    private:
        Point3 _pos;
        Float  _radius;
    };

}