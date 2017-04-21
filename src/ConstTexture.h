#pragma once

#include <Texture.h>

namespace Photon {

    template<typename T>
    class ConstTexture : public Texture<T> {
    public:
        ConstTexture(T val) : _val(val) { }

        bool isConst() const {
            return true;
        }

        T fetch(const Point2& uv) const {
            return _val;
        }

    private:
        T _val;
    };

}