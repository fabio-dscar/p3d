///////////////////////////////////////////////////////////////////////
//
// P3D Course
//   You should develop your rayTracing( Ray ray, int depth, float RefrIndex) which returns a color and
// to develop your load_NFF function
//
///////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <memory>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <Utils.h>
#include <Scene.h>
#include <NFFParser.h>
#include <Ray.h>

using namespace Photon;

//#include "scene.h"

#define CAPTION "Photon Tracer"

#define VERTEX_COORD_ATTRIB 0
#define COLOR_ATTRIB 1

#define MAX_DEPTH 6

// Points defined by 2 attributes: positions which are stored in vertices array and colors which are stored in colors array
float *colors;
float *vertices;

/*
std::vector<Color3> colors;
std::vector<Vec2> vertices;
*/

int size_vertices;
int size_colors;

GLfloat m[16];  //projection matrix initialized by ortho function

GLuint VaoId;
GLuint VboId[2];

GLuint VertexShaderId, FragmentShaderId, ProgramId;
GLint UniformId;

std::shared_ptr<Scene> scene = nullptr;
int RES_X, RES_Y;

enum DrawMode {
    DRAW_POINTS = 0,
    DRAW_LINE = 1,
    DRAW_FRAME = 2
};

DrawMode drawMode = DrawMode::DRAW_LINE;

int WindowHandle = 0;

///////////////////////////////////////////////////////////////////////  RAY-TRACE SCENE

/*Color rayTracing(Ray ray, int depth, float RefrIndex) {
    //INSERT HERE YOUR CODE
}*/

struct Color {
    float r, g, b;
};

Color3 rayTracing(Ray ray, int depth, float RefrIndex) {
    //INSERT HERE YOUR CODE
    return Color3(0.5f, 0.5f, 0.0f);
}

Color rayTracing2(int depth, float RefrIndex) {
    //INSERT HERE YOUR CODE
    return { 0.5f, 0.5f, 0.0f };
}

Ray calculateRay(int x, int y) {
    const Camera cam = scene->getCamera();
    Vec3 origin = cam.getPosition();

    Vec3 dir = -glm::length(cam.getPosition() - cam.getTarget()) * cam.getN() //glm::length(cam.getPosition() - cam.getTarget())
             + cam.getH() * ((float)y / (float)cam.getHeight() - 0.5f) * cam.getV()
             + cam.getW() * ((float)x / (float)cam.getWidth() - 0.5f) * cam.getU();

    dir = glm::normalize(dir);

    /*dir = -glm::length(cam.getPosition() - cam.getTarget()) * cam.getN() //glm::length(cam.getPosition() - cam.getTarget())
        + (-cam.getH() + (cam.getH()*2.0f) * ( ((float)y + 0.5f*y) / (float)cam.getHeight() )) * cam.getV()
        + (-cam.getW() + (cam.getW()*2.0f) * ( ((float)x + 0.5f*x) / (float)cam.getWidth() )) * cam.getU();

    dir = glm::normalize(dir);
    */

    Ray r(origin, dir);
    return r;
}

/////////////////////////////////////////////////////////////////////// SHADERs
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

void createShaderProgram() {
    VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShaderId, 1, &VertexShader, 0);
    glCompileShader(VertexShaderId);

    FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShaderId, 1, &FragmentShader, 0);
    glCompileShader(FragmentShaderId);

    ProgramId = glCreateProgram();
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);

    glBindAttribLocation(ProgramId, VERTEX_COORD_ATTRIB, "in_Position");
    glBindAttribLocation(ProgramId, COLOR_ATTRIB, "in_Color");

    glLinkProgram(ProgramId);
    UniformId = glGetUniformLocation(ProgramId, "Matrix");

    Utils::checkOpenGLError("ERROR: Could not create shaders.");
}

void destroyShaderProgram() {
    glUseProgram(0);
    glDetachShader(ProgramId, VertexShaderId);
    glDetachShader(ProgramId, FragmentShaderId);

    glDeleteShader(FragmentShaderId);
    glDeleteShader(VertexShaderId);
    glDeleteProgram(ProgramId);

    Utils::checkOpenGLError("ERROR: Could not destroy shaders.");
}
/////////////////////////////////////////////////////////////////////// VAOs & VBOs
void createBufferObjects() {
    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);
    glGenBuffers(2, VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

    /* Nao e necessario fazer glBufferData, ou seja o envio dos pontos para a placa grafica pois isso
    e feito na drawPoints em tempo de execucao com GL_DYNAMIC_DRAW */

    glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
    glVertexAttribPointer(VERTEX_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
    glEnableVertexAttribArray(COLOR_ATTRIB);
    glVertexAttribPointer(COLOR_ATTRIB, 3, GL_FLOAT, 0, 0, 0);

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
    glDisableVertexAttribArray(COLOR_ATTRIB);
    
    Utils::checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void destroyBufferObjects() {
    glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
    glDisableVertexAttribArray(COLOR_ATTRIB);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, VboId);
    glDeleteVertexArrays(1, &VaoId);
    
    Utils::checkOpenGLError("ERROR: Could not destroy VAOs and VBOs.");
}

void drawPoints() {
    glBindVertexArray(VaoId);
    glUseProgram(ProgramId);

    glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
    glBufferData(GL_ARRAY_BUFFER, size_vertices, vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
    glBufferData(GL_ARRAY_BUFFER, size_colors, colors, GL_DYNAMIC_DRAW);

    glUniformMatrix4fv(UniformId, 1, GL_FALSE, m);
    glDrawArrays(GL_POINTS, 0, RES_X * RES_Y);
    glFinish();

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Utils::checkOpenGLError("ERROR: Could not draw scene.");
}

/////////////////////////////////////////////////////////////////////// CALLBACKS

// Render function by primary ray casting from the eye towards the scene's objects

void tracePatch() {

}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT);

    int index_pos = 0;
    int index_col = 0;

    for (int y = 0; y < RES_Y; y++) {
        for (int x = 0; x < RES_X; x++) {
            const std::vector<std::shared_ptr<Geometry>> objs = scene->getGeometry();
            const std::vector <PointLight> lights = scene->getLights();
            const Camera cam = scene->getCamera();

            Ray r = calculateRay(x, y);
            
            // Calculate intersection
            HitInfo info, curr;
            curr._t = 1000000000;
            curr._hit = false;
            for (std::shared_ptr<Geometry> obj : objs)
                if (obj->intersectRay(r, info))
                    if (info._t > 0.0f && curr._t > info._t) 
                        curr = info;

            // Calculate lighting
            Color3 color = scene->getBackgroundColor();
            if (curr._hit) {
                const Material mtl = curr._obj->getMaterial();
                color = Vec3(0.0f);

                // View vector
                Vec3 v = glm::normalize(curr._point - cam.getPosition());

                for (PointLight light : lights) {
                    Vec3 l = glm::normalize(light.getPosition() - curr._point);
                    float NdotL = glm::dot(curr._normal, l);
                    if (NdotL <= 0)
                        continue;

                    // Reflected direction
                    Vec3 r = glm::normalize(2.0f * NdotL * curr._normal - l);
                    float RdotV = std::pow(std::abs(glm::dot(r, v)), mtl.getShininess());
                   
                    color += NdotL * mtl.getDiff() * light.getColor() * mtl.getColor();
                    color += RdotV * mtl.getSpec() * light.getColor() * mtl.getColor();
                }

            } 

            colors[index_col++] = (float)color.r;
            colors[index_col++] = (float)color.g;
            colors[index_col++] = (float)color.b;

            vertices[index_pos++] = (float)x;
            vertices[index_pos++] = (float)y;

            if (drawMode == DrawMode::DRAW_POINTS) {  // desenhar o conteudo da janela ponto a ponto
                drawPoints();
                index_pos = 0;
                index_col = 0;
            }
        }

        //std::cout << "Line " << y << std::endl;
        if (drawMode == DrawMode::DRAW_LINE) {  // desenhar o conteudo da janela linha a linha
            drawPoints();
            index_pos = 0;
            index_col = 0;
        }
    }

    if (drawMode == DrawMode::DRAW_FRAME) //preenchar o conteudo da janela com uma imagem completa
        drawPoints();

    std::cout << "Finished!" << std::endl;
}

void cleanup() {
    destroyShaderProgram();
    destroyBufferObjects();

    // Release the memory
    //scene.reset();
}

void ortho(float left, float right, float bottom, float top,
           float nearp, float farp) {
    m[0 * 4 + 0] = 2 / (right - left);
    m[0 * 4 + 1] = 0.0;
    m[0 * 4 + 2] = 0.0;
    m[0 * 4 + 3] = 0.0;
    m[1 * 4 + 0] = 0.0;
    m[1 * 4 + 1] = 2 / (top - bottom);
    m[1 * 4 + 2] = 0.0;
    m[1 * 4 + 3] = 0.0;
    m[2 * 4 + 0] = 0.0;
    m[2 * 4 + 1] = 0.0;
    m[2 * 4 + 2] = -2 / (farp - nearp);
    m[2 * 4 + 3] = 0.0;
    m[3 * 4 + 0] = -(right + left) / (right - left);
    m[3 * 4 + 1] = -(top + bottom) / (top - bottom);
    m[3 * 4 + 2] = -(farp + nearp) / (farp - nearp);
    m[3 * 4 + 3] = 1.0;
}

void reshape(int w, int h) {
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, w, h);
    ortho(0, (float)RES_X, 0, (float)RES_Y, -1.0, 1.0);
}

/////////////////////////////////////////////////////////////////////// SETUP
void setupCallbacks() {
    glutCloseFunc(cleanup);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
}

void setupGLEW() {
    glewExperimental = GL_TRUE;
    GLenum result = glewInit();
    if (result != GLEW_OK) {
        std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
        exit(EXIT_FAILURE);
    }
    GLenum err_code = glGetError();
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version: %s\n", glGetString(GL_VERSION));
    printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void setupGLUT(int argc, char* argv[]) {
    glutInit(&argc, argv);

    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutInitWindowPosition(640, 100);
    glutInitWindowSize(RES_X, RES_Y);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glDisable(GL_DEPTH_TEST);
    WindowHandle = glutCreateWindow(CAPTION);
    if (WindowHandle < 1) {
        std::cerr << "ERROR: Could not create a new rendering window." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void init(int argc, char* argv[]) {
    setupGLUT(argc, argv);
    setupGLEW();
    std::cerr << "CONTEXT: OpenGL v" << glGetString(GL_VERSION) << std::endl;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    createShaderProgram();
    createBufferObjects();
    setupCallbacks();
}

int main(int argc, char* argv[]) {
    /*INSERT HERE YOUR CODE FOR PARSING NFF FILES
        scene = new Scene();
    if (!(scene->load_nff("jap.nff"))) return 0;
    

    RES_X = scene->GetCamera()->GetResX();
    RES_Y = scene->GetCamera()->GetResY();
    */

    if (argc < 1) {
        std::cerr << "Usage: " << argv[0] << " <NFF_file>" << std::endl;
        std::cin.get();
        return EXIT_FAILURE;
    }

    // Parse scene
    //std::string filePath(argv[1]);
    std::string filePath("mount_very_high.nff");
    scene = Utils::NFFParser::fromFile(filePath);
    if (!scene)
        Utils::throwError("Failed to load scene.");

    RES_X = scene->getCamera().getWidth();
    RES_Y = scene->getCamera().getHeight();

    switch (drawMode) {
        case DRAW_POINTS:
            size_vertices = 2 * sizeof(float);
            size_colors = 3 * sizeof(float);

            std::cout << "DRAWING MODE: POINT BY POINT" << std::endl;
            break;
        case DRAW_LINE:
            size_vertices = 2 * RES_X * sizeof(float);
            size_colors = 3 * RES_X * sizeof(float);

            std::cout << "DRAWING MODE: LINE BY LINE" << std::endl;
            break;
        case DRAW_FRAME:
            size_vertices = 2 * RES_X*RES_Y * sizeof(float);
            size_colors = 3 * RES_X*RES_Y * sizeof(float);

            std::cout << "DRAWING MODE: FULL IMAGE" << std::endl;
            break;
        default:
            printf("Draw mode not valid \n");
            exit(0);
    }

    printf("resx = %d  resy= %d.\n", RES_X, RES_Y);

    /*vertices.resize(size_vertices / (2 * sizeof(float)));
    colors.resize(size_colors / (3 * sizeof(float)));
    */

    // Allocate memory
    vertices = (float*)malloc(size_vertices);
    if (vertices == NULL) 
        exit(1);

    colors = (float*)malloc(size_colors);
    if (colors == NULL) 
        exit(1);

    init(argc, argv);
    glutMainLoop();
    exit(EXIT_SUCCESS);
}
///////////////////////////////////////////////////////////////////////