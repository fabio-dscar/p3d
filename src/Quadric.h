#pragma once

#include <PhotonMath.h>
#include <Shape.h>

namespace Photon {

    class Quadric : public Shape {
    public:
        Quadric(Float A, Float B, Float C, Float D, Float E, Float F, Float G, Float H, Float I, Float J) :
            A(A), B(B), C(C), D(D), E(E), F(F), G(G), H(H), I(I), J(J) {
        
            _bounds = Bounds3(Point3(-0.5, -0.5, -0.5), Point3(0.5, 0.5, 0.5));
        }

        virtual Bounds3 bbox() const {
            return _bounds;
            //return Bounds3::UNBOUNDED;
        }

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const {
            const Point3 o = ray.origin();
            const Vec3 d = ray.dir();

            Float dx2 = d.x * d.x;
            Float dy2 = d.y * d.y;
            Float dz2 = d.z * d.z;

            Float ox2 = o.x * o.x;
            Float oy2 = o.y * o.y;
            Float oz2 = o.z * o.z;

            Float a = A * dx2 + B * dy2 + C * dz2 +
                      D * d.x * d.y + E * d.y * d.z + F * d.x * d.z;

            Float b = 2.0 * (G * d.x + H * d.y + I * d.z +
                             D * (o.x * d.y + o.y * d.x) +
                             E * (o.y * d.z + o.z * d.y) +
                             F * (o.x * d.z + o.z * d.x) +
                             A * d.x * o.x +
                             B * d.y * o.y +
                             C * d.z * o.z);

            Float c = J + A * ox2 + B * oy2 + C * oz2 +
                      2 * (G * o.x + H * o.y + I * o.z +
                           D * o.x * o.y + E * o.y * o.z + F * o.x * o.z);
                     
            Float disc = b * b - 4.0 * a * c;
            if (disc < 0)
                return false;

            Float denom = 1.0 / (2.0 * a);
            Float sqrtDisc = std::sqrt(disc);
            Float t1, t2;
            t1 = (-b - sqrtDisc) * denom;
            t2 = (-b + sqrtDisc) * denom;

            if (t2 < t1)
                std::swap(t1, t2);

            if (ray.inRange(t1)) {
                Point3 pt = ray(t1);
                if (_bounds.contains(pt)) {
                    ray.setMaxT(t1);
                    evt->setEvent(ray, this, calcNormal(pt));
                    return true;
                }
            }
          
            if (ray.inRange(t2)) {
                Point3 pt = ray(t2);
                if (_bounds.contains(pt)) {
                    ray.setMaxT(t2);
                    evt->setEvent(ray, this, calcNormal(pt));
                    return true;
                }
            }

            return false;
        }

        bool isOccluded(const Ray& ray) const {
            Point3 o = ray.origin();
            Vec3 d = ray.dir();

            Float dx2 = d.x * d.x;
            Float dy2 = d.y * d.y;
            Float dz2 = d.z * d.z;

            Float ox2 = o.x * o.x;
            Float oy2 = o.y * o.y;
            Float oz2 = o.z * o.z;

            Float a = A * dx2 + B * dy2 + C * dz2 +
                D * d.x * d.y + E * d.y * d.z + F * d.x * d.z;

            Float b = 2.0 * (G * d.x + H * d.y + I * d.z +
                             D * (o.x * d.y + o.y * d.x) +
                             E * (o.y * d.z + o.z * d.y) +
                             F * (o.x * d.z + o.z * d.x) +
                             A * d.x * o.x +
                             B * d.y * o.y +
                             C * d.z * o.z);

            Float c = J + A * ox2 + B * oy2 + C * oz2 +
                2 * (G * o.x + H * o.y + I * o.z +
                     D * o.x * o.y + E * o.y * o.z + F * o.x * o.z);

            Float disc = b * b - 4.0 * a * c;
            if (disc < 0)
                return false;

            Float denom = 1.0 / (2.0 * a);
            Float t1, t2;
            t1 = (-b - std::sqrt(disc)) * denom;
            t2 = (-b + std::sqrt(disc)) * denom;

            if (t2 < t1)
                std::swap(t1, t2);

            if (ray.inRange(t1)) {
                if (_bounds.contains(ray(t1)))
                    return true;
            }

            if (ray.inRange(t2))
                if (_bounds.contains(ray(t2)))
                    return true;

            return false;
        }

    private:
        Normal calcNormal(const Point3& pt) const {
            return normalize(Normal(2.0 * (A * pt.x + D * pt.y + F * pt.z + G),
                                    2.0 * (B * pt.y + D * pt.x + E * pt.z + H),
                                    2.0 * (C * pt.z + E * pt.y + F * pt.x + I)));
        }

        Float A, B, C, D, E, F, G, H, I, J;
        Bounds3 _bounds;
    };

}