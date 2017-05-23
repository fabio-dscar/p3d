#pragma once

#include <FreeImage.h>

#include <PhotonMath.h>
#include <Vector.h>

using namespace Photon::Math;

namespace Photon {

    class Image {
    public:
        Image(const Vec2ui res, uint32 bpp, uint32 nChannels, std::unique_ptr<Float[]>& bits);

        void exportImage();

    private:
        void setPixelValue(uint32 x, uint32 y, FIBITMAP* bitmap, const Float* vals, uint32 bpp);

        std::string _filename;
        std::string _ext;

        Vec2ui _res;
        uint32 _bpp;
        uint32 _nChannels;

        std::unique_ptr<Float[]> _bits;
    };

}