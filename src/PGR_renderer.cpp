/*
 * File:   PGR_renderer.cpp
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 2013-10-13, 18:20
 */

#include "pgr.h"

#include "PGR_renderer.h"
#include "model.h"
#include "sphere.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* Buffers */
extern GLuint roomVBO, roomEBO;

/* Shaders */
extern GLuint iVS, iFS, iProg;
extern GLuint positionAttrib, colorAttrib, normalAttrib, mvpUniform, laUniform, ldUniform, lightPosUniform;

extern const char * vertexShaderRoom;
extern const char * fragmentShaderRoom;

glm::vec3 light_pos = glm::vec3(0, 2.5, 0);

PGR_renderer::PGR_renderer()
{

}

PGR_renderer::PGR_renderer(const PGR_renderer& orig)
{
}

PGR_renderer::~PGR_renderer()
{
}


/**
 * Draw 3D scene using default renderer. This rendering should be real-time and
 * used to set view angle light sources. It doesn't need to run on GPU.
 */
void PGR_renderer::drawSceneDefault(glm::mat4 mvp)
{

    glUseProgram(iProg);

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3f(laUniform, 0.2, 0.2, 0.2);
    glUniform3f(lightPosUniform, light_pos[0], light_pos[1], light_pos[2]);
    glUniform3f(ldUniform, 0.5, 0.5, 0.5);


    /* Draw room */
    /* Note: Here should be the second shader program, object to bottom from here should react to light */
    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);
    glEnableVertexAttribArray(normalAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, color));
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, normal));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomEBO);
    glDrawElements(GL_QUADS, sizeof (room) / sizeof (*room), GL_UNSIGNED_BYTE, NULL);

    glDisableVertexAttribArray(positionAttrib);
    glDisableVertexAttribArray(colorAttrib);
    glDisableVertexAttribArray(normalAttrib);


    /* Draw objects */
}

/**
 * Draw 3D scene with radiosity computed using OpenCL. This rendering doesn't
 * run real-time and also changing of angle/resizing window is not permitted.
 */
void PGR_renderer::drawSceneRadiosity(glm::mat4 mvp)
{
    // ###DBG###
    glUseProgram(iProg);

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));


    /* Draw room */
    /* Note: Here should be the second shader program, object to bottom from here should react to light */
    //glUseProgram(iProg);
    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof (Point), (void*) offsetof(Point, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomEBO);
    glDrawElements(GL_QUADS, sizeof (room) / sizeof (*room), GL_UNSIGNED_BYTE, NULL);

    glDisableVertexAttribArray(positionAttrib);
    glDisableVertexAttribArray(colorAttrib);
    // ###DBG###
}