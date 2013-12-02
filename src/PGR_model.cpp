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

PGR_model::PGR_model()
{
    this->indices = new unsigned int[(sizeof (room)) / sizeof (*room)];
    memcpy(this->indices, room, sizeof (room));
    this->indicesCount = sizeof (room) / sizeof (*room);

    this->vertices = new Point[(sizeof (roomVertices)) / sizeof (*roomVertices)];
    memcpy(this->vertices, roomVertices, sizeof (roomVertices));
    this->verticesCount = sizeof (roomVertices) / sizeof (*roomVertices);

    this->maxArea = -1.0;
    this->updatePatches();
}

PGR_model::PGR_model(const PGR_model& orig)
{
}

PGR_model::~PGR_model()
{
    delete [] this->indices;
    delete [] this->vertices;
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
            tmpPatches.push_back(np);
        }
    }

    this->deletePatches();

    this->patches = tmpPatches;
}

int PGR_model::getPatchesCL(cl_float4 *data)
{
    int i;
    for (i = 0; i < this->patches.size(); i++)
    {
        //energy
        data[i].s0 = this->patches.at(i)->energy;

        //color, all vertices are the same
        data[i].s1 = this->patches.at(i)->vertices[0].color[0];
        data[i].s2 = this->patches.at(i)->vertices[0].color[1];
        data[i].s3 = this->patches.at(i)->vertices[0].color[2];
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

        //the last part is unused
        data[i].s[15] = 0;
    }
    return i;
}

int PGR_model::getIdsOfNMostEnergizedPatches(int **ids, int n)
{
    int count = 0;
    
    vector<PGR_patch*> tmpPatches = this->patches;
    
    for(int i = 0; i < n; i++, count++) 
    {
        double energy = 0;
        int id = 0;
        for(int j = 0; j < tmpPatches.size(); j++)
        {
            if(tmpPatches[j]->energy > energy)
            {
                energy = tmpPatches[j]->energy;
                id = j;
            }
        }
        
        (*ids)[i] = id;
        tmpPatches[id]->energy = 0;
    }
    
    return count;
}

double PGR_model::getMaximalEnergy()
{
    double energy = 0;
    for(int i = 0; i < this->patches.size(); i++)
    {
        energy = MAX(energy, this->patches[i]->energy);
    }
    
    return energy;
}