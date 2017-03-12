#pragma once

#include <Vector.h>

namespace Photon {

    class Material {
    public:
        Material() : _color(1.0f), _diff(1.0f), _spec(1.0f), _shininess(1.0f) { }
        Material(const Color3& color, float diff, float spec, float shininess) : 
            _color(color), _diff(diff), _spec(spec), _shininess(shininess) { }

    private:
        Color3 _color;
        float _diff;
        float _spec;
        float _shininess;
    };

}