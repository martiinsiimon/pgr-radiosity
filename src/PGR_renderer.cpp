/*
 * File:   PGR_renderer.cpp
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 2013-10-13, 18:20
 */

#include "PGR_renderer.h"

PGR_renderer::PGR_renderer()
{

}

PGR_renderer::PGR_renderer(string model)
{
    this->_modelFile = model;
    this->model = PGR_model(this->_modelFile);
    this->model.loadModel();
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
void PGR_renderer::drawSceneDefault()
{

    glBegin(GL_TRIANGLES);

    for (uint f = 0; f < this->model.triangles.size(); f++)
    {
        glNormal3f(this->model.normalsCoords[this->model.triangles[f].n1 - 1].x,
                   this->model.normalsCoords[this->model.triangles[f].n1 - 1].y,
                   this->model.normalsCoords[this->model.triangles[f].n1 - 1].z);

        glVertex3f(this->model.verticesCoords[this->model.triangles[f].v1 - 1].x,
                   this->model.verticesCoords[this->model.triangles[f].v1 - 1].y,
                   this->model.verticesCoords[this->model.triangles[f].v1 - 1].z);


        glNormal3f(this->model.normalsCoords[this->model.triangles[f].n2 - 1].x,
                   this->model.normalsCoords[this->model.triangles[f].n2 - 1].y,
                   this->model.normalsCoords[this->model.triangles[f].n2 - 1].z);

        glVertex3f(this->model.verticesCoords[this->model.triangles[f].v2 - 1].x,
                   this->model.verticesCoords[this->model.triangles[f].v2 - 1].y,
                   this->model.verticesCoords[this->model.triangles[f].v2 - 1].z);


        glNormal3f(this->model.normalsCoords[this->model.triangles[f].n3 - 1].x,
                   this->model.normalsCoords[this->model.triangles[f].n3 - 1].y,
                   this->model.normalsCoords[this->model.triangles[f].n3 - 1].z);

        glVertex3f(this->model.verticesCoords[this->model.triangles[f].v3 - 1].x,
                   this->model.verticesCoords[this->model.triangles[f].v3 - 1].y,
                   this->model.verticesCoords[this->model.triangles[f].v3 - 1].z);
    }

    glEnd();

    glutSwapBuffers();
}

/**
 * Draw 3D scene with radiosity computed using OpenCL. This rendering doesn't
 * run real-time and also changing of angle/resizing window is not permitted.
 */
void PGR_renderer::drawSceneRadiosity()
{
    // ###DBG###
    glBegin(GL_TRIANGLES);

    for (uint f = 0; f < this->model.triangles.size(); f++)
    {
        glNormal3f(this->model.normalsCoords[this->model.triangles[f].n1 - 1].x,
                   this->model.normalsCoords[this->model.triangles[f].n1 - 1].y,
                   this->model.normalsCoords[this->model.triangles[f].n1 - 1].z);

        glVertex3f(this->model.verticesCoords[this->model.triangles[f].v1 - 1].x,
                   this->model.verticesCoords[this->model.triangles[f].v1 - 1].y,
                   this->model.verticesCoords[this->model.triangles[f].v1 - 1].z);


        glNormal3f(this->model.normalsCoords[this->model.triangles[f].n2 - 1].x,
                   this->model.normalsCoords[this->model.triangles[f].n2 - 1].y,
                   this->model.normalsCoords[this->model.triangles[f].n2 - 1].z);

        glVertex3f(this->model.verticesCoords[this->model.triangles[f].v2 - 1].x,
                   this->model.verticesCoords[this->model.triangles[f].v2 - 1].y,
                   this->model.verticesCoords[this->model.triangles[f].v2 - 1].z);


        glNormal3f(this->model.normalsCoords[this->model.triangles[f].n3 - 1].x,
                   this->model.normalsCoords[this->model.triangles[f].n3 - 1].y,
                   this->model.normalsCoords[this->model.triangles[f].n3 - 1].z);

        glVertex3f(this->model.verticesCoords[this->model.triangles[f].v3 - 1].x,
                   this->model.verticesCoords[this->model.triangles[f].v3 - 1].y,
                   this->model.verticesCoords[this->model.triangles[f].v3 - 1].z);
    }

    glEnd();
    glutSwapBuffers();
    // ###DBG###
}