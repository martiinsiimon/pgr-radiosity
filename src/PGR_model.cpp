/*
 * File:   PGR_model.cpp
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 16. listopad 2013, 17:12
 */
#include "PGR_model.h"
//#include <iostream>
#include <cstring>
#include <list>
#include <GL/glu.h>


PGR_model::PGR_model()
{

}

PGR_model::PGR_model(int t)
{
    if (t == C_ROOM)
    {
        this->indices = new unsigned int[(sizeof (room)) / sizeof (*room)];
        memcpy(this->indices, room, sizeof (room));
        this->indicesCount = sizeof (room) / sizeof (*room);

        this->vertices = new Point[(sizeof (roomVertices)) / sizeof (*roomVertices)];
        memcpy(this->vertices, roomVertices, sizeof (roomVertices));
        this->verticesCount = sizeof (roomVertices) / sizeof (*roomVertices);
    }
    else if (t == C_LIGHT)
    {
        this->indices = new unsigned int[(sizeof (lightIn)) / sizeof (*lightIn)];
        memcpy(this->indices, lightIn, sizeof (lightIn));
        this->indicesCount = sizeof (lightIn) / sizeof (*lightIn);

        this->vertices = new Point[(sizeof (lightVe)) / sizeof (*lightVe)];
        memcpy(this->vertices, lightVe, sizeof (lightVe));
        this->verticesCount = sizeof (lightVe) / sizeof (*lightVe);
    }
    else
    {
        this->indices = NULL;
        this->vertices = NULL;

        this->indicesCount = 0;
        this->verticesCount = 0;
    }

    this->maxArea = -1.0;
    this->updatePatches();

    if (t == C_LIGHT)
    {
        this->addLightEnergy(C_LIGHT_ENERGY);
    }
}

PGR_model::PGR_model(const PGR_model& orig)
{
}

PGR_model::~PGR_model()
{
    delete [] this->indices;
    delete [] this->vertices;

    this->deletePatches();
}

void PGR_model::appendModel(PGR_model* m)
{
    for (int i = 0; i < m->patches.size(); i++)
    {
        PGR_patch * p = new PGR_patch();
        p->setVertices(m->patches[i]->vertices[i],
                       m->patches[i]->vertices[i + 1],
                       m->patches[i]->vertices[i + 2],
                       m->patches[i]->vertices[i + 3]);
        p->setEnergy(m->patches[i]->energy);
        this->patches.push_back(p);
    }
    this->updateArrays();
}

void PGR_model::addLightEnergy(double e)
{
    for (int i = 0; i < this->patches.size(); i++)
    {
        this->patches.at(i)->setEnergy(e);
    }
}

unsigned int* PGR_model::getIndices()
{
    return this->indices;
}

unsigned int PGR_model::getIndicesCount()
{
    return this->indicesCount;
}

float* PGR_model::getVertices()
{
    return (float *) this->vertices;
}

unsigned int PGR_model::getVerticesCount()
{
    return this->verticesCount;
}

void PGR_model::setMaxArea(float area)
{
    this->maxArea = area;
}

void PGR_model::updateArrays()
{
    delete [] this->indices;
    delete [] this->vertices;

    this->indices = new unsigned int[this->patches.size() * 4];
    this->vertices = new Point[this->patches.size() * 4];

    this->indicesCount = this->patches.size() * 4;
    this->verticesCount = this->patches.size() * 4;

    for (int n = 0; n < this->patches.size(); n++)
    {
        this->indices[n * 4 + 0] = n * 4 + 0;
        this->indices[n * 4 + 1] = n * 4 + 1;
        this->indices[n * 4 + 2] = n * 4 + 2;
        this->indices[n * 4 + 3] = n * 4 + 3;

        this->vertices[n * 4 + 0] = this->patches.at(n)->vertices[0];
        this->vertices[n * 4 + 1] = this->patches.at(n)->vertices[1];
        this->vertices[n * 4 + 2] = this->patches.at(n)->vertices[2];
        this->vertices[n * 4 + 3] = this->patches.at(n)->vertices[3];
    }
}

void PGR_model::updatePatches()
{
    this->deletePatches();

    for (int i = 0; i < this->indicesCount; i += 4)
    {
        PGR_patch * p = new PGR_patch();
        p->setVertices(this->vertices[i],
                       this->vertices[i + 1],
                       this->vertices[i + 2],
                       this->vertices[i + 3]);
        p->setEnergy(0);
        this->patches.push_back(p);
    }
}

void PGR_model::deletePatches()
{
    for (int i = 0; i < this->patches.size(); i++)
    {
        delete this->patches.at(i);
    }
    this->patches.clear();
}

int PGR_model::getPatchesCount()
{
    return this->patches.size();
}

void PGR_model::divide()
{
    if (this->maxArea <= 0.0)
        return;

    vector<PGR_patch*> tmpPatches;

    for (int i = 0; i < this->patches.size(); i++)
    {
        if (this->patches[i]->area > this->maxArea)
        {
            vector<PGR_patch*> vec;
            this->patches[i]->divide(this->maxArea, &vec);

            for (int n = 0; n < vec.size(); n++)
            {
                PGR_patch * np = new PGR_patch();
                np->setVertices(vec[n]->vertices[0],
                                vec[n]->vertices[1],
                                vec[n]->vertices[2],
                                vec[n]->vertices[3]);
                np->setEnergy(vec[n]->getEnergy());
                tmpPatches.push_back(np);
                delete vec[n];
            }
            vec.clear();
        }
        else
        {
            PGR_patch * np = new PGR_patch();
            np->setVertices(this->patches[i]->vertices[0],
                            this->patches[i]->vertices[1],
                            this->patches[i]->vertices[2],
                            this->patches[i]->vertices[3]);
            np->setEnergy(this->patches[i]->getEnergy());
            tmpPatches.push_back(np);
        }
    }

    this->deletePatches();

    this->patches = tmpPatches;
}

int PGR_model::getPatchesCL(cl_float4 *data, cl_double *energies)
{
    int i;
    for (i = 0; i < this->patches.size(); i++)
    {
        //energy
        energies[i] = this->patches.at(i)->energy;

        //color, all vertices are the same
        data[i].s0 = this->patches.at(i)->vertices[0].color[0];
        data[i].s1 = this->patches.at(i)->vertices[0].color[1];
        data[i].s2 = this->patches.at(i)->vertices[0].color[2];
    }

    return i;
}

int PGR_model::getPatchesGeometryCL(cl_float16 *data)
{
    int i;
    for (i = 0; i < this->patches.size(); i++)
    {
        //vertex 1
        data[i].s[0] = this->patches.at(i)->vertices[0].position[0];
        data[i].s[1] = this->patches.at(i)->vertices[0].position[1];
        data[i].s[2] = this->patches.at(i)->vertices[0].position[2];

        //vertex 2
        data[i].s[3] = this->patches.at(i)->vertices[1].position[0];
        data[i].s[4] = this->patches.at(i)->vertices[1].position[1];
        data[i].s[5] = this->patches.at(i)->vertices[1].position[2];

        //vertex 3
        data[i].s[6] = this->patches.at(i)->vertices[2].position[0];
        data[i].s[7] = this->patches.at(i)->vertices[2].position[1];
        data[i].s[8] = this->patches.at(i)->vertices[2].position[2];

        //vertex 4
        data[i].s[9] = this->patches.at(i)->vertices[3].position[0];
        data[i].s[10] = this->patches.at(i)->vertices[3].position[1];
        data[i].s[11] = this->patches.at(i)->vertices[3].position[2];

        //normal of patch - the same for all vertices
        data[i].s[12] = this->patches.at(i)->vertices[0].normal[0];
        data[i].s[13] = this->patches.at(i)->vertices[0].normal[1];
        data[i].s[14] = this->patches.at(i)->vertices[0].normal[2];

        //area of patch
        data[i].s[15] = this->patches.at(i)->area;
    }
    return i;
}

cl_uint PGR_model::getIdsOfNMostEnergizedPatchesCL(cl_uint *indices, int n, double limit)
{
    vector<uint> ids;
    int count = this->getIdsOfNMostEnergizedPatches(&ids, n, limit);

    for (int i = 0; i < count; i++)
    {
        indices[i] = ids.at(i);
    }

    return (cl_uint) count;
}

int PGR_model::getIdsOfNMostEnergizedPatches(vector<uint> *ids, int n, double limit)
{
    int count = 0; //real count
    uint pos = 0; //position of maximal energy
    uint maxPos = this->patches.size(); //sentinel
    double max = 0.0; //maximal energy
    double lastMax = 10000000.0; //last maximal energy
    double lastMaxOld = lastMax;


    for (int i = 0; i < n; i++)
    {
        max = 0.0;
        int j;
        for (j = 0; j < maxPos; j++)
        {
            if (this->patches.at(j)->energy >= max && this->patches.at(j)->energy < lastMax)
            {
                pos = j;
                max = this->patches.at(j)->energy;
            }
        }

        if (max == 0.0 || max < limit)
        {
            if (j == this->patches.size() - 1)
            {
                break;
            }

            lastMax = lastMaxOld;
            maxPos = this->patches.size();
            continue;
        }

        ids->push_back(pos);
        count++;
        maxPos = pos;
        lastMaxOld = max;
    }
    return count;
}

double PGR_model::getMaximalEnergy()
{
    double energy = 0;
    for(int i = 0; i < this->patches.size(); i++)
    {
        if(this->patches[i]->energy > energy)
        {
            energy = this->patches[i]->energy;
        }
    }

    return energy;
}

void PGR_model::decodePatchesCL(cl_float4 *data, cl_double *energies, uint size)
{
    if (size != this->getPatchesCount())
    {
        cout << "Pole nejsou stejne velka!!!" << endl;
        return;
    }

    for (int i = 0; i < size; i++)
    {
        this->patches[i]->setEnergy(energies[i]);

        this->patches[i]->vertices[0].color[0] = data[i].s[0];
        this->patches[i]->vertices[1].color[0] = data[i].s[0];
        this->patches[i]->vertices[2].color[0] = data[i].s[0];
        this->patches[i]->vertices[3].color[0] = data[i].s[0];

        this->patches[i]->vertices[0].color[1] = data[i].s[1];
        this->patches[i]->vertices[1].color[1] = data[i].s[1];
        this->patches[i]->vertices[2].color[1] = data[i].s[1];
        this->patches[i]->vertices[3].color[1] = data[i].s[1];

        this->patches[i]->vertices[0].color[2] = data[i].s[2];
        this->patches[i]->vertices[1].color[2] = data[i].s[2];
        this->patches[i]->vertices[2].color[2] = data[i].s[2];
        this->patches[i]->vertices[3].color[2] = data[i].s[2];
    }
}

void PGR_model::decodePatchesGeometryCL(cl_float16* data, uint size)
{
    if (size != this->getPatchesCount())
    {
        cout << "Pole nejsou stejne velka!!!" << endl;
        return;
    }

    for (int i = 0; i < size; i++)
    {
        //vertex 1
        this->patches[i]->vertices[0].position[0] = data[i].s[0];
        this->patches[i]->vertices[0].position[1] = data[i].s[1];
        this->patches[i]->vertices[0].position[2] = data[i].s[2];

        //vertex 2
        this->patches[i]->vertices[1].position[0] = data[i].s[3];
        this->patches[i]->vertices[1].position[1] = data[i].s[4];
        this->patches[i]->vertices[1].position[2] = data[i].s[5];

        //vertex 3
        this->patches[i]->vertices[2].position[0] = data[i].s[6];
        this->patches[i]->vertices[2].position[1] = data[i].s[7];
        this->patches[i]->vertices[2].position[2] = data[i].s[8];

        //vertex 4
        this->patches[i]->vertices[3].position[0] = data[i].s[9];
        this->patches[i]->vertices[3].position[1] = data[i].s[10];
        this->patches[i]->vertices[3].position[2] = data[i].s[11];

        //normal of patch - the same for all vertices
        this->patches[i]->vertices[0].normal[0] = data[i].s[12];
        this->patches[i]->vertices[0].normal[1] = data[i].s[13];
        this->patches[i]->vertices[0].normal[2] = data[i].s[14];

        //area of patch
        this->patches[i]->area = data[i].s[15];
    }
}

void PGR_model::getViewFromPatch(int i, cl_float3 *texFront, cl_float3 *texTop, cl_float3 *texBottom, cl_float3 *texLeft, cl_float3 *texRight)
{
    PGR_patch * p = this->patches[i];


    float eye[] = {0.0f, 0.0f, 0.0f}; //TODO
    float center[] = {
        (p->vertices[0].position[0] + p->vertices[1].position[0] + p->vertices[2].position[0] + p->vertices[3].position[0]) / 4,
        (p->vertices[0].position[1] + p->vertices[1].position[1] + p->vertices[2].position[1] + p->vertices[3].position[1]) / 4,
        (p->vertices[0].position[2] + p->vertices[1].position[2] + p->vertices[2].position[2] + p->vertices[3].position[2]) / 4
    };
    float up[3]; //TODO compute the UP vector
    //determine the center of the patch

    cl_float3 * screen = new cl_float3[256 * 256]; //always read the square

    /* Front view */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //is this correct?
    glViewport(0, 0, 256, 256);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, (double) 1, 1e-3, 50);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //TODO center should stay the same, recompute eye and up vectors
    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    glBegin(GL_QUADS);
    for (unsigned i = 0; i< this->patches.size(); i++)
    {
        glColor3f(this->patches[i]->uniqueColor[0], this->patches[i]->uniqueColor[1], this->patches[i]->uniqueColor[2]);
        for (int j = 0; j < 4; j++)
            glVertex3f(this->patches[i]->vertices[j].position[0],
                       this->patches[i]->vertices[j].position[1],
                       this->patches[i]->vertices[j].position[2]);
    }
    glEnd();
    glReadPixels(0, 0, 256, 256, GL_BGR, GL_FLOAT, screen);

    int in;
    int texIn;
    /* Copy the whole screen to texture */
    for (int h = 0; h < 256; h++)
    {
        for (int w = 0; w < 256; w++)
        {
            in = w + h * 256;
            texIn = w + h * 256;
            texFront[texIn] = screen[in];
        }
    }

    /* Top view */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //is this correct?
    glViewport(0, 0, 256, 256);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, (double) 1, 1e-3, 50);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //TODO center should stay the same, recompute eye and up vectors
    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    glBegin(GL_QUADS);
    for (unsigned i = 0; i< this->patches.size(); i++)
    {
        glColor3f(this->patches[i]->uniqueColor[0], this->patches[i]->uniqueColor[1], this->patches[i]->uniqueColor[2]);
        for (int j = 0; j < 4; j++)
            glVertex3f(this->patches[i]->vertices[j].position[0],
                       this->patches[i]->vertices[j].position[1],
                       this->patches[i]->vertices[j].position[2]);
    }
    glEnd();
    glReadPixels(0, 0, 256, 256, GL_BGR, GL_FLOAT, screen);

    /* Copy the bottom part of a screen to texture */
    for (int h = 128; h < 256; h++)
    {
        for (int w = 0; w < 256; w++)
        {
            in = w + h * 256;
            texIn = w + (h - 128) * 256;
            texTop[texIn] = screen[in];
        }
    }

    /* Bottom view */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //is this correct?
    glViewport(0, 0, 256, 256);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, (double) 1, 1e-3, 50);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //TODO center should stay the same, recompute eye and up vectors
    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    glBegin(GL_QUADS);
    for (unsigned i = 0; i< this->patches.size(); i++)
    {
        glColor3f(this->patches[i]->uniqueColor[0], this->patches[i]->uniqueColor[1], this->patches[i]->uniqueColor[2]);
        for (int j = 0; j < 4; j++)
            glVertex3f(this->patches[i]->vertices[j].position[0],
                       this->patches[i]->vertices[j].position[1],
                       this->patches[i]->vertices[j].position[2]);
    }
    glEnd();
    glReadPixels(0, 0, 256, 256, GL_BGR, GL_FLOAT, screen);

    /* Copy the top part of a screen to texture */
    for (int h = 0; h < 128; h++)
    {
        for (int w = 0; w < 256; w++)
        {
            in = w + h * 256;
            texIn = w + h * 256;
            texBottom[texIn] = screen[in];
        }
    }

    /* Left view */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //is this correct?
    glViewport(0, 0, 256, 256); //FIXME
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, (double) 1, 1e-3, 50);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //TODO center should stay the same, recompute eye and up vectors
    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    glBegin(GL_QUADS);
    for (unsigned i = 0; i< this->patches.size(); i++)
    {
        glColor3f(this->patches[i]->uniqueColor[0], this->patches[i]->uniqueColor[1], this->patches[i]->uniqueColor[2]);
        for (int j = 0; j < 4; j++)
            glVertex3f(this->patches[i]->vertices[j].position[0],
                       this->patches[i]->vertices[j].position[1],
                       this->patches[i]->vertices[j].position[2]);
    }
    glEnd();
    glReadPixels(0, 0, 256, 256, GL_BGR, GL_FLOAT, screen);

    /* Copy the right part of a screen to texture */
    for (int h = 0; h < 256; h++)
    {
        for (int w = 128; w < 256; w++)
        {
            in = w + h * 256;
            texIn = (w - 128) + h * 256;
            texLeft[texIn] = screen[in];
        }
    }

    /* Right view */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //is this correct?
    glViewport(0, 0, 256, 256);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, (double) 1, 1e-3, 50);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //TODO center should stay the same, recompute eye and up vectors
    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    glBegin(GL_QUADS);
    for (unsigned i = 0; i< this->patches.size(); i++)
    {
        glColor3f(this->patches[i]->uniqueColor[0], this->patches[i]->uniqueColor[1], this->patches[i]->uniqueColor[2]);
        for (int j = 0; j < 4; j++)
            glVertex3f(this->patches[i]->vertices[j].position[0],
                       this->patches[i]->vertices[j].position[1],
                       this->patches[i]->vertices[j].position[2]);
    }
    glEnd();
    glReadPixels(0, 0, 256, 256, GL_BGR, GL_FLOAT, screen);

    /* Copy the left part of a screen to texture */
    for (int h = 0; h < 256; h++)
    {
        for (int w = 0; w < 128; w++)
        {
            in = w + h * 256;
            texIn = w + h * 256;
            texRight[texIn] = screen[in];
        }
    }

    delete [] screen;
}

void PGR_model::generateUniqueColor()
{
    for(int i = 0; i < this->patches.size(); i++)
    {
        float *uniqueColor = new float [3];
        this->idToUniqueColor(i, &uniqueColor);
        this->patches[i]->uniqueColor[0] = uniqueColor[0];
        this->patches[i]->uniqueColor[1] = uniqueColor[1];
        this->patches[i]->uniqueColor[2] = uniqueColor[2];
        delete[] uniqueColor;
    }
}

void PGR_model::idToUniqueColor(int id, float *uniqueColor[3])
{
    (*uniqueColor)[0] = (float)(id & 0b11111111) / 256;
    id >>= 8;
    (*uniqueColor)[1] = (float)(id & 0b11111111) / 256;
    id >>= 8;
    (*uniqueColor)[2] = (float)(id & 0b11111111) / 256;
}

int PGR_model::uniqueColorToId(float *uniqueColor)
{
    int id = (int)(uniqueColor[2] * 256);
    id <<= 8;
    id |= (int)(uniqueColor[1] * 256);
    id <<= 8;
    id |= (int)(uniqueColor[0] * 256);

    return id;
}