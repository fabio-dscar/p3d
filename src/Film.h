#pragma once

#include <PhotonMath.h>
#include <Spectral.h>
#include <Tonemap.h>
#include <Filter.h>
#include <Bounds.h>
#include <Atomic.h>

namespace Photon {

    enum BufferType {
        COLOR, DEPTH, NORMAL, VISIBILITY, SAMPLES
    };

    struct FeaturesRecord {
    public:
        Normal normal;   // Shading normal at intersection
        Point2 raster;   // Raster position of sample        
        Float  dist;     // Primary ray distance
        uint32 vis;      // Number of unoccluded shadow rays
        uint32 nSamples;

        FeaturesRecord() : dist(0), vis(0), nSamples(0) { }
    };

    struct Pixel {
        Color       color;
        Float       weight;
        AtomicColor splat;
        uint32      nSamples;

#ifdef PHOTON_DOUBLE
        uint8 pad[4]; // Make pixel 64 bytes
#endif

        Pixel() : nSamples(0), weight(0) { }

    }; // 32 Bytes (4 byte FP)

    class Film {
    public:
        Film(const Vec2ui& res);

        void setToneOperator(ToneOperator op);
        void setExposure(Float exp);

        const Bounds2& bounds() const;

        uint32 width() const;
        uint32 height() const;
        Vec2ui resolution() const;

        Float aspect() const;

        uint32 pixelArea() const;

        void addPreviewSample(uint32 x, uint32 y, const Color& color);
        void addColorSample(uint32 x, uint32 y, const Color& color);
        void addSplatSample(const Point2& pt, const Color& splat);
        void addFeatureSample(const FeaturesRecord& record);

        const Float* preview() const;

        std::unique_ptr<Float[]> color() const;
        std::unique_ptr<Float[]> depth() const;
        std::unique_ptr<Float[]> normals() const;
        std::unique_ptr<Float[]> sampleDensity() const;
        std::unique_ptr<Float[]> visibility() const;

        FeaturesRecord& feature(const Point2& p);
        FeaturesRecord& feature(const Point2ui& p);
        const FeaturesRecord& feature(uint32 idx) const;
        const FeaturesRecord& feature(const Point2ui& p) const;
        
        Pixel& pixel(uint32 idx);
        Pixel& pixel(const Point2ui& p);
        Pixel& pixel(const Point2& p);
        const Pixel& pixel(const Point2ui& p) const;

        Pixel& operator()(const Point2& p);

        void exportImage(BufferType type) const;

    private:
        Vec2ui  _res;
        Bounds2 _bounds;

        ToneOperator _toneOp;
        Float _exposure;

        Float _splatScale;

        std::unique_ptr<Color[]> _preview;
        std::unique_ptr<Pixel[]> _pixels;
        std::unique_ptr<FeaturesRecord[]> _feats;
    };


}