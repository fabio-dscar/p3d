#pragma once

#include <Ray.h>

namespace Photon {

    template<typename T>
    class Texture {
    public:
        Texture() { }
        virtual ~Texture() { }

        virtual bool isConst() const {
            return false;
        }

        virtual T fetch(const Point2& uv) const = 0;
        virtual T fetch(const SurfaceEvent& evt) const {
            return fetch(evt.uv);
        }

        T operator()(Float u, Float v) const {
            fetch({ u, v });
        }
    };

}