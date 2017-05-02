#include <Triangle.h>

#include <Bounds.h>

using namespace Photon;
using namespace Photon::Math;

Triangle::Triangle(const Point3& v1, const Point3& v2, const Point3& v3)
    : _vertices{ v1, v2, v3 } {

    Vec3 p1p2 = v2 - v1;
    Vec3 p1p3 = v3 - v1;
    
    _normal = Normal(normalize(cross(p1p2, p1p3)));
}

void Triangle::setVertices(const Point3& v1, const Point3& v2, const Point3& v3) {
    _vertices[0] = v1;
    _vertices[1] = v2;
    _vertices[2] = v3;

    Vec3 p1p2 = v2 - v1;
    Vec3 p1p3 = v3 - v1;

    _normal = Normal(normalize(cross(p1p2, p1p3)));
}

const std::array<Point3, 3>& Triangle::vertices() const {
    return _vertices;
}

const Normal& Triangle::normal() const {
    return _normal;
}

Bounds3 Triangle::bbox() const {
    Point3 max = Math::max(_vertices[0], Math::max(_vertices[1], _vertices[2]));
    Point3 min = Math::min(_vertices[0], Math::min(_vertices[1], _vertices[2]));

    return Bounds3(min + Point3(-F_EPSILON), 
                   max + Point3(F_EPSILON));
}

bool Triangle::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    // Badouel's ray-tri intersection algoritm

    // Get distance of triangle's plane to origin
    Float dist = dot(_vertices[0].posVec(), _normal);

    // First check intersection with triangle's plane
    Float NdotD = dot(_normal, ray.dir());
    if (std::abs(NdotD) <= F_EPSILON)
        return false;

    Float NdotO = dot(_normal, ray.origin().posVec());

    // Compute intersection with triangle's plane
    Float t = (dist - NdotO) / NdotD;
    if (!ray.inRange(t))
        return false;

    // Get hitpoint from ray's equation
    Point3 hitPoint = ray(t);

    // Index choice phase using the normal
    int32 idx1 = 0;
    int32 idx2 = 1;

    Normal absNormal = abs(_normal);
    if (absNormal[2] < absNormal[idx1]) {
        idx1 = 2;
        if (absNormal[0] < absNormal[idx2])
            idx2 = 0;
    } else {
        if (absNormal[idx2] > absNormal[2])
            idx2 = 2;
    }

    // Check if point on the plane is inside the triangle
    Float u0 = hitPoint[idx1] - _vertices[0][idx1];
    Float v0 = hitPoint[idx2] - _vertices[0][idx2];

    Float u1 = _vertices[1][idx1] - _vertices[0][idx1];
    Float u2 = _vertices[2][idx1] - _vertices[0][idx1];
    Float v1 = _vertices[1][idx2] - _vertices[0][idx2];
    Float v2 = _vertices[2][idx2] - _vertices[0][idx2];

    Float alpha = -1;
    Float beta = -1;

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

    return false;
}

bool Triangle::isOccluded(const Ray& ray) const {
    // Badouel's ray-tri intersection algoritm

    // Get distance of triangle's plane to origin
    Float dist = dot(_vertices[0].posVec(), _normal);

    // First check intersection with triangle's plane
    Float NdotD = dot(_normal, ray.dir());
    if (std::abs(NdotD) > F_EPSILON) {
        Float NdotO = dot(_normal, ray.origin().posVec());

        // Compute intersection with triangle's plane
        Float t = (dist - NdotO) / NdotD;
        if (!ray.inRange(t))
            return false;

        Point3 pt = ray(t);

        // Index choice phase using the normal
        int32 idx1 = 0;
        int32 idx2 = 1;

        Normal absNormal = abs(_normal);
        if (absNormal[2] < absNormal[idx1]) {
            idx1 = 2;
            if (absNormal[0] < absNormal[idx2])
                idx2 = 0;
        } else {
            if (absNormal[idx2] > absNormal[2])
                idx2 = 2;
        }

        // Check if point on the plane is inside the triangle
        Float u0 = pt[idx1] - _vertices[0][idx1];
        Float v0 = pt[idx2] - _vertices[0][idx2];

        Float u1 = _vertices[1][idx1] - _vertices[0][idx1];
        Float u2 = _vertices[2][idx1] - _vertices[0][idx1];
        Float v1 = _vertices[1][idx2] - _vertices[0][idx2];
        Float v2 = _vertices[2][idx2] - _vertices[0][idx2];

        Float alpha = -1;
        Float beta = -1;

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