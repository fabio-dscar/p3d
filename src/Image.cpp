#include <Image.h>

#include <Threading.h>

using namespace Photon;

Image::Image(const Vec2ui res, uint32 bpp, uint32 nChannels, std::unique_ptr<Float[]>& bits)
    : _bits(std::move(bits)), _res(res), _bpp(bpp), _nChannels(nChannels) {

    //_filename = "default";
    //_ext = "png";
}

void Image::loadImage(const std::string& filename) {

}

void Image::exportImage(const std::string& filename, const std::string& ext) {
    bool isHdr = isHdrExtension(ext);

    FIBITMAP* bitmap = FreeImage_AllocateT(getFormatType(_bpp, isHdr), _res.x, _res.y, _bpp);
    if (bitmap) {
        // Convert camera's film image to [0, 255 range]
        Threading::parallelFor(0, _res.y, 32, [&](uint32 i) {
            const Float* pColor = _bits.get();

            for (uint32 x = 0; x < _res.x; x++) {
                uint32 idx = x + _res.x * i;
                if (isHdr)
                    setPixelValueHdr(x, i, bitmap, &pColor[_nChannels * idx], _bpp);
                else
                    setPixelValue(x, i, bitmap, &pColor[_nChannels * idx], _bpp);
            }
        });

        std::string outName(filename + "." + ext);
        FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFormat(ext.c_str());
        if (format == FIF_UNKNOWN) {
            std::cerr << "Error: Unknown export image format." << std::endl;
            return;
        }
        
        FreeImage_Save(format, bitmap, outName.c_str(), 0);
        FreeImage_Unload(bitmap);
    }
}

bool Image::isHdrExtension(const std::string& ext) {
    FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFormat(ext.c_str());
    switch (format) {
        case FIF_EXR:
        case FIF_HDR:
            return true;
        default:
            return false;
    }
}

void Image::setPixelValue(uint32 x, uint32 y, FIBITMAP* bitmap, const Float* vals, uint32 bpp) {
    BYTE* bits = (BYTE*)FreeImage_GetScanLine(bitmap, y);

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

void Image::setPixelValueHdr(uint32 x, uint32 y, FIBITMAP* bitmap, const Float* vals, uint32 bpp) {
    if (bpp == 32) {
        float* bits = (float*)FreeImage_GetScanLine(bitmap, y);
        bits[x] = (float)vals[0];
    } else if (bpp == 96) {
        FIRGBF* bits  = (FIRGBF*)FreeImage_GetScanLine(bitmap, y);
        bits[x].red   = (float)vals[0];
        bits[x].green = (float)vals[1];
        bits[x].blue  = (float)vals[2];
    } else if (bpp == 128) {
        FIRGBAF* bits = (FIRGBAF*)FreeImage_GetScanLine(bitmap, y);
        bits[x].red   = (float)vals[0];
        bits[x].green = (float)vals[1];
        bits[x].blue  = (float)vals[2];
        bits[x].alpha = 1;
    }
}

FREE_IMAGE_TYPE Image::getFormatType(uint32 bpp, bool isHdr) {
    if (isHdr) {
        switch (bpp) {
            case 32:
                return FIT_FLOAT;
                break;
            case 96:
                return FIT_RGBF;
                break;
            case 128:
                return FIT_RGBAF;
                break;
        }
    }

    return FIT_BITMAP;
}
