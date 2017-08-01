#include <Perspective.h>

using namespace Photon;

Perspective::Perspective(const Transform& objToWorld, Vec2ui res, Float fov, Float near, Float far)
    : Camera(objToWorld, res, near, far) {

    // Build perspective matrix
    Float invTan = 1.0 / std::tan(radians(fov / 2.0));
    Float zScale = far / (far - near);
    Float zTrans = -far * near / (far - near);

    Mat4 perspMat(invTan, 0, 0, 0,
                  0, invTan, 0, 0,
                  0, 0, zScale, zTrans,
                  0, 0, 1, 0);

    Transform camToClip = Transform(perspMat);
    
    // Aspect transform
    Float aspect = (Float)res.x / res.y;
    Transform aspectTr = scale(0.5, 0.5 * aspect, 1.0) *
                         translate(Vec3(1.0, 1.0 / aspect, 0.0));

    Transform camToPlane = scale(res.x, res.y, 1.0) * aspectTr * camToClip;
    _planeToCam = inverse(camToPlane);

    // Compute view plane normal in world space
    _n = normalize(_camToWorld(Normal(0, 0, 1)));

    _worldToPlane = camToPlane * inverse(_camToWorld);

    // Calculate film area
    Point3 pMin = _planeToCam(Point3(0, 0, 0));
    Point3 pMax = _planeToCam(Point3(res.x, res.y, 0));
    pMin /= pMin.z;
    pMax /= pMax.z;

    _filmArea = std::abs((pMax.x - pMin.x) * (pMax.y - pMin.y));
}

Color Perspective::evalWe(const Ray& ray) const {
    Float cos = dot(ray.dir(), _n);
    if (cos <= 0)
        return Color::BLACK;

    Float t = 1.0 / cos;
    if (_lens.radius > 0)
        t *= _lens.focalDist;

    Point3 focus  = ray(t);
    Point3 vplane = _worldToPlane(focus);

    // Check if point lies in view plane
    Point2 p = Point2(vplane.x, vplane.y);
    if (!_film.bounds().contains(p))
        return Color::BLACK;

    Float lensArea = _lens.area;
    Float cos4 = (cos * cos) * (cos * cos);

    return Color(1.0 / (lensArea * cos4 * _filmArea));
}

Float Perspective::pdfWe(const Ray& ray) const {
    Float cos = dot(ray.dir(), _n);
    if (cos <= 0)
        return 0;

    Float t = 1.0 / cos;
    if (_lens.radius > 0)
        t *= _lens.focalDist;

    Point3 focus  = ray(t);
    Point3 vplane = _worldToPlane(focus);

    // Check if point lies in view plane
    Point2 p = Point2(vplane.x, vplane.y);
    if (!_film.bounds().contains(p))
        return 0;

    Float cos3 = cos * cos * cos;

    return 1.0 / (_filmArea * cos3);
}

Float Perspective::pdfPosition(PositionSample* ps) const {
    return 1.0 / _lens.area;
}

Color Perspective::sampleDirect(const Point2& rand, DirectSample* sample) const {
    // Sample point on lens
    Point2 lensPt = _lens.radius * sampleConcentricDisk(rand);
    Point3 lensWorldPt = _camToWorld(Point3(lensPt.x, lensPt.y, 0));

    // Compute vector from reference to sampled point
    const RayEvent& ref = *sample->ref;
    const Vec3 refToLens = lensWorldPt - ref.point;
    Float dist = refToLens.length();

    sample->wi     = refToLens / dist;
    sample->dist   = dist;
    sample->pdf    = pdfDirect(*sample);
    sample->normal = _n;

    // Return emitted importance
    return evalWe(Ray(lensWorldPt, -sample->wi));
}

Float Perspective::pdfDirect(const DirectSample& sample) const {
    Float lensPdf = 1.0 / _lens.area;

    // Convert lens pdf to solid angle density
    Float distSqr = sample.dist * sample.dist;
    Float dot = absDot(_n, sample.wi);

    return lensPdf * distSqr / dot;
}