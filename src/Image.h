#pragma once

#include <FreeImage.h>

#include <PhotonMath.h>
#include <Vector.h>

using namespace Photon::Math;

namespace Photon {

    class Image {
    public:
        Image(const Vec2ui res, uint32 bpp, uint32 nChannels, std::unique_ptr<Float[]>& bits);

        void loadImage(const std::string& filename);
        void exportImage(const std::string& filename, const std::string& ext);

        static bool isHdrExtension(const std::string& ext);
    private:
        void setPixelValue(uint32 x, uint32 y, FIBITMAP* bitmap, const Float* vals, uint32 bpp);
        void setPixelValueHdr(uint32 x, uint32 y, FIBITMAP* bitmap, const Float* vals, uint32 bpp);

        static FREE_IMAGE_TYPE getFormatType(uint32 bpp, bool isHdr);

        //std::string _filename;
        //std::string _ext;

        Vec2ui _res;
        uint32 _bpp;
        uint32 _nChannels;

        std::unique_ptr<Float[]> _bits;
    };

}