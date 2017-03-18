#include <Triangle.h>

using namespace Photon;

Triangle::Triangle(const Vec3& v1, const Vec3& v2, const Vec3& v3)
    : _vertices{ v1, v2, v3 } {

    Vec3 p1p2 = v2 - v1;
    Vec3 p1p3 = v3 - v1;

    _normal = glm::normalize(glm::cross(p1p2, p1p3));
}

void Triangle::setVertices(const Vec3& v1, const Vec3& v2, const Vec3& v3) {
    _vertices[0] = v1;
    _vertices[1] = v2;
    _vertices[2] = v3;

    Vec3 p1p2 = v2 - v1;
    Vec3 p1p3 = v3 - v1;

    _normal = glm::normalize(glm::cross(p1p2, p1p3));
}

const std::array<Vec3, 3>& Triangle::vertices() const {
    return _vertices;
}

const Vec3& Triangle::normal() const {
    return _normal;
}

bool Triangle::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    // Badouel's ray-tri intersection algoritm

    // Get distance of triangle's plane to origin
    float dist = glm::dot(_vertices[0], _normal);

    // First check intersection with triangle's plane
    float NdotD = glm::dot(_normal, ray.dir());
    if (std::abs(NdotD) > F_EPSILON) {
        float NdotO = glm::dot(_normal, ray.origin());

        // Compute intersection with triangle's plane
        float t = (dist - NdotO) / NdotD;
        if (!ray.inRange(t))
            return false;

        // Get hitpoint from ray's equation
        Vec3 hitPoint = ray(t);

        // Index choice phase using the normal
        int idx1 = 0;
        int idx2 = 1;

        if (std::abs(_normal[2]) < std::abs(_normal[idx1])) {
            idx1 = 2;
            if (std::abs(_normal[0]) < std::abs(_normal[idx2]))
                idx2 = 0;
        } else {
            if (std::abs(_normal[idx2]) > std::abs(_normal[2]))
                idx2 = 2;
        }

        // Check if point on the plane is inside the triangle
        float u0 = hitPoint[idx1] - _vertices[0][idx1];
        float v0 = hitPoint[idx2] - _vertices[0][idx2];

        float u1 = _vertices[1][idx1] - _vertices[0][idx1];
        float u2 = _vertices[2][idx1] - _vertices[0][idx1];
        float v1 = _vertices[1][idx2] - _vertices[0][idx2];
        float v2 = _vertices[2][idx2] - _vertices[0][idx2];

        float alpha = -1;
        float beta = -1;

        if (std::abs(u1) < F_EPSILON) {
            beta = u0 / u2;
            if (beta >= 0 && beta <= 1)
                alpha = (v0 - beta * v2) / v1;
        } else {
            beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
            if (beta >= 0 && beta <= 1)
                alpha = (u0 - beta * u2) / u1;
        }

        if (beta >= 0 && alpha >= 0 && (alpha + beta) <= 1) {
            ray.setMaxT(t);
            evt->setEvent(ray, this, _normal);
            return true;
        }
    }

    return false;
}

bool Triangle::isOccluded(const Ray& ray) const {
    // Badouel's ray-tri intersection algoritm

    // Get distance of triangle's plane to origin
    float dist = glm::dot(_vertices[0], _normal);

    // First check intersection with triangle's plane
    float NdotD = glm::dot(_normal, ray.dir());
    if (std::abs(NdotD) > F_EPSILON) {
        float NdotO = glm::dot(_normal, ray.origin());

        // Compute intersection with triangle's plane
        float t = (dist - NdotO) / NdotD;
        if (!ray.inRange(t))
            return false;

        Vec3 pt = ray(t);

        // Index choice phase using the normal
        int idx1 = 0;
        int idx2 = 1;

        if (std::abs(_normal[2]) < std::abs(_normal[idx1])) {
            idx1 = 2;
            if (std::abs(_normal[0]) < std::abs(_normal[idx2]))
                idx2 = 0;
        } else {
            if (std::abs(_normal[idx2]) > std::abs(_normal[2]))
                idx2 = 2;
        }

        // Check if point on the plane is inside the triangle
        float u0 = pt[idx1] - _vertices[0][idx1];
        float v0 = pt[idx2] - _vertices[0][idx2];

        float u1 = _vertices[1][idx1] - _vertices[0][idx1];
        float u2 = _vertices[2][idx1] - _vertices[0][idx1];
        float v1 = _vertices[1][idx2] - _vertices[0][idx2];
        float v2 = _vertices[2][idx2] - _vertices[0][idx2];

        float alpha = -1;
        float beta = -1;

        if (std::abs(u1) < F_EPSILON) {
            beta = u0 / u2;
            if (beta >= 0 && beta <= 1)
                alpha = (v0 - beta * v2) / v1;
        } else {
            beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
            if (beta >= 0 && beta <= 1)
                alpha = (u0 - beta * u2) / u1;
        }

        return (beta >= 0 && alpha >= 0 && (alpha + beta) <= 1);
    }

    return false;
}