#include <Image.h>

#include <Threading.h>

using namespace Photon;

Image::Image(const Vec2ui res, uint32 bpp, uint32 nChannels, std::unique_ptr<Float[]>& bits)
    : _bits(std::move(bits)), _res(res), _bpp(bpp), _nChannels(nChannels) {

    _filename = "testststststs";
    _ext = "png";
}

void Image::exportImage() {
    FIBITMAP* bitmap = FreeImage_Allocate(_res.x, _res.y, _bpp);
    if (bitmap) {
        // Convert camera's film image to [0, 255 range]
        Threading::parallelFor(0, _res.y, 32, [&](uint32 i) {
            const Float* pColor = _bits.get();
            BYTE* bits = (BYTE *)FreeImage_GetBits(bitmap);

            for (uint32 x = 0; x < _res.x; x++) {
                uint32 idx = x + _res.x * i;
                setPixelValue(x, i, bitmap, &pColor[_nChannels * idx], _bpp);
            }
        });

        std::string outName(_filename + "." + _ext);
        FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFormat(_ext.c_str());
        if (format == FIF_UNKNOWN) {
            std::cerr << "Error: Unknown export image format." << std::endl;
            return;
        }

        FreeImage_Save(format, bitmap, outName.c_str(), 0);
        FreeImage_Unload(bitmap);
    }
}

void Image::setPixelValue(uint32 x, uint32 y, FIBITMAP* bitmap, const Float* vals, uint32 bpp) {
    BYTE* bits = (BYTE *)FreeImage_GetScanLine(bitmap, y);

    switch (bpp) {
        case 8:
            bits[x] = clamp<uint32>(vals[0] * 255.0, 0u, 255u);
            break;
        case 24:
            bits[3 * x + FI_RGBA_RED]   = clamp<uint32>(vals[0] * 255.0, 0u, 255u);
            bits[3 * x + FI_RGBA_GREEN] = clamp<uint32>(vals[1] * 255.0, 0u, 255u);
            bits[3 * x + FI_RGBA_BLUE]  = clamp<uint32>(vals[2] * 255.0, 0u, 255u);
            break;
        case 32:
            bits[4 * x + FI_RGBA_RED]   = clamp<uint32>(vals[0] * 255.0, 0u, 255u);
            bits[4 * x + FI_RGBA_GREEN] = clamp<uint32>(vals[1] * 255.0, 0u, 255u);
            bits[4 * x + FI_RGBA_BLUE]  = clamp<uint32>(vals[2] * 255.0, 0u, 255u);
            bits[4 * x + FI_RGBA_ALPHA] = 255;
            break;
        default:
            break;
    };
}