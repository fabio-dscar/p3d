#include <Sphere.h>

using namespace Photon;

const Point3& Sphere::pos() const {
    return _pos;
}

Float Sphere::radius() const {
    return _radius;
}

bool Sphere::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    Vec3 cdir = ray.origin() - _pos;

    // Solve quadratic
    Float B = dot(cdir, ray.dir());
    Float C = cdir.lengthSqr() - _radius * _radius;
    Float detSq = B * B - C;
    if (detSq >= 0.0) {
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
            evt->setEvent(ray, this, Normal(normalize(ray(t) - _pos)));
            evt->backface = true;  // Hit from inside sphere
            return true;
        }
    }

    return false;
}

bool Sphere::isOccluded(const Ray& ray) const {
    Vec3 cdir = ray.origin() - _pos;

    // Solve quadratic
    Float B = dot(cdir, ray.dir());
    Float C = cdir.lengthSqr() - _radius * _radius;
    Float detSq = B * B - C;
    if (detSq >= 0.0) {
        Float det = std::sqrt(detSq);

        return (ray.inRange(-B - det) || 
                ray.inRange(-B + det));
    }

    return false;
}