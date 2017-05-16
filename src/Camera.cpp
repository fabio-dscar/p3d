#include <Camera.h>
#include <Matrix.h>

using namespace Photon;

Camera::Camera(const Transform& camToWorld, Vec2ui res, Float near, Float far)
    : _film(res), _camToWorld(camToWorld),
      _near(near), _far(far), _lens({ 0, 1, 1 }) {} // 2.3 and 3.7

void Camera::setLensParams(Float radius, Float focalDist) {
    _lens.radius = radius;
    _lens.focalDist = focalDist;
    _lens.area = 1;
    if (radius > 0)
        _lens.area = PI * radius * radius;
}

Ray Camera::primaryRay(const Point2& pixel, const Point2& lens) const {
    Point3 origin = Point3(0, 0, 0);
    Point3 pPlane = Point3(pixel.x, pixel.y, 0);

    // Transform point in plane to camera space
    Point3 pCam = _planeToCam(pPlane);

    // Build direction from origin to pixel
    Vec3 rDir = normalize(pCam.posVec());

    // Build ray and transform to world space
    Ray ray;

    if (_lens.radius > 0) {
        Point2 ptDisk = _lens.radius * sampleConcentricDisk(lens);
        Point3 ptLens = Point3(ptDisk.x, ptDisk.y, 0);
        Point3 pFocus = origin + _lens.focalDist / rDir.z * rDir;

        ray = Ray(ptLens, normalize(pFocus - ptLens), _near);
    } else {
        ray = Ray(origin, rDir, _near);
    }

    ray = _camToWorld(ray);
    ray.setPrimary(true);

    return ray;
}

Ray Camera::primaryRay(const Point2ui& pixel, Sampler& sampler) const {
    // Sample point in pixel square
    Point2 rand   = sampler.next2D();
    Point2 uPixel = Point2(pixel.x + rand.x, pixel.y + rand.y);

    Point3 origin = Point3(0, 0, 0);
    Point3 pPlane = Point3(uPixel.x, uPixel.y, 0);

    // Transform point in plane to camera space
    Point3 pCam = _planeToCam(pPlane);

    // Build direction from origin to pixel
    Vec3 rDir = normalize(pCam.posVec());

    // Build ray and transform to world space
    Ray ray;

    if (_lens.radius > 0) {
        Point2 ptDisk = sampleConcentricDisk(sampler.next2D());
        Point3 ptLens = _lens.radius * Point3(ptDisk.x, ptDisk.y, 0);
        Point3 pFocus = origin + _lens.focalDist * rDir / rDir.z;

        ray = Ray(ptLens, normalize(pFocus - ptLens), _near);
    } else {
        ray = Ray(origin, rDir, _near);
    }

    ray.setTime(sampler.next1D());

    ray = _camToWorld(ray);
    ray.setPrimary(true);

    return ray;
}

Frame Camera::camFrame() const {
    const Mat4& mat = _camToWorld.invMatrix();

    Vec3 u = Vec3(mat(0, 0), mat(1, 0), mat(2, 0));
    Vec3 v = Vec3(mat(0, 1), mat(1, 1), mat(2, 1));
    Vec3 n = Vec3(mat(0, 2), mat(1, 2), mat(2, 2));

    return Frame(u, v, n);
}

Transform Camera::lookAt(const Point3& pos, const Point3& at, const Vec3& up) {
    Mat4 lookAtMat;

    Vec3 n = normalize(pos - at);
    Vec3 u = normalize(cross(up, n));
    Vec3 v = cross(n, u);

    // Rotation
    lookAtMat(0, 0) = u.x;
    lookAtMat(1, 0) = u.y;
    lookAtMat(2, 0) = u.z;
    lookAtMat(3, 0) = 0;

    lookAtMat(0, 1) = v.x;
    lookAtMat(1, 1) = v.y;
    lookAtMat(2, 1) = v.z;
    lookAtMat(3, 1) = 0;

    lookAtMat(0, 2) = -n.x;
    lookAtMat(1, 2) = -n.y;
    lookAtMat(2, 2) = -n.z;
    lookAtMat(3, 2) = 0;

    // Translation
    lookAtMat(0, 3) = pos.x;
    lookAtMat(1, 3) = pos.y;
    lookAtMat(2, 3) = pos.z;
    lookAtMat(3, 3) = 1;

    return Transform(lookAtMat);
}