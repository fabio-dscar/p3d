#include <Quad.h>

using namespace Photon;

Quad::Quad(const Transform& objToWorld) : Shape(objToWorld) {
    Vec3 dpdu = _objToWorld(Vec3(1, 0, 0));
    Vec3 dpdv = _objToWorld(Vec3(0, 1, 0));

    _area = dpdu.length() * dpdv.length();
}

bool Quad::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    const Ray objRay = _worldToObj(ray);

    // Check if ray is parallel to the quad
    Float dz = objRay.dir().z;
    if (std::abs(dz) < F_EPSILON)
        return false;

    // Compute intersection point
    Float t = -objRay.origin().z / dz;
    if (!objRay.inRange(t))
        return false;

    // Check if inside quad bounds
    const Point3 hit = objRay(t);
    const Point2 uv = Point2(hit.x + 0.5, hit.y + 0.5);

    if (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1)
        return false;

    ray.setMaxT(t);
    evt->obj = this;
    evt->uv = uv;

    if (-dz < 0)
        evt->backface = true;

    return true;
}

bool Quad::isOccluded(const Ray& ray) const {
    const Ray objRay = _worldToObj(ray);

    // Check if ray is parallel to the quad
    Float dz = objRay.dir().z;
    if (std::abs(dz) < F_EPSILON)
        return false;

    // Compute intersection point
    Float t = -objRay.origin().z / dz;
    if (!objRay.inRange(t))
        return false;

    // Check if inside quad bounds
    const Point3 hit = objRay(t);

    return (hit.x >= -0.5 && hit.x <= 0.5 && hit.y >= -0.5 && hit.y <= 0.5);
}

void Quad::computeSurfaceEvent(const Ray& ray, SurfaceEvent& evt) const {
    const Normal n = _objToWorld(Normal(0, 0, 1));

    evt.setEvent(ray, this, n);
}

Bounds3 Quad::bbox() const {
    Bounds3 box(0);

    box.expand(_objToWorld(Point3(-0.5, -0.5, 0)));
    box.expand(_objToWorld(Point3(-0.5, 0.5, 0)));
    box.expand(_objToWorld(Point3(0.5, -0.5, 0)));
    box.expand(_objToWorld(Point3(0.5, 0.5, 0)));
    box.expand(F_EPSILON);

    return box;
}

Float Quad::area() const {
    return _area;
}

void Quad::samplePosition(const Point2& rand, PositionSample* sample) const {
    const Point3 quad = Point3(rand.x - 0.5, rand.y - 0.5, 0);

    sample->pos = _objToWorld(quad);
    sample->pdf = 1.0 / area();

    const Normal n = _objToWorld(Normal(0, 0, 1));
    sample->frame = Frame(n);
}

Float Quad::pdfPosition(const PositionSample& sample) const {
    return 1.0 / area();
}

void Quad::sampleDirect(const Point2& rand, DirectSample* sample) const {
    const RayEvent& ref = *sample->ref;

    // Sample a position on the quad
    Point3 pos = _objToWorld(Point3(rand.x - 0.5, rand.y - 0.5, 0));
    Float posPdf = 1.0 / area();

    // Compute direction from reference
    const Vec3 refToPt = pos - ref.point;
    Float distSqr = refToPt.lengthSqr();

    sample->dist = sqrt(distSqr);
    sample->wi = refToPt / sample->dist;
    sample->normal = _objToWorld(Normal(0, 0, 1));

    // Convert to solid angle density
    Float dot = absDot(sample->normal, -sample->wi);
    sample->pdf = posPdf * distSqr / dot;
}

Float Quad::pdfDirect(const DirectSample& sample) const {
    Float pdfPos = 1.0 / area();

    // Convert area pdf to solid angle density
    Float distSqr = sample.dist * sample.dist;
    Float dot = absDot(sample.normal, -sample.wi);

    return pdfPos * distSqr / dot;
}