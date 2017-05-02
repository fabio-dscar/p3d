#include <TriMesh.h>

using namespace Photon;

MeshTriangle::MeshTriangle(const TriMesh& mesh, const uint32* indices)
    : Shape(), _mesh(&mesh), _idx(indices) { }

Bounds3 MeshTriangle::bbox() const {
    // Retrieve vertices from mesh
    const Point3 V0 = _mesh->vertex(_idx[0]);
    const Point3 V1 = _mesh->vertex(_idx[1]);
    const Point3 V2 = _mesh->vertex(_idx[2]);

    Point3 max = Math::max(V0, Math::max(V1, V2));
    Point3 min = Math::min(V0, Math::min(V1, V2));

    Bounds3 box = Bounds3(min, max);
    box.expand(F_EPSILON);

    return box;
}

bool MeshTriangle::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    // Moller-Trumbore ray-triangle intersection

    const Vec3   D = ray.dir();
    const Point3 O = ray.origin();

    // Retrieve vertices from mesh
    const Point3 V0 = _mesh->vertex(_idx[0]);
    const Point3 V1 = _mesh->vertex(_idx[1]);
    const Point3 V2 = _mesh->vertex(_idx[2]);

    // Compute triangle edges
    Vec3 E1 = V1 - V0;
    Vec3 E2 = V2 - V0;

    Vec3 P = cross(D, E2);

    Float det = dot(E1, P);  
    if (det > -F_EPSILON && det < F_EPSILON)
        return false;

    Vec3 T = O - V0;
    Vec3 Q = cross(T, E1);

    Float invDet = 1.0 / det;

    Float t = dot(E2, Q) * invDet;
    if (!ray.inRange(t))
        return false;

    Float u = dot(T, P) * invDet;
    if (u < 0.0 || u > 1.0)
        return false;

    Float v = dot(D, Q) * invDet;
    if (v < 0.0 || (u + v) > 1.0)
        return false;

    ray.setMaxT(t);
    evt->obj    = this;
    evt->point  = (1.0f - u - v) * V0 + u * V1 + v * V2; //ray.hitPoint();
    evt->normal = normalize(Normal(cross(E1, E2)));

    // Save the (u, v) barycentric coordinates for later
    evt->uv = Point2(u, v);

    return true;
}

bool MeshTriangle::isOccluded(const Ray& ray) const {
    const Vec3   D = ray.dir();
    const Point3 O = ray.origin();

    // Retrieve vertices from mesh
    const Point3 V0 = _mesh->vertex(_idx[0]);
    const Point3 V1 = _mesh->vertex(_idx[1]);
    const Point3 V2 = _mesh->vertex(_idx[2]);

    // Compute triangle edges
    Vec3 E1 = V1 - V0;
    Vec3 E2 = V2 - V0;

    Vec3 P = cross(D, E2);

    Float det = dot(E1, P);
    if (abs(det) < F_EPSILON)
        return false;

    Vec3 T = O - V0;
    Vec3 Q = cross(T, E1);

    Float invDet = 1.0 / det;

    Float t = dot(E2, Q) * invDet;
    if (!ray.inRange(t))
        return false;

    Float u = dot(T, P) * invDet;
    if (u < 0 || u > 1)
        return false;

    Float v = dot(D, Q) * invDet;
    if (v < 0 || (u + v) > 1)
        return false;

    return true;
}

void MeshTriangle::computeSurfaceEvent(const Ray& ray, SurfaceEvent& evt) const {
    // Use the previously stored barycentric coordinates
    Float u = evt.uv.x;
    Float v = evt.uv.y;

    Frame sFrame;

    // Compute normal
    Normal n = evt.normal;
    
    if (_mesh->hasNormals()) {
        const Normal N0 = _mesh->normal(_idx[0]);
        const Normal N1 = _mesh->normal(_idx[1]);
        const Normal N2 = _mesh->normal(_idx[2]);

        // Interpolate normal
        n = normalize((1.0f - u - v) * N0 + u * N1 + v * N2);

        // Make it face the geometry normal
        if (dot(n, evt.normal) < 0)
            n = -n;

        // Set a shading frame with the interpolated normal
        sFrame = Frame(n);

        Vec3 tan, bitan;
        if (_mesh->hasTangents()) {
            // Compute tangent
            const Vec3 T0 = _mesh->tangent(_idx[0]);
            const Vec3 T1 = _mesh->tangent(_idx[1]);
            const Vec3 T2 = _mesh->tangent(_idx[2]);

            // Interpolate tangent
            tan = normalize((1 - u - v) * T0 + u * T1 + v * T2);
            if (tan.lengthSqr() > 0) {
                tan.normalize();

                bitan = cross(tan, Vec3(n));
                if (bitan.lengthSqr() > 0) {
                    bitan.normalize();
                    tan = cross(bitan, Vec3(n));

                    sFrame = Frame(tan, bitan, n);
                } 
            } 
        } 
    } else {
        sFrame = Frame(evt.normal);
    }

    // Compute UVs
    Point2 UV0, UV1, UV2;
    if (_mesh->hasUVs()) {
        UV0 = _mesh->uv(_idx[0]);
        UV1 = _mesh->uv(_idx[1]);
        UV2 = _mesh->uv(_idx[2]);
    } else {
        UV0 = Point2(0, 0);
        UV1 = Point2(1, 0);
        UV2 = Point2(0.5, 1);
    }

    // Interpolate uv
    Point2 uv = (1 - u - v) * UV0 + u * UV1 + v * UV2;

    evt.uv = uv;
    evt.wo = sFrame.toLocal(-ray.dir());
    evt.gFrame = Frame(evt.normal);
    evt.sFrame = sFrame;
}