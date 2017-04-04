#pragma once

#include <Vector.h>

namespace Photon {

    class Frame {
    public:
        Frame(const Normal& norm);

        Vec3 toWorld(const Vec3& vec);
        Vec3 toLocal(const Vec3& vec);

        bool consistent();

    private:
        Vec3 _u, _v, _n;
    };

};