#pragma once

#include <PhotonMath.h>
#include <Spectral.h>
#include <Tonemap.h>
#include <Filter.h>
#include <Framebuffer.h>

namespace Photon {

    
    class Film {
    public:
        Film(uint32 width, uint32 height)
            : _width(width), _height(height) {

            _film.resize(_width * _height * 3);
            _normal.resize(_width * _height * 3);
            /*_depth.resize(_width * _height);*/
        }

        Film(const Vec2ui& res)
            : _width(res.x), _height(res.y) {

            _film.resize(_width * _height * 3);
            _normal.resize(_width * _height * 3);
            /*_depth.resize(_width * _height);*/
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

        void addColorSample(uint32 x, uint32 y, const Color& color) {
            uint32 idx = x + _width * y;

            Color xyz = RGBToXYZ(color);
            Color rgb = XYZToRGB(xyz);

            Color hdr = filmicHDR(rgb);
            
            _film[3 * idx]     = hdr.r;
            _film[3 * idx + 1] = hdr.g;
            _film[3 * idx + 2] = hdr.b;
        }

        void addNormalSample(uint32 x, uint32 y, const Vec3& normal) {
            uint32 idx = x + _width * y;

            _normal[3 * idx]     = normal.x;
            _normal[3 * idx + 1] = normal.y;
            _normal[3 * idx + 2] = normal.z;
        }

        void addDepthSample(uint32 x, uint32 y, Float dist) {
            //_depth[x + _width * y] = dist;
        }

        const std::vector<Float>& image() const {
            return _normal;
            //return _film;
        }

        /*const Float* image(uint32 row) const {
        uint32 idx = _width * row;

        return &_film[idx * 3];
        }*/

        Color operator()(uint32 x, uint32 y) const {
            uint32 idx = x + _width * y;

            return Color(_film[3 * idx], _film[3 * idx + 1], _film[3 * idx + 2]);
        }

    private:
        uint32 _width;
        uint32 _height;

        std::vector<Float> _film;
        const Filter*      _filter;
        //Framebuffer        _film1;

        std::vector<Float> _normal;
        /*std::vector<float>   _depth;*/
    };


}