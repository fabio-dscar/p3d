#pragma once

#include <array>

#include <Vector.h>
#include <Geometry.h>

namespace Photon {

    class Triangle : public Geometry {
    public:
        Triangle(const Vec3& v1, const Vec3& v2, const Vec3& v3) 
            : _vertices{ v1, v2, v3 } { 
            
            Vec3 p1p2 = v2 - v1;
            Vec3 p1p3 = v3 - v1;

            _normal = glm::normalize(glm::cross(p1p2, p1p3));
        }

        Triangle(const Vec3& v1, const Vec3& v2, const Vec3& v3, const Vec3& normal) 
            : _vertices{ v1, v2, v3 }, _normal(normal) { }

        void setVertices(const Vec3& v1, const Vec3& v2, const Vec3& v3) {
            _vertices[0] = v1;
            _vertices[1] = v2;
            _vertices[2] = v3;

            Vec3 p1p2 = v2 - v1;
            Vec3 p1p3 = v3 - v1;

            _normal = glm::normalize(glm::cross(p1p2, p1p3));
        }

        const std::array<Vec3, 3>& getVertices() const {
            return _vertices;
        }

        const Vec3& getNormal() const {
            return _normal;
        }

        bool intersectRay(const Ray& ray, HitInfo& info) const {
            float dist = glm::dot(_vertices[0], _normal);

            // Check intersection with triangle's plane
            float NdotD = glm::dot(_normal, ray.getDirection());
            if (std::abs(NdotD) > 1e-6) {
                float NdotO = glm::dot(_normal, ray.getOrigin());

                info._t = (dist - NdotO) / NdotD;
                info._point = ray.getPoint(info._t);
                info._normal = _normal;
                info._obj = (Geometry*)this;

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

                float u0 = info._point[idx1] - _vertices[0][idx1];
                float v0 = info._point[idx2] - _vertices[0][idx2];

                float u1 = _vertices[1][idx1] - _vertices[0][idx1];
                float u2 = _vertices[2][idx1] - _vertices[0][idx1];
                float v1 = _vertices[1][idx2] - _vertices[0][idx2];
                float v2 = _vertices[2][idx2] - _vertices[0][idx2];

                float alpha = -1; 
                float beta = -1;

                if (std::abs(u1) < 1e-6) {
                    beta = u0 / u2;
                    if (beta >= 0 && beta <= 1) 
                        alpha = (v0 - beta * v2) / v1; 
                } else {
                    beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
                    if (beta >= 0 && beta <= 1)
                        alpha = (u0 - beta * u2) / u1;
                }

                info._hit = (beta >= 0 && alpha >= 0 && (alpha + beta) <= 1);

                return info._hit;
            }

            return false;
        }

    private:
        std::array<Vec3, 3> _vertices;
        Vec3 _normal;
    };

}