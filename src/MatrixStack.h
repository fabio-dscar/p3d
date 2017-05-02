#pragma once

#include <vector>

#include <Transform.h>

namespace Photon {

    const uint32 MAX_NUM_MATRICES = 8;

    class MatrixStack {
    public:
        MatrixStack() : _currMat() { 
            _matStack.resize(MAX_NUM_MATRICES); 
        }

        ~MatrixStack() {}

        void loadIdentity();
        void pushMatrix();
        void popMatrix();

        void scale(const Vec3& scale);
        void translate(const Vec3& translation);
        void rotateX(Float angle);
        void rotateY(Float angle);
        void rotateZ(Float angle);

        void setMat(const Mat4& mat);
        void mulMat(const Mat4& mat);

        const Mat4& loadMatrix() const;

    private:
        std::vector<Mat4> _matStack;
        Mat4 _currMat;
    };

}