#include <OpenGLRenderer.h>

#include <iostream>
#include <thread>

#include <Utils.h>
#include <Scene.h>

using namespace Photon;
using namespace Photon::OpenGL;

const GLchar* VertexShader =
{
    "#version 330 core\n"

    "in vec2 in_Position;\n"
    "in vec3 in_Color;\n"
    "uniform mat4 Matrix;\n"
    "out vec4 color;\n"

    "void main(void)\n"
    "{\n"
    "	vec4 position = vec4(in_Position, 0.0, 1.0);\n"
    "	color = vec4(in_Color, 1.0);\n"
    "	gl_Position = Matrix * position;\n"
    "}\n"
};

const GLchar* FragmentShader =
{
    "#version 330 core\n"

    "in vec4 color;\n"
    "out vec4 out_Color;\n"

    "void main(void)\n"
    "{\n"
    "	out_Color = color;\n"
    "}\n"
};


OpenGLRenderer::OpenGLRenderer(const std::shared_ptr<Scene> scene) {
    _scene = scene;

    const Film& film = scene->getCamera().film();

    _width = film.width();
    _height = film.height();

    // Populate the points list
    _points.resize(_width * _height * 2);
    uint32 vertInd = 0;
    for (uint32 y = 0; y < _height; y++) {
        for (uint32 x = 0; x < _width; x++) {
            _points[vertInd++] = Float(x);
            _points[vertInd++] = Float(y);
        }
    }

    _sizePoints = 2 * film.pixelArea() * sizeof(Float);
    _sizeColors = 3 * film.pixelArea() * sizeof(Float);
}

void OpenGLRenderer::initialize(int argc, char* argv[]) {
    setupGLUT(argc, argv);
    setupGLEW();
    std::cerr << "CONTEXT: OpenGL v" << glGetString(GL_VERSION) << std::endl;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    createShaderProgram();
    createBufferObjects();
    //setupCallbacks();
}

void OpenGLRenderer::startRender() {
    glutMainLoop();
}

void OpenGLRenderer::resize(int32 w, int32 h) {
    const Film& film = _scene->getCamera().film();

    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, w, h);
    ortho(0, (float)film.width(), 0, (float)film.height(), -1.0, 1.0);
}

void OpenGLRenderer::cleanup() {
    destroyShaderProgram();
    destroyBufferObjects();
}

void OpenGLRenderer::renderToScreen() const {
    const Film& film = _scene->getCamera().film();

    // Draw camera film
    glBindVertexArray(_vaoId);
    glUseProgram(_programId);

    glBindBuffer(GL_ARRAY_BUFFER, _vboId[0]);
    glBufferData(GL_ARRAY_BUFFER, _sizePoints, &_points[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, _vboId[1]);
    glBufferData(GL_ARRAY_BUFFER, _sizeColors, &(film.image()[0]), GL_DYNAMIC_DRAW);

    glUniformMatrix4fv(_uniformId, 1, GL_FALSE, _m);
    glBindVertexArray(_vaoId);
    glDrawArrays(GL_POINTS, 0, film.pixelArea());
    glFlush();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    glutPostRedisplay();
}

void OpenGLRenderer::setDisplayCallback(void(*display)()) {
    glutDisplayFunc(display);
}

void OpenGLRenderer::setReshapeCallback(void(*reshape)(int w, int h)) {
    glutReshapeFunc(reshape);
}

void OpenGLRenderer::setCleanupCallback(void(*cleanup)()) {
    glutCloseFunc(cleanup);
}

void OpenGLRenderer::setupGLEW() {
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

void OpenGLRenderer::setupGLUT(int argc, char* argv[]) {
    glutInit(&argc, argv);

    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutInitWindowPosition(50, 50);
    glutInitWindowSize(_width, _height);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glDisable(GL_DEPTH_TEST);
    _window = glutCreateWindow("Photon Tracer");
    if (_window < 1) {
        std::cerr << "ERROR: Could not create a new rendering window." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void OpenGLRenderer::createShaderProgram() {
    _vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(_vertShaderId, 1, &VertexShader, 0);
    glCompileShader(_vertShaderId);

    _fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(_fragShaderId, 1, &FragmentShader, 0);
    glCompileShader(_fragShaderId);

    _programId = glCreateProgram();
    glAttachShader(_programId, _vertShaderId);
    glAttachShader(_programId, _fragShaderId);

    glBindAttribLocation(_programId, VERTEX_COORD_ATTRIB, "in_Position");
    glBindAttribLocation(_programId, COLOR_ATTRIB, "in_Color");

    glLinkProgram(_programId);
    _uniformId = glGetUniformLocation(_programId, "Matrix");

    Utils::checkOpenGLError("ERROR: Could not create shaders.");
}

void OpenGLRenderer::destroyShaderProgram() {
    glUseProgram(0);
    glDetachShader(_programId, _vertShaderId);
    glDetachShader(_programId, _fragShaderId);

    glDeleteShader(_fragShaderId);
    glDeleteShader(_vertShaderId);
    glDeleteProgram(_programId);

    Utils::checkOpenGLError("ERROR: Could not destroy shaders.");
}

void OpenGLRenderer::ortho(float left, float right, float bottom, float top,
           float nearp, float farp) {
    _m[0 * 4 + 0] = 2 / (right - left);
    _m[0 * 4 + 1] = 0.0;
    _m[0 * 4 + 2] = 0.0;
    _m[0 * 4 + 3] = 0.0;
    _m[1 * 4 + 0] = 0.0;
    _m[1 * 4 + 1] = 2 / (top - bottom);
    _m[1 * 4 + 2] = 0.0;
    _m[1 * 4 + 3] = 0.0;
    _m[2 * 4 + 0] = 0.0;
    _m[2 * 4 + 1] = 0.0;
    _m[2 * 4 + 2] = -2 / (farp - nearp);
    _m[2 * 4 + 3] = 0.0;
    _m[3 * 4 + 0] = -(right + left) / (right - left);
    _m[3 * 4 + 1] = -(top + bottom) / (top - bottom);
    _m[3 * 4 + 2] = -(farp + nearp) / (farp - nearp);
    _m[3 * 4 + 3] = 1.0;
}

void OpenGLRenderer::createBufferObjects() {
    glGenVertexArrays(1, &_vaoId);
    glBindVertexArray(_vaoId);
    glGenBuffers(2, _vboId);

    glBindBuffer(GL_ARRAY_BUFFER, _vboId[0]);
    glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);

#if PHOTON_DOUBLE
    glVertexAttribPointer(VERTEX_COORD_ATTRIB, 2, GL_DOUBLE, 0, 0, 0);
#else
    glVertexAttribPointer(VERTEX_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);
#endif

    glBindBuffer(GL_ARRAY_BUFFER, _vboId[1]);
    glEnableVertexAttribArray(COLOR_ATTRIB);

#if PHOTON_DOUBLE
    glVertexAttribPointer(COLOR_ATTRIB, 3, GL_DOUBLE, 0, 0, 0);
#else
    glVertexAttribPointer(COLOR_ATTRIB, 3, GL_FLOAT, 0, 0, 0);
#endif

    // Unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
    glDisableVertexAttribArray(COLOR_ATTRIB);

    Utils::checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void OpenGLRenderer::destroyBufferObjects() {
    glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
    glDisableVertexAttribArray(COLOR_ATTRIB);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, _vboId);
    glDeleteVertexArrays(1, &_vaoId);

    Utils::checkOpenGLError("ERROR: Could not destroy VAOs and VBOs.");
}