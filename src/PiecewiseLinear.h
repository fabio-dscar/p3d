#pragma once

#include <vector>

#include <PhotonMath.h>

namespace Photon {

    struct FPoint {
        Float obj;
        Float img;
    };

    class PiecewiseLinear {
    public:
        PiecewiseLinear(Float* objs, Float* imgs, uint32 num) {
            /*_objs.resize(num);
            _imgs.resize(num);

            for (uint32 i = 0; i < num; ++i) {
                _objs[i] = objs[i];
                _imgs[i] = imgs[i];
            }*/

            _func.resize(num);
            for (uint32 i = 0; i < num; ++i)
                _func[i] = { objs[i], imgs[i] };

            // Sort function

        }

        const std::vector<Float> objs() const {
            return _objs;
        }

        const std::vector<Float> imgs() const {
            return _imgs;
        }

        Float evalInterpolate(Float x) const {

        }

        Float integrate(Float from, Float to) const {
            if (from < _objs[0] && to < _objs[0])
                return 0;


        }

    private:
        std::vector<FPoint> _func;
        //std::vector<Float> _objs;
        //std::vector<Float> _imgs;
    };

}