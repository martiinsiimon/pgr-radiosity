/*
 * File:   main.cpp
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 2013-10-11, 11:34
 */

#include <GL/glew.h>
#include <iostream>
#include <GL/glut.h>
#include "PGR_renderer.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace std;

/* Program constants - mostly default values */
#define PGR_WINDOW_WIDTH 1024 // Window width
#define PGR_WINDOW_HEIGHT 768 // Window height
#define PGR_CAMERA_Z 5.0f // Translation of camera in z-axis direction
#define PGR_CAMERA_Y 0.75f // Translation of camera in y-axis direction
#define PGR_CAMERA_X 0.0f // Translation of camera in x-axis direction
#define PGR_ROTATE_X 0.0f // Rotation about x-axis
#define PGR_ROTATE_Y 2 // Rotation about y-axis

/* Program global variables - initialized to default values*/
GLuint width = PGR_WINDOW_WIDTH;
GLuint height = PGR_WINDOW_HEIGHT;
GLfloat camera_rot_y = PGR_ROTATE_Y; // Rotation about y-axis
GLfloat camera_rot_x = PGR_ROTATE_X;
bool renderRadiosity = false;

/* Mouse handeling variables */
int old_mouse_x = 0;
int old_mouse_y = 0;
int mouse_state = 0;

/* Model geometry */
GLuint roomVBO, roomEBO;
GLuint winBackVBO, winBackEBO;
GLuint winRightVBO, winRightEBO;
GLuint topLightVBO, topLightEBO;
GLuint SphereVBO, SphereEBO;


/* Shaders */
GLuint iVS, iFS, iProg;
GLuint positionAttrib, colorAttrib, mvpUniform;

const char * vertexShaderRoom
    = "#version 130\n in vec3 position; in vec4 color; uniform mat4 mvp; out vec4 c; void main() { gl_Position = mvp*vec4(position,1); c = color;}";
const char * fragmentShaderRoom
    = "#version 130\n in vec4 c; out vec4 fragColor; void main() { fragColor = c; }";


GLuint winBackVS, winBackFS, winBackProg;
GLuint SpositionAttrib, SnormalAttrib, ScolorAttrib, SmvpUniform, SlightvecUniform;
const char * vertexShaderWinBack
    = "#version 130\n in vec3 position; in vec3 normal; in vec3 color; uniform mat4 mvp; uniform vec3 lightvec; out float intensity; out vec4 c; void main() { gl_Position = mvp*vec4(position,1); intensity = 0.3 + 0.7*max(0, dot(normal, lightvec)); c = vec4(color, 1); }";
const char * fragmentShaderWinBack
    = "#version 130\n in float intensity; in vec4 c; out vec4 fragColor; void main() { fragColor = vec4(intensity) * c; }";

/* Renderer */
PGR_renderer renderer;

/**
 * Callback for rendering display. Default rotation and translation is done and
 * correct renderer is called.
 */
void onDisplay()
{
    glm::mat4 projection;

    projection = glm::perspective(75.0f, (float) width / (float) height, 0.1f, 200.0f);

    glm::mat4 mvp = glm::rotate(
                                glm::rotate(
                                            glm::translate(
                                                           projection,
                                                           glm::vec3(PGR_CAMERA_X, -PGR_CAMERA_Y, -PGR_CAMERA_Z)
                                                           ),
                                            camera_rot_x, glm::vec3(1, 0, 0)
                                            ),
                                camera_rot_y, glm::vec3(0, 1, 0)
                                );


    /* Choose renderer */
    if (!renderRadiosity)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        renderer.drawSceneDefault(mvp);

        glutSwapBuffers();
    }
    else
    {
        //TODO here should be radiosity rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        renderer.drawSceneRadiosity(mvp);

        glutSwapBuffers();
    }
}

/**
 * Callback for window change size event
 * @param w New window width
 * @param h New window height
 */
void onReshape(int w, int h)
{
    /* Reshaping is not allowed if radiosity renderer is used */
    if (!renderRadiosity)
    {
        /* Set new window size */
        width = w;
        height = h;
    }

    /* Re-compute projection matrix */
    glViewport(0, 0, width, height);
}

/**
 * Callback for key press event
 * @param key The key which is pressed
 * @param x Cursor x-coordinate in moment of key pressing
 * @param y Cursor y-coordinate in moment of key pressing
 */
void onKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 't':
        /* Change rendering core */
        renderRadiosity = !renderRadiosity;
        break;

    case 27:
    case 'q':
    case 'x':
        /* Quit program */
        exit(0);
        break;

    default:
        break;
    }

    /* Call re-rendering */
    glutPostRedisplay();
}


/**
 * Callback registering mouse click events. We need to register which button
 * causes the event.
 * @license Taken from IZG2011/12, FIT BUT
 * @param button The button which causes mouse click event
 * @param state Wheter the click means "press" or "release"
 * @param x Coordinate in x-axis direction
 * @param y Coordinate in y-axis direction
 */
void onMouseClick(int button, int _state, int x, int y)
{
    /* Remember position */
    old_mouse_x = x;
    old_mouse_y = y;

    /* Do not permit scene rotation when radiosity rendering */
    if (renderRadiosity)
        return;

    /* Check if the left key is pressed down or not */
    if (button == GLUT_LEFT_BUTTON && _state == GLUT_DOWN)
    {
        mouse_state = 1;
    }
    else
    {
        mouse_state = 0;
    }
}

/**
 * Callback for mouse motion when key pressed
 * @license Taken from IZG2011/12, FIT BUT
 * @param x Coordinate in x-axis direction
 * @param y Coordinate in y-axis diriection
 */
void onMouseMotion(int x, int y)
{
    /* Do not permit scene rotation when radiosity rendering */
    if (renderRadiosity)
        return;

    /* Check if correct mouse button is pressed down */
    if (mouse_state == 1)
    {
        camera_rot_y += x - old_mouse_x; // compute new rotation
        camera_rot_x += y - old_mouse_y;
        glutPostRedisplay(); // force re-rendering
    }
    else
    {

    }
    /* Remember position */
    old_mouse_x = x;
    old_mouse_y = y;
}

/**
 * Main function. Event handlers are registered and scene and renderer are
 * initialized
 */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitWindowSize(width, height);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("gi04 - Radiosita na OpenCL");

    // GL init
    glewInit();
    if (!glewIsSupported("GL_VERSION_2_0 " "GL_ARB_pixel_buffer_object"))
    {
        std::cerr << "Support for necessary OpenGL extensions missing."
            << std::endl;
        return EXIT_FAILURE;
    }

    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(onKeyboard);
    glutMouseFunc(onMouseClick);
    glutMotionFunc(onMouseMotion);

    renderer = PGR_renderer();

    renderer.initialize();
    glutMainLoop();
    return 0;
}