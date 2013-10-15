/*
 * File:   main.cpp
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 2013-10-11, 11:34
 */

#include <iostream>
#include <GL/glut.h>
#include "PGR_renderer.h"

using namespace std;

/* Program constants - mostly default values */
#define PGR_WINDOW_WIDTH 1024 // Window width
#define PGR_WINDOW_HEIGHT 768 // Window height
#define PGR_CAMERA_Z 3500.0f // Translation of camera in z-axis direction
#define PGR_CAMERA_Y 2000.0f // Translation of camera in y-axis direction
#define PGR_CAMERA_X 0.0f // Translation of camera in x-axis direction
#define PGR_ROTATE_X 10 // Rotation about x-axis
#define PGR_ROTATE_Y 5 // Rotation about y-axis
#define PGR_MODEL "model.obj" // Model in WaweFront (*.obj) format with triangles

/* Program global variables - initialized to default values*/
GLuint width = PGR_WINDOW_WIDTH;
GLuint height = PGR_WINDOW_HEIGHT;
GLint camera_rot_y = PGR_ROTATE_Y; // Rotation about y-axis
bool renderRadiosity = false;

/* Mouse handeling variables */
int old_mouse_x = 0;
int mouse_state = 0;

/* Renderer */
PGR_renderer renderer;


/* Function declarations */
void initialize();

/**
 * Callback for rendering display. Default rotation and translation is done and
 * correct renderer is called.
 */
void onDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* Translate scene */
    glTranslatef(-PGR_CAMERA_X, -PGR_CAMERA_Y, -PGR_CAMERA_Z);

    /* Rotate scene */
    glRotated(PGR_ROTATE_X, 1.0, 0.0, 0.0);
    glRotated(camera_rot_y, 0.0, 1.0, 0.0);

    /* Choose renderer */
    if (renderRadiosity)
    {
        renderer.drawSceneRadiosity();
    }
    else
    {
        renderer.drawSceneDefault();
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

        /* Re-compute projection matrix */
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, width, height);
        gluPerspective(100, (GLfloat) width / (GLfloat) height, 1.0, 100000.0);
        glMatrixMode(GL_MODELVIEW);
    }
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
        initialize();
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
 * Initialization of scene
 */
void initialize()
{
    glEnable(GL_DEPTH_TEST); // enable depth testing
    glClearColor(1.0, 1.0, 1.0, 1.0); //set background color

    /* Initialization depends on chosen renderer */
    if (renderRadiosity)
    {
        // ### DBG ###
        glShadeModel(GL_SMOOTH);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        // ### DBG ###
    }
    else
    {
        glShadeModel(GL_FLAT);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }
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

    /* Force re-rendering */
    glutPostRedisplay();
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
        glutPostRedisplay(); // force re-rendering
    }

    /* Remember position */
    old_mouse_x = x;
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
    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(onKeyboard);
    glutMouseFunc(onMouseClick);
    glutMotionFunc(onMouseMotion);

    renderer = PGR_renderer(PGR_MODEL);

    initialize();
    glutMainLoop();
    return 0;
}