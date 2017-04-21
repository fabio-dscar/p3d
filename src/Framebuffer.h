#pragma once

namespace Photon {

    struct PixelRecord {
        Vec2ui pixel;
        Color  color;
        Normal normal;
        Float  dist;
        Float  visibility;
    };

    template<typename T>
    class BufferRecord {
    public:
        T val;
        uint32 numSamples;

        BufferRecord() : _numSamples(0) { }
    };

    template<typename T>
    class Buffer{
    public:
        Buffer() : _buffer(nullptr) { }
        Buffer(const Vec2ui& sizes) {
            _buffer = std::make_unique<T>(sizes.x * sizes.y);
        }

        void addRecord(const Vec2ui& pixel, const T& rec) {
            _buffer[pixel].val = rec;
        }

        const T& operator[](uint32 idx) const {
            return _buffer[idx].val;
        }

        T& operator[](uint32 idx) {
            return _buffer[idx].val;
        }

        bool available() {
            return _buffer != nullptr;
        }

    private:
        //std::unique_ptr<T[]> _buffer;
        std::unique_ptr<BufferRecord<T>[]> _buffer;
    };

    class Framebuffer {
    public:
        Framebuffer() : _color(), _normals(), _visibility() { 
            
        }

        void addPixelRecord(const PixelRecord& pRec) {
            uint32 idx = pRec.pixel.x + _width * pRec.pixel.y;

            _preview[3 * idx]     = pRec.color.r;
            _preview[3 * idx + 1] = pRec.color.g;
            _preview[3 * idx + 2] = pRec.color.b;

            if (_color.available())
                _color[idx] = pRec.color;

            if (_normals.available())
                _normals[idx] = pRec.normal;

            if (_visibility.available())
                _visibility[idx] = pRec.visibility;
        }

        PixelRecord recordAt(const Vec2ui& pixel) const {
            PixelRecord ret;

            uint32 idx = pixel.x + _width * pixel.y;

            ret.pixel      = pixel;
            ret.color      = _color[idx];
            ret.normal     = _normals[idx];
            ret.visibility = _visibility[idx];

            return ret;
        }

        const Color& color(const Vec2ui& pixel) const {
            uint32 idx = pixel.x + _width * pixel.y;
            return _color[idx];
        }

    private:
        uint32 _width;
        uint32 _height;

        std::unique_ptr<Float[]> _preview;
        
        Buffer<Color>  _color;
        Buffer<Normal> _normals;
        Buffer<Float>  _depth;
        Buffer<Float>  _visibility;

        /*std::unique_ptr<Float[]> _color;
        std::unique_ptr<Float[]> _normals;
        std::unique_ptr<Float[]> _visibility;*/
    };

}