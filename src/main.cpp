/*
 * File:   main.cpp
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 2013-10-11, 11:34
 */

#include "pgr.h"
#include <iostream>
#include "PGR_renderer.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

/* Program constants - mostly default values */
#define PGR_GPU 1
#define PGR_CPU 2
#define PGR_WINDOW_WIDTH 1024 // Window width
#define PGR_WINDOW_HEIGHT 768 // Window height
#define PGR_CAMERA_Z 5.3f // Translation of camera in z-axis direction
#define PGR_CAMERA_Y 2.5f // Translation of camera in y-axis direction
#define PGR_CAMERA_X 0.0f // Translation of camera in x-axis direction
#define PGR_ROTATE_X 20.0f // Rotation about x-axis
#define PGR_ROTATE_Y -25.0f // Rotation about y-axis
#define PGR_CORE PGR_CPU //Radiosity computing unit {cpu,gpu}
#define PGR_MAX_AREA 0.1f //Maximal area size of every patch

/* Program global variables - initialized to default values*/
GLuint width = PGR_WINDOW_WIDTH;
GLuint height = PGR_WINDOW_HEIGHT;
GLfloat camera_rot_y = PGR_ROTATE_Y; // Rotation about y-axis
GLfloat camera_rot_x = PGR_ROTATE_X;
GLfloat camera_z = PGR_CAMERA_Z;
bool renderRadiosity = false;


/* Model geometry */
GLuint roomVBO, roomEBO;

/* Shaders */
GLuint iVS, iFS, iProg;
GLuint positionAttrib, colorAttrib, normalAttrib, mvpUniform, laUniform, ldUniform, lightPosUniform;

const char * vertexShaderRoom
    = "#version 130\n in vec3 position; in vec3 color; in vec3 normal; uniform mat4 mvp; out vec4 c; out vec3 n; void main() { gl_Position = mvp*vec4(position,1); c = vec4(color,1); n = normal;}";
const char * fragmentShaderRoom
    = "#version 130\n in vec4 c; out vec4 fragColor; in vec3 n; uniform vec3 la,ld,lightPos; void main() { fragColor = c*vec4(la,1) + c*vec4(ld,1)*max(0.0, dot(normalize(n), normalize(lightPos))); }";


/* Renderer */
PGR_renderer * renderer;

void onInit()
{
    renderer->setMaxArea(PGR_MAX_AREA);
    renderer->init();
}

/**
 * Callback for rendering display. Default rotation and translation is done and
 * correct renderer is called.
 */
void onWindowRedraw()
{
    glm::mat4 projection;

    projection = glm::perspective(90.0f, (float) width / (float) height, 0.1f, 200.0f);

    glm::mat4 mvp = glm::rotate(
                                glm::rotate(
                                            glm::translate(
                                                           projection,
                                                           glm::vec3(PGR_CAMERA_X, -PGR_CAMERA_Y, -camera_z)
                                                           ),
                                            camera_rot_x, glm::vec3(1, 0, 0)
                                            ),
                                camera_rot_y, glm::vec3(0, 1, 0)
                                );
    renderer->setResolution(width, height);


    /* Choose renderer */
    if (!renderRadiosity)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        renderer->drawSceneDefault(mvp);

        SDL_GL_SwapBuffers();
    }
    else
    {
        //TODO here should be radiosity rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        renderer->drawSceneRadiosity(mvp);

        SDL_GL_SwapBuffers();
    }
}

/**
 * Main function. Event handlers are registered and scene and renderer are
 * initialized
 */
int main(int argc, char** argv)
{
    int core;
    if (argc == 2 && string(argv[1]) == "cpu")
    {
        core = PGR_CPU;
    }
    else if (argc == 2 && string(argv[1]) == "gpu")
    {
        core = PGR_GPU;
    }
    else
    {
        core = PGR_CPU;
    }

    try
    {
        // Init SDL - only video subsystem will be used
        if (SDL_Init(SDL_INIT_VIDEO) < 0) throw SDL_Exception();

        // Shutdown SDL when program ends
        atexit(SDL_Quit);

        renderer = new PGR_renderer(core);

        init(width, height, 24, 24, 8);

        mainLoop();

    }
    catch (exception & ex)
    {
        cout << "ERROR : " << ex.what() << endl;
        return EXIT_FAILURE;
    }

    delete renderer;
    return EXIT_SUCCESS;
}

void onMouseDown(Uint8 /*button*/, unsigned /*x*/, unsigned /*y*/)
{
}

void onMouseUp(Uint8 /*button*/, unsigned /*x*/, unsigned /*y*/)
{
}

void onKeyUp(SDLKey /*key*/, Uint16 /*mod*/)
{
}

/**
 * Callback for window change size event
 * @param w New window width
 * @param h New window height
 */
void onWindowResized(int w, int h)
{
    glViewport(0, 0, w, h);

    /* Reshaping is not allowed if radiosity renderer is used */
    if (!renderRadiosity)
    {
        /* Set new window size */
        width = w;
        height = h;
    }
}

/**
 * Callback for key press event
 *
 * @param key Pressed key
 */
void onKeyDown(SDLKey key, Uint16 /*mod*/)
{
    switch (key)
    {
    case SDLK_t:
        /* Change rendering core */
        renderRadiosity = !renderRadiosity;
        redraw();
        return;

    case SDLK_q:
    case SDLK_x:
    case SDLK_ESCAPE: quit();
        return;
    default: return;
    }
}

/**
 * Callback for mouse motion event
 *
 * @param xrel Move in direction of x-axis
 * @param yrel Move in direction of y-axis
 * @param buttons Mask of pressed buttons
 */
void onMouseMove(unsigned /*x*/, unsigned /*y*/, int xrel, int yrel, Uint8 buttons)
{
    /* Check if correct mouse button is pressed down */
    if (buttons & SDL_BUTTON_LMASK)
    {
        camera_rot_y += xrel; // compute new rotation
        camera_rot_x += yrel;
        redraw(); // force re-rendering
    }
    else if (buttons & SDL_BUTTON_RMASK)
    {
        camera_z += yrel / 10.0;
        redraw();
    }
}