#include <Sphere.h>

using namespace Photon;

const Point3& Sphere::pos() const {
    return _pos;
}

Float Sphere::radius() const {
    return _radius;
}

bool Sphere::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    Vec3 p = ray.origin() - _pos;

    // Solve quadratic
    Float B = dot(p, ray.dir());
    Float C = p.lengthSqr() - _radius * _radius;
    Float detSq = B * B - C;
    if (detSq >= 0.0f) {
        Float det = std::sqrt(detSq);

        Float t = -B - det;
        if (ray.inRange(t)) {
            ray.setMaxT(t);
            evt->setEvent(ray, this, Normal(normalize(ray(t) - _pos)));
            return true;
        }

        t = -B + det;
        if (ray.inRange(t)) {
            ray.setMaxT(t);
            evt->setEvent(ray, this, -Normal(normalize(ray(t) - _pos)));
            evt->setBackface(true);  // Hit from inside sphere
            return true;
        }
    }

    return false;
}

bool Sphere::isOccluded(const Ray& ray) const {
    Vec3 p = ray.origin() - _pos;

    // Solve quadratic
    Float B = dot(p, ray.dir());
    Float C = p.lengthSqr() - _radius * _radius;
    Float detSq = B*B - C;
    if (detSq >= 0.0f) {
        Float det = std::sqrt(detSq);
        Float t = -B - det;

        if (ray.inRange(t))
            return true;

        t = -B + det;
        if (ray.inRange(t))
            return true;
    }

    return false;
}