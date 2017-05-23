#include <Film.h>

#include <Threading.h>
#include <Image.h>

using namespace Photon;
using namespace Photon::Threading;

Film::Film(const Vec2ui& res)
    : _res(res), _toneOp(FILMIC), _exposure(0.6), _bounds(0),
    _pixels(nullptr), _preview(nullptr), _feats(nullptr),
    _splatScale(1) {

    _bounds.expand(Point2(res.x, res.y));

    uint32 nPixels = pixelArea();
    _pixels  = std::make_unique<Pixel[]>(nPixels);
    _preview = std::make_unique<Color[]>(nPixels);

    memset(_preview.get(), 0, nPixels);

    _feats = std::make_unique<FeaturesRecord[]>(nPixels);
}

void Film::setToneOperator(ToneOperator op) {
    _toneOp = op;
}

void Film::setExposure(Float exp) {
    _exposure = exp;
}

const Bounds2& Film::bounds() const {
    return _bounds;
}

Vec2ui Film::resolution() const {
    return _res;
}

uint32 Film::width() const {
    return _res.x;
}

uint32 Film::height() const {
    return _res.y;
}

Float Film::aspect() const {
    return Float(_res.x) / Float(_res.y);
}

uint32 Film::pixelArea() const {
    return _res.x * _res.y;
}

void Film::addPreviewSample(uint32 x, uint32 y, const Color& color) {
    if (_preview.get()) {
        Color hdr = ToneMap(_toneOp, color, _exposure);
        Pixel& p = pixel(Point2ui(x, y));

        _preview[x + _res.x * y] = hdr;
    }
}

void Film::addColorSample(uint32 x, uint32 y, const Color& color) {
    Pixel& p  = pixel(Point2ui(x, y));
    p.color  += color;
    p.nSamples++;
}

void Film::addSplatSample(const Point2& pt, const Color& splat) {
    Pixel& p  = pixel(pt);
    p.splat.add(splat);
}

void Film::addFeatureSample(const FeaturesRecord& record) {
    if (_feats.get()) {
        FeaturesRecord& rec = feature(record.raster);
        rec.dist   += record.dist;
        rec.normal += record.normal;
        rec.raster  = record.raster;     
        rec.nSamples++;
    }
}

const Float* Film::preview() const {
    return &_preview[0][0];
}

std::unique_ptr<Float[]> Film::color() const {
    const Float nPixels = pixelArea();
    const Float nChannels = 3;

    if (!_pixels)
        return nullptr;

    std::unique_ptr<Float[]> out = std::make_unique<Float[]>(nPixels * nChannels);
    parallelFor(0, _res.x, 32, [&](uint32 i) {
        for (uint32 y = 0; y < _res.y; ++y) {
            const Pixel& px = pixel(Point2ui(i, y));

            // TODO: filters, for now apply box
            uint32 idx = i + _res.x * y;

            Color tone = ToneMap(_toneOp, px.color / px.nSamples, _exposure);
            out[nChannels * idx]     = tone.r;
            out[nChannels * idx + 1] = tone.g;
            out[nChannels * idx + 2] = tone.b;

            Color splat = Color(px.splat.r.val(),
                                px.splat.g.val(),
                                px.splat.b.val());

            splat = ToneMap(_toneOp, splat / px.nSamples, _exposure);
            out[nChannels * idx]     += splat.r;
            out[nChannels * idx + 1] += splat.g;
            out[nChannels * idx + 2] += splat.b;
        }
    });

    return std::move(out);
}

std::unique_ptr<Float[]> Film::depth() const {
    const Float nPixels   = pixelArea();
    const Float nChannels = 1;

    if (!_feats)
        return nullptr;

    // Find maximum distance
    Float maxDist = 0;
    parallelFor(0, _res.x, 32, [&](uint32 i) {
        for (uint32 y = 0; y < _res.y; y++) {
            const FeaturesRecord& rec = feature(Point2ui(i, y));
            maxDist = std::max(maxDist, rec.dist / rec.nSamples);
        }
    });

    // Output normalized buffer
    std::unique_ptr<Float[]> out = std::make_unique<Float[]>(nPixels * nChannels);
    parallelFor(0, _res.x, 32, [&](uint32 i) {
        for (uint32 y = 0; y < _res.y; y++) {
            const FeaturesRecord& rec = feature(Point2ui(i, y));

            uint32 idx = i + _res.x * y;
            out[nChannels * idx] = rec.dist / (rec.nSamples * maxDist);
        }
    });

    return std::move(out);
}

std::unique_ptr<Float[]> Film::normals() const {
    const Float nPixels   = pixelArea();
    const Float nChannels = 3;

    if (!_feats)
        return nullptr;

    std::unique_ptr<Float[]> out = std::make_unique<Float[]>(nPixels * nChannels);
    parallelFor(0, _res.x, 32, [&](uint32 i) {
        for (uint32 y = 0; y < _res.y; y++) {
            const FeaturesRecord& rec = feature(Point2ui(i, y));
            const Normal n = abs(rec.normal / rec.nSamples);

            uint32 idx = i + _res.x * y;
            out[nChannels * idx]     = n.x;
            out[nChannels * idx + 1] = n.y;
            out[nChannels * idx + 2] = n.z;
        }
    });

    return std::move(out);
}

std::unique_ptr<Float[]> Film::sampleDensity() const {
    const Float nPixels   = pixelArea();
    const Float nChannels = 1;

    if (!_feats)
        return nullptr;

    // Find highest number of samples on a pixel
    uint32 maxSamples = 0;
    parallelFor(0, _res.x, 32, [&](uint32 i) {
        for (uint32 y = 0; y < _res.y; y++) {
            const Pixel& px = pixel(Point2ui(i, y));
            maxSamples = std::max(maxSamples, px.nSamples);
        }
    });

    // Output normalized buffer
    std::unique_ptr<Float[]> out = std::make_unique<Float[]>(nPixels * nChannels);
    parallelFor(0, _res.x, 32, [&](uint32 i) {
        for (uint32 y = 0; y < _res.y; y++) {
            const Pixel& px = pixel(Point2ui(i, y));

            uint32 idx = i + _res.x * y;
            out[nChannels * idx] = (Float)px.nSamples / maxSamples;
        }
    });

    return std::move(out);
}

std::unique_ptr<Float[]> Film::visibility() const {
    // TODO
    const Float nPixels = pixelArea();
    const Float nChannels = 1;

    if (!_feats)
        return nullptr;

    // Find maximum distance
    Float maxDist = 0;
    parallelFor(0, _res.x, 32, [&](uint32 i) {
        for (uint32 y = 0; y < _res.y; y++) {
            const FeaturesRecord& rec = feature(Point2ui(i, y));
            maxDist = std::max(maxDist, rec.dist / rec.nSamples);
        }
    });

    // Output normalized buffer
    std::unique_ptr<Float[]> out = std::make_unique<Float[]>(nPixels * nChannels);
    parallelFor(0, _res.x, 32, [&](uint32 i) {
        for (uint32 y = 0; y < _res.y; y++) {
            const FeaturesRecord& rec = feature(Point2ui(i, y));

            uint32 idx = i + _res.x * y;
            out[nChannels * idx] = rec.dist / (rec.nSamples * maxDist);
        }
    });

    return std::move(out);
}

FeaturesRecord& Film::feature(const Point2& p) {
    uint32 x = (uint32)p.x;
    uint32 y = (uint32)p.y;

    uint32 idx = x + _res.x * y;

    return _feats[idx];
}

const FeaturesRecord& Film::feature(uint32 idx) const {
    return _feats[idx];
}

const FeaturesRecord& Film::feature(const Point2ui& p) const {
    return _feats[p.x + _res.x * p.y];
}

FeaturesRecord& Film::feature(const Point2ui& p) {
    return _feats[p.x + _res.x * p.y];
}

Pixel& Film::pixel(uint32 idx) {
    return _pixels[idx];
}

const Pixel& Film::pixel(const Point2ui& p) const {
    return _pixels[p.x + _res.x * p.y];
}

Pixel& Film::pixel(const Point2ui& p) {
    return _pixels[p.x + _res.x * p.y];
}

Pixel& Film::pixel(const Point2& p) {
    uint32 x = (uint32)p.x;
    uint32 y = (uint32)p.y;

    uint32 idx = x + _res.x * y;

    return _pixels[idx];
}

Pixel& Film::operator()(const Point2& p) {
    uint32 x = (uint32)p.x;
    uint32 y = (uint32)p.y;

    uint32 idx = x + _res.x * y;

    return _pixels[idx];
}

void Film::exportImage(BufferType type) const {
    std::unique_ptr<Float[]> buffer = nullptr;

    uint32 bpp, nChannels;

    switch (type) {
        case COLOR:
            bpp = 24; nChannels = 3;
            buffer = color();
            break;
        case NORMAL:
            bpp = 24; nChannels = 3;
            buffer = normals();
            break;
        case DEPTH:
            bpp = 8; nChannels = 1;
            buffer = depth();
            break;
        case VISIBILITY:
            bpp = 8; nChannels = 1;
            buffer = visibility();
            break;
        case SAMPLES:
            bpp = 8; nChannels = 1;
            buffer = sampleDensity();
            break;
        default:
            break;
    };

    if (buffer.get()) {
        Image img = Image(_res, bpp, nChannels, buffer);
        img.exportImage();
    }
}