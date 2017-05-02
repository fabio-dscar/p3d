#pragma once

#include <unordered_map>
#include <memory>

namespace Photon {

    class TriMesh;

    // A singleton manager for resources, to be initialized at the start
    class Resources {
    public:
        ~Resources() {
            _meshMap.clear();
        }

        // Do not allow the copy constructor
        Resources(const Resources& manager) = delete;

        static Resources& get() {
            static Resources instance;
            return instance;
        }

        static void initialize() {
            get(); // Initialize instance
        }

        std::shared_ptr<TriMesh> loadObj(const std::string& path, const std::string& name);

    private:
        Resources() { }

        std::unordered_map<std::string, std::shared_ptr<TriMesh>> _meshMap;
    };

}