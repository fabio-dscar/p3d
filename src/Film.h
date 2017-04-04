#pragma once

#include <PhotonMath.h>

namespace Photon {

    class Film {
    public:
        Film(uint32 width, uint32 height)
            : _width(width), _height(height) {

            _film.resize(_width * _height * 3);
            /*_normal.resize(_width * _height * 3);
            _depth.resize(_width * _height);*/
        }

        Film(const Vec2ui& res)
            : _width(res.x), _height(res.y) {

            _film.resize(_width * _height * 3);
            /*_normal.resize(_width * _height * 3);
            _depth.resize(_width * _height);*/
        }

        uint32 width() const {
            return _width;
        }

        uint32 height() const {
            return _height;
        }

        Float aspect() const {
            return Float(_width) / Float(_height);
        }

        uint32 pixelArea() const {
            return _width * _height;
        }

        void addColorSample(uint32 x, uint32 y, const Color3& color) {
            uint32 idx = x + _width * y;

            Color3 hdr = filmicHDR(color);

            _film[3 * idx] = hdr.x;
            _film[3 * idx + 1] = hdr.y;
            _film[3 * idx + 2] = hdr.z;
        }

        void addNormalSample(uint32 x, uint32 y, const Vec3& normal) {
            uint32 idx = x + _width * y;

            /*_normal[3 * idx] = normal.x;
            _normal[3 * idx + 1] = normal.y;
            _normal[3 * idx + 2] = normal.z;*/
        }

        void addDepthSample(uint32 x, uint32 y, Float dist) {
            //_depth[x + _width * y] = dist;
        }

        const std::vector<Float>& image() const {
            return _film;
        }

        /*const Float* image(uint32 row) const {
        uint32 idx = _width * row;

        return &_film[idx * 3];
        }*/

        Color3 operator()(uint32 x, uint32 y) const {
            uint32 idx = x + _width * y;

            return Color3(_film[3 * idx], _film[3 * idx + 1], _film[3 * idx + 2]);
        }

    private:
        Color3 filmicHDR(Color3 in) {
            const Float EXPOSURE = 0.6; // 0.6

            in = EXPOSURE * in;

            Float a = 2.51f;
            Float b = 0.03f;
            Float c = 2.43f;
            Float d = 0.59f;
            Float e = 0.14f;
            Vec3 nom = (in * (a * in + b));
            Vec3 denom = (in * (c * in + d) + e);

            Vec3 res = clamp(Vec3(nom.x / denom.x, nom.y / denom.y, nom.z / denom.z), Vec3(0), Vec3(1));

            //std::cout << in.x << ", " << in.y << ", " << in.z << std::endl;
            //std::cout << res.x << ", " << res.y << ", " << res.z << std::endl;

            //return pow(res, 0.9);

            return pow(res, 1.0 / 1.6); // 1.0 / 1.6
        }

        uint32 _width, _height;
        std::vector<Float> _film;
        /*std::vector<float> _normal;
        std::vector<float> _depth;*/
    };


}