/*
 * File:   PGR_renderer.cpp
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 2013-10-13, 18:20
 */

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include "PGR_renderer.h"
#include "PGR_model.h"
#include "sphere.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* Buffers */
extern GLuint roomVBO, roomEBO;
extern GLuint winBackVBO, winBackEBO;
extern GLuint winRightVBO, winRightEBO;
extern GLuint topLightVBO, topLightEBO;
extern GLuint SphereVBO, SphereEBO;

/* Shaders */
extern GLuint iVS, iFS, iProg;
extern GLuint positionAttrib, colorAttrib, mvpUniform;

extern const char * vertexShaderRoom;
extern const char * fragmentShaderRoom;

extern GLuint winBackVS, winBackFS, winBackProg;
extern GLuint SpositionAttrib, SnormalAttrib, ScolorAttrib, SmvpUniform, SlightvecUniform;
extern const char * vertexShaderWinBack;
extern const char * fragmentShaderWinBack;


PGR_renderer::PGR_renderer()
{

}

PGR_renderer::PGR_renderer(const PGR_renderer& orig)
{
}

PGR_renderer::~PGR_renderer()
{
}

void PGR_renderer::initialize()
{
    /* Create shaders */
    iVS = glCreateShader(GL_VERTEX_SHADER);
    iFS = glCreateShader(GL_FRAGMENT_SHADER);
    winBackVS = glCreateShader(GL_VERTEX_SHADER);
    winBackFS = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(iVS, 1, &vertexShaderRoom, NULL);
    glShaderSource(iFS, 1, &fragmentShaderRoom, NULL);
    glShaderSource(winBackVS, 1, &vertexShaderWinBack, NULL);
    glShaderSource(winBackFS, 1, &fragmentShaderWinBack, NULL);

    glCompileShader(iVS);
    glCompileShader(iFS);
    glCompileShader(winBackVS);
    glCompileShader(winBackFS);

    iProg = glCreateProgram();
    glAttachShader(iProg, iVS);
    glAttachShader(iProg, iFS);
    glLinkProgram(iProg);

    winBackProg = glCreateProgram();
    glAttachShader(winBackProg, winBackVS);
    glAttachShader(winBackProg, winBackFS);
    glLinkProgram(winBackProg);

    //TODO mozna odchytit navratove hodnoty

    /* Link shader input/output to gl variables */
    positionAttrib = glGetAttribLocation(iProg, "position");
    colorAttrib = glGetAttribLocation(iProg, "color");
    mvpUniform = glGetUniformLocation(iProg, "mvp");

    SpositionAttrib = glGetAttribLocation(winBackProg, "position");
    SnormalAttrib = glGetAttribLocation(winBackProg, "normal");
    ScolorAttrib = glGetAttribLocation(winBackProg, "color");
    SmvpUniform = glGetUniformLocation(winBackProg, "mvp");
    SlightvecUniform = glGetUniformLocation(winBackProg, "lightvec");

    /* Create buffers */
    glGenBuffers(1, &roomVBO);
    glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof (roomVertices), roomVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &roomEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (room), room, GL_STATIC_DRAW);

    /* Create buffers for back window */
    glGenBuffers(1, &winBackVBO);
    glBindBuffer(GL_ARRAY_BUFFER, winBackVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof (winBackVertices), winBackVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &winBackEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, winBackEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (winBack), winBack, GL_STATIC_DRAW);

    /* Create buffers for right window */
    glGenBuffers(1, &winRightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, winRightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof (winRightVertices), winRightVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &winRightEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, winRightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (winRight), winRight, GL_STATIC_DRAW);

    /* Create buffers for top light */
    glGenBuffers(1, &topLightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, topLightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof (topLightVertices), topLightVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &topLightEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topLightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (topLight), topLight, GL_STATIC_DRAW);

    /* Create buffers for spheres */
    glGenBuffers(1, &SphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, SphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof (sphereVertices), sphereVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &SphereEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (sphere), sphere, GL_STATIC_DRAW);

    glClearColor(1.0, 1.0, 1.0, 1.0);
}

/**
 * Draw 3D scene using default renderer. This rendering should be real-time and
 * used to set view angle light sources. It doesn't need to run on GPU.
 */
void PGR_renderer::drawSceneDefault(glm::mat4 mvp)
{

    /* Draw lights */
    glUseProgram(iProg);
    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));

    /* Back window */
    glBindBuffer(GL_ARRAY_BUFFER, winBackVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, winBackEBO);
    glDrawElements(GL_TRIANGLES, sizeof (winBack) / sizeof (*winBack), GL_UNSIGNED_BYTE, NULL);

    /* Right window*/
    glBindBuffer(GL_ARRAY_BUFFER, winRightVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, winRightEBO);
    glDrawElements(GL_TRIANGLES, sizeof (winRight) / sizeof (*winRight), GL_UNSIGNED_BYTE, NULL);

    /* Top light */
    glBindBuffer(GL_ARRAY_BUFFER, topLightVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topLightEBO);
    glDrawElements(GL_TRIANGLES, sizeof (topLight) / sizeof (*topLight), GL_UNSIGNED_BYTE, NULL);

    glDisableVertexAttribArray(positionAttrib);
    glDisableVertexAttribArray(colorAttrib);


    /* Draw room */
    /* Note: Here should be the second shader program, object to bottom from here should react to light */
    glUseProgram(iProg);
    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, color));
    //glVertexAttribPointer(SnormalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, normal));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomEBO);
    glDrawElements(GL_TRIANGLES, sizeof (room) / sizeof (*room), GL_UNSIGNED_BYTE, NULL);


    /* Draw spheres */
    glBindBuffer(GL_ARRAY_BUFFER, SphereVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (SphereVertex), (void*) offsetof(SphereVertex, position));
    //glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (SphereVertex), (void*) offsetof(SphereVertex, normal));

    // Sphere 1
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(glm::scale(glm::translate(mvp, glm::vec3(3.0, -0.9, -2.0)), glm::vec3(0.6))));
    //glUniform3fv(SlightvecUniform, 1, glm::value_ptr(glm::normalize(glm::vec3(0.0f, 0.4f, -3.0f) - glm::vec3(3.0, -1.0, -2.0))));
    glDrawElements(GL_TRIANGLES, sizeof (sphere) / sizeof (**sphere), sphereIndexType, NULL);

    glDisableVertexAttribArray(positionAttrib);
    glDisableVertexAttribArray(colorAttrib);

}

/**
 * Draw 3D scene with radiosity computed using OpenCL. This rendering doesn't
 * run real-time and also changing of angle/resizing window is not permitted.
 */
void PGR_renderer::drawSceneRadiosity(glm::mat4 mvp)
{
    // ###DBG###
    /* Draw lights */
    glUseProgram(iProg);
    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));

    /* Back window */
    glBindBuffer(GL_ARRAY_BUFFER, winBackVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, winBackEBO);
    glDrawElements(GL_TRIANGLES, sizeof (winBack) / sizeof (*winBack), GL_UNSIGNED_BYTE, NULL);

    /* Right window*/
    glBindBuffer(GL_ARRAY_BUFFER, winRightVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, winRightEBO);
    glDrawElements(GL_TRIANGLES, sizeof (winRight) / sizeof (*winRight), GL_UNSIGNED_BYTE, NULL);

    /* Top light */
    glBindBuffer(GL_ARRAY_BUFFER, topLightVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topLightEBO);
    glDrawElements(GL_TRIANGLES, sizeof (topLight) / sizeof (*topLight), GL_UNSIGNED_BYTE, NULL);

    glDisableVertexAttribArray(positionAttrib);
    glDisableVertexAttribArray(colorAttrib);


    /* Draw room */
    /* Note: Here should be the second shader program, object to bottom from here should react to light */
    glUseProgram(iProg);
    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, color));
    //glVertexAttribPointer(SnormalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, normal));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomEBO);
    glDrawElements(GL_TRIANGLES, sizeof (room) / sizeof (*room), GL_UNSIGNED_BYTE, NULL);


    /* Draw spheres */
    glBindBuffer(GL_ARRAY_BUFFER, SphereVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (SphereVertex), (void*) offsetof(SphereVertex, position));
    //glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (SphereVertex), (void*) offsetof(SphereVertex, normal));

    // Sphere 1
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(glm::scale(glm::translate(mvp, glm::vec3(3.0, -0.9, -2.0)), glm::vec3(0.6))));
    //glUniform3fv(SlightvecUniform, 1, glm::value_ptr(glm::normalize(glm::vec3(0.0f, 0.4f, -3.0f) - glm::vec3(3.0, -1.0, -2.0))));
    glDrawElements(GL_TRIANGLES, sizeof (sphere) / sizeof (**sphere), sphereIndexType, NULL);

    glDisableVertexAttribArray(positionAttrib);
    glDisableVertexAttribArray(colorAttrib);
    // ###DBG###
}