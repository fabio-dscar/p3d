#include <Sphere.h>

using namespace Photon;

const Vec3& Sphere::pos() const {
    return _pos;
}

float Sphere::radius() const {
    return _radius;
}

bool Sphere::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    Vec3 p = ray.origin() - _pos;

    // Solve quadratic
    float B = glm::dot(p, ray.dir());
    float C = glm::length2(p) - _radius * _radius;
    float detSq = B*B - C;
    if (detSq >= 0.0f) {
        float det = std::sqrt(detSq);
        float t = -B - det;

        if (ray.inRange(t)) {
            ray.setMaxT(t);
            evt->setEvent(ray, this, glm::normalize(ray(t) - _pos));
            return true;
        }

        t = -B + det;
        if (ray.inRange(t)) {
            ray.setMaxT(t);
            evt->setEvent(ray, this, -glm::normalize(ray(t) - _pos));
            evt->setBackface(true);  // Hit from inside sphere
            return true;
        }
    }

    return false;
}

bool Sphere::isOccluded(const Ray& ray) const {
    Vec3 p = ray.origin() - _pos;

    // Solve quadratic
    float B = glm::dot(p, ray.dir());
    float C = glm::length2(p) - _radius * _radius;
    float detSq = B*B - C;
    if (detSq >= 0.0f) {
        float det = std::sqrt(detSq);
        float t = -B - det;

        if (ray.inRange(t))
            return true;

        t = -B + det;
        if (ray.inRange(t))
            return true;
    }

    return false;
}