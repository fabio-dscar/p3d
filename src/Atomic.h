#pragma once

#include <atomic>

#include <PhotonMath.h>
#include <Spectral.h>

namespace Photon {

    class AtomicFloat {
    public:
        AtomicFloat() : _float(0) { }
        AtomicFloat(Float val) : _float(val) { }

        void add(Float val) {
            auto curr = _float.load();
            while (!_float.compare_exchange_weak(curr, curr + val));
        }

        void sub(Float val) {
            auto curr = _float.load();
            while (!_float.compare_exchange_weak(curr, curr - val));
        }

        void mul(Float val) {
            auto curr = _float.load();
            while (!_float.compare_exchange_weak(curr, curr * val));
        }

        Float val() const {
            return _float.load();
        }

    private:
        std::atomic<Float> _float;
    };

    class AtomicColor {
    public:
        AtomicFloat r, g, b;

        void add(const Color& col) {
            r.add(col.r);
            g.add(col.g);
            b.add(col.b);
        }

        void sub(const Color& col) {
            r.sub(col.r);
            g.sub(col.g);
            b.sub(col.b);
        }

        void mul(const Color& col) {
            r.mul(col.r);
            g.mul(col.g);
            b.mul(col.b);
        }

    };

}