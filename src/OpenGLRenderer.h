#pragma once

#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <MathDefs.h>

#define CAPTION "Photon Tracer"

#define VERTEX_COORD_ATTRIB 0
#define COLOR_ATTRIB 1

using namespace Photon;

namespace Photon {

    // Forward declaration
    class Scene;

    namespace OpenGL {

        class OpenGLRenderer {
        public:
            OpenGLRenderer(const std::shared_ptr<Scene> scene);

            void initialize(int argc, char* argv[]);
            void startRender();
            void resize(int32 w, int32 h);
            void cleanup();
            void renderToScreen() const;

            // Set callbacks
            void setDisplayCallback(void(*display)());
            void setReshapeCallback(void(*reshape)(int w, int h));
            void setCleanupCallback(void(*cleanup)());

        private:
            std::shared_ptr<Scene> _scene;

            uint32 _width;
            uint32 _height;
            int32  _window;

            GLuint _fragShaderId, _vertShaderId, _programId;
            GLint _uniformId;

            GLuint _vaoId;
            GLuint _vboId[2];

            uint32 _sizePoints;
            uint32 _sizeColors;

            GLfloat _m[16];

            std::vector<Float> _points;

            void setupGLEW();
            void setupGLUT(int argc, char* argv[]);

            void ortho(float left, float right, float bottom, float top, float nearp, float farp);

            void createShaderProgram();
            void destroyShaderProgram();

            void createBufferObjects();
            void destroyBufferObjects();       
        };

 
    }

}