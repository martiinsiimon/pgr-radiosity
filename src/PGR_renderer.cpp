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
    this->model = new PGR_model();
    this->maxArea = -1.0;
    this->divided = true;
}

PGR_renderer::PGR_renderer(const PGR_renderer& orig)
{
}

PGR_renderer::~PGR_renderer()
{
    delete this->model;
}

void PGR_renderer::setMaxArea(float area)
{
    this->maxArea = area;
    this->divided = false;
}

void PGR_renderer::init()
{
    /* Create shaders */
    iVS = compileShader(GL_VERTEX_SHADER, vertexShaderRoom);
    iFS = compileShader(GL_FRAGMENT_SHADER, fragmentShaderRoom);
    iProg = linkShader(2, iVS, iFS);

    /* Link shader input/output to gl variables */
    positionAttrib = glGetAttribLocation(iProg, "position");
    normalAttrib = glGetAttribLocation(iProg, "normal");
    colorAttrib = glGetAttribLocation(iProg, "color");
    mvpUniform = glGetUniformLocation(iProg, "mvp");
    laUniform = glGetUniformLocation(iProg, "la");
    ldUniform = glGetUniformLocation(iProg, "ld");
    lightPosUniform = glGetUniformLocation(iProg, "lightPos");

    this->divide();

    this->createBuffers();
}

bool PGR_renderer::divide()
{
    if (!this->divided && this->maxArea > 0.0)
    {
        /* Max area set, need to divide all patches */
        this->model->setMaxArea(this->maxArea);
        cout << "Patches before dividing:" << this->model->patches.size() << endl;
        this->model->divide();
        this->model->updateArrays();
        cout << "Patches after dividing:" << this->model->patches.size() << endl;
        this->divided = true;
        return true;
    }
    else
        return false;
}

void PGR_renderer::printPatches()
{
    for (int i = 0; i <this->model->patches.size(); i++)
    {
        cout << "Patch: " << i << endl;
        cout << "\t[" << this->model->patches[i]->vertices[0].position[0] << "," << this->model->patches[i]->vertices[0].position[1] << "," << this->model->patches[i]->vertices[0].position[2] << "]" << endl;
        cout << "\t[" << this->model->patches[i]->vertices[1].position[0] << "," << this->model->patches[i]->vertices[1].position[1] << "," << this->model->patches[i]->vertices[1].position[2] << "]" << endl;
        cout << "\t[" << this->model->patches[i]->vertices[2].position[0] << "," << this->model->patches[i]->vertices[2].position[1] << "," << this->model->patches[i]->vertices[2].position[2] << "]" << endl;
        cout << "\t[" << this->model->patches[i]->vertices[3].position[0] << "," << this->model->patches[i]->vertices[3].position[1] << "," << this->model->patches[i]->vertices[3].position[2] << "]" << endl;
    }
}

void PGR_renderer::createBuffers()
{
    /* Create buffers */
    glGenBuffers(1, &roomVBO);
    glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
    glBufferData(GL_ARRAY_BUFFER, this->model->getVerticesCount() * sizeof (Point), this->model->getVertices(), GL_STATIC_DRAW);

    glGenBuffers(1, &roomEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->model->getIndicesCount() * sizeof (unsigned int), this->model->getIndices(), GL_STATIC_DRAW);
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


    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomEBO);
    glDrawElements(GL_QUADS, this->model->getIndicesCount(), GL_UNSIGNED_INT, NULL);

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
    /* If there's something to divide */
    if (this->divide())
        this->createBuffers();

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

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roomEBO);
    glDrawElements(GL_QUADS, this->model->getIndicesCount(), GL_UNSIGNED_INT, NULL);

    glDisableVertexAttribArray(positionAttrib);
    glDisableVertexAttribArray(colorAttrib);
    // ###DBG###
}

/*
 * Computation form factor
 * @param glm::vec3 RecvPos - world-space position of this element
 * @param glm::vec3 ShootPos - world-space position of shooter
 * @param glm::vec3 RecvNormal - world-space normal of this element
 * @param glm::vec3 ShootNormal - world-space normal of shooter
 * @param glm::vec3 ShooterEnergy - energy from shooter residual texture
 * @param float ShootDArea - the delta area of the shooter
 * @param glm::vec3 RecvColor - the reflectivity of this element
 */
glm::vec3 PGR_renderer::formFactor(glm::vec3 RecvPos, glm::vec3 ShootPos, glm::vec3 RecvNormal, glm::vec3 ShootNormal, glm::vec3 ShooterEnergy, float ShootDArea, glm::vec3 RecvColor)
{
  // a normalized vector from shooter to receiver
  glm::vec3 r = ShootPos - RecvPos;
  float distance2 = glm::dot(r, r);
  r = glm::normalize(r);

  // the angles of the receiver and the shooter from r
  float cosi = glm::dot(RecvNormal, r);
  float cosj = -glm::dot(ShootNormal, r);

  // compute the disc approximation form factor
  float Fij = max(cosi * cosj, (float)0) / (M_PI * distance2 + ShootDArea);

  // Modulate shooter's energy by the receiver's reflectivity
  // and the area of the shooter.
  glm::vec3 delta = ShooterEnergy * RecvColor * ShootDArea * Fij;

  return delta;     
}