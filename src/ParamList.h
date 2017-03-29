#pragma once

#include <unordered_map>

#include <MathDefs.h>

namespace Photon {

    class ParamList {
    public:
        ParamList() { }

        std::string getString(const std::string& name) {
            return _strings[name];
        }



    private:
        std::unordered_map<std::string, Float> _floats;
        std::unordered_map<std::string, std::string> _strings;
    };

}