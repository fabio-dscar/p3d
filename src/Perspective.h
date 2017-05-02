#pragma once

#include <Camera.h>

namespace Photon {

    class Perspective : public Camera {
    public:
        Perspective(const Transform& objToWorld, Vec2ui res, Float fov, Float near, Float far)
            : Camera(objToWorld, res, near, far) { 
        
            // Build perspective matrix
            Float invTan = 1.0 / std::tan(radians(fov) / 2.0);
            Float zScale = far / (far - near);
            Float zTrans = -far * near / (far - near);

            Mat4 perspMat(invTan, 0, 0, 0,
                          0, invTan, 0, 0,
                          0, 0, zScale, zTrans,
                          0, 0, 1, 0);

            _camToClip = Transform(perspMat);
            
            // Aspect transform
            Float aspect = res.x / res.y;
            Transform aspectTr = scale(0.5 * aspect, 0.5, 1.0) *
                                 translate(Vec3(1.0 / aspect, 1.0, 0.0));

            _camToPlane = scale(res.x, res.y, 1.0) * aspectTr * _camToClip;
            _planeToCam = inverse(_camToPlane);
        }




    private:
        
    };
}