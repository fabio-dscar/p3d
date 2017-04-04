#include <Frame.h>

using namespace Photon;

Frame::Frame(const Normal& norm) : _n(normalize(Vec3(norm))) {
    Math::basisFromVector(_n, &_u, &_v);
    _u.normalize();
    _v.normalize();

    if (!consistent())
        std::cout << "WARNING";
}

Vec3 Frame::toWorld(const Vec3& vec) {
    return _u * vec.x + _v * vec.y + _n * vec.z;
}

Vec3 Frame::toLocal(const Vec3& vec) {
    Float dotU = dot(_u, vec);
    Float dotV = dot(_v, vec);
    Float dotN = dot(_n, vec);
    return Vec3(dotU, dotV, dotN);

    //return _u * vec.x + _v * vec.y + _n * vec.z;
}

bool Frame::consistent() {
    Float dot12 = dot(_u, _v);
    Float dot13 = dot(_u, _n);
    Float dot23 = dot(_v, _n);
    return (abs(dot12) < F_EPSILON &&
            abs(dot13) < F_EPSILON &&
            abs(dot23) < F_EPSILON);
}