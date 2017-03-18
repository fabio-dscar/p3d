#pragma once

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <Vector.h>
#include <Scene.h>

namespace Photon {

    class Renderer {
    public:
        Renderer() : _clearColor(0.1f, 0.1f, 0.1f, 1.0f), _window(0), _width(DEFAULT_WIDTH), _height(DEFAULT_HEIGHT) { }

        void renderScene(const Scene& scene) {

        }

    private:
        unsigned int _width;
        unsigned int _height;
        int _window;
        Color4 _clearColor;

        void setupGLEW() {
            glewExperimental = GL_TRUE;
            GLenum result = glewInit();
            if (result != GLEW_OK) {
                std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
                exit(EXIT_FAILURE);
            }

            GLenum err_code = glGetError();
            std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
            std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
            std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
            std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
        }

        void setupGLUT(int argc, char* argv[]) {
            glutInit(&argc, argv);

            glutInitContextVersion(3, 3);
            glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
            glutInitContextProfile(GLUT_CORE_PROFILE);

            glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

            glutInitWindowPosition(0, 0);
            glutInitWindowSize(_width, _height);
            glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
            glDisable(GL_DEPTH_TEST);
            _window = glutCreateWindow("Photon Tracer");
            if (_window < 1) {
                std::cerr << "ERROR: Could not create a new rendering window." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    };

}