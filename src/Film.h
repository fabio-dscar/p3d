#pragma once

#include <PhotonMath.h>
#include <Spectral.h>
#include <Tonemap.h>
#include <Filter.h>
#include <Framebuffer.h>
#include <Bounds.h>

namespace Photon {

    
    class Film {
    public:
        Film(uint32 width, uint32 height)
            : _res(width, height), _toneOp(FILMIC), _exposure(0.6) {

            _bounds.expand(Point2(width, height));

            _film.resize(_res.x * _res.y * 3);
            //_normal.resize(_width * _height * 3);
            /*_depth.resize(_width * _height);*/
        }

        Film(const Vec2ui& res)
            : _res(res), _toneOp(FILMIC), _exposure(0.6) {

            _bounds.expand(Point2(res.x, res.y));

            _film.resize(_res.x * _res.y * 3);
            //_normal.resize(_width * _height * 3);
            /*_depth.resize(_width * _height);*/
        }

        const Bounds2& bounds() const {
            return _bounds;
        }

        Vec2ui resolution() const {
            return _res;
        }

        uint32 width() const {
            return _res.x;
        }

        uint32 height() const {
            return _res.y;
        }

        Float aspect() const {
            return Float(_res.x) / Float(_res.y);
        }

        uint32 pixelArea() const {
            return _res.x * _res.y;
        }

        void setToneOperator(ToneOperator toneOp) {
            _toneOp = toneOp;
        }

        void setExposure(Float exposure) {
            _exposure = exposure;
        }

        void addColorSample(uint32 x, uint32 y, const Color& color) {
            uint32 idx = x + _res.x * y;

            //Color xyz = RGBToXYZ(color);
            //Color rgb = XYZToRGB(xyz);

            Color hdr = ToneMap(_toneOp, color, _exposure);
            
            _film[3 * idx]     = hdr.r;
            _film[3 * idx + 1] = hdr.g;
            _film[3 * idx + 2] = hdr.b;
        }

        void addSplatSample(uint32 x, uint32 y, const Color color) {

        }

        void addNormalSample(uint32 x, uint32 y, const Vec3& normal) {
            uint32 idx = x + _res.x * y;

            _normal[3 * idx]     = normal.x;
            _normal[3 * idx + 1] = normal.y;
            _normal[3 * idx + 2] = normal.z;
        }

        void addDepthSample(uint32 x, uint32 y, Float dist) {
            //_depth[x + _width * y] = dist;
        }

        const std::vector<Float>& image() const {
            //return _normal;
            return _film;
        }

        /*const Float* image(uint32 row) const {
        uint32 idx = _width * row;

        return &_film[idx * 3];
        }*/

        Color operator()(uint32 x, uint32 y) const {
            uint32 idx = x + _res.x * y;

            return Color(_film[3 * idx], _film[3 * idx + 1], _film[3 * idx + 2]);
        }

    private:
        Vec2ui _res;

        ToneOperator _toneOp;
        Float _exposure;

        std::vector<Float> _film;
        const Filter*      _filter;
        //Framebuffer        _film1;

        std::vector<Float> _normal;
        /*std::vector<float>   _depth;*/

        Bounds2 _bounds;
    };


}