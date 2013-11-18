/*
 * File:   PGR_model.cpp
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 16. listopad 2013, 17:12
 */
#include "PGR_model.h"
#include <iostream>
#include <cstring>

PGR_model::PGR_model()
{
    this->indices = new unsigned char[(sizeof (room)) / sizeof (*room)];
    memcpy(this->indices, room, sizeof (room));
    this->indicesCount = sizeof (room) / sizeof (*room);

    this->vertices = new Point[(sizeof (roomVertices)) / sizeof (*roomVertices)];
    memcpy(this->vertices, roomVertices, sizeof (roomVertices));
    this->verticesCount = sizeof (roomVertices) / sizeof (*roomVertices);

    this->maxArea = -1.0;
    this->updatePatches();
    cout << "test" << endl;
}

PGR_model::PGR_model(const PGR_model& orig)
{
}

PGR_model::~PGR_model()
{

}


unsigned char* PGR_model::getIndices()
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
    delete []this->indices;
    delete []this->vertices;

    this->indices = new unsigned char[this->patches.size() * 4];
    this->vertices = new Point[this->patches.size() * 4];

    this->indicesCount = this->patches.size() * 4;
    this->verticesCount = this->patches.size() * 4;

    unsigned int sizeIndices = 0;
    unsigned int sizeVertices = 0;
    unsigned int pointSize = sizeof (Point);
    unsigned int charSize = sizeof (unsigned char);
    unsigned char i = 0;

    for (int n = 0; n < this->patches.size(); n++)
    {
        *(this->indices + sizeIndices) = i++;
        sizeIndices += charSize;
        *(this->indices + sizeIndices) = i++;
        sizeIndices += charSize;
        *(this->indices + sizeIndices) = i++;
        sizeIndices += charSize;
        *(this->indices + sizeIndices) = i++;
        sizeIndices += charSize;

        *(this->vertices + sizeVertices) = this->patches.at(n)->vertices[0];
        sizeVertices += pointSize;
        *(this->vertices + sizeVertices) = this->patches.at(n)->vertices[1];
        sizeVertices += pointSize;
        *(this->vertices + sizeVertices) = this->patches.at(n)->vertices[2];
        sizeVertices += pointSize;
        *(this->vertices + sizeVertices) = this->patches.at(n)->vertices[3];
        sizeVertices += pointSize;
    }
}

void PGR_model::updatePatches()
{
    for (int i = 0; i < this->patches.size(); i++)
    {
        PGR_patch *p = this->patches.at(i);
        delete p;
    }
    this->patches.clear();


    for (int i = 0; i < this->indicesCount; i += 4)
    {
        PGR_patch * p = new PGR_patch();
        p->addVertices(this->vertices[i], this->vertices[i + 1], this->vertices[i + 2], this->vertices[i + 3]);
        this->patches.push_back(p);
    }
}

void PGR_model::divide()
{
    if (this->maxArea <= 0.0)
        return;

    vector<PGR_patch*> tmpPatches;

    for (vector<PGR_patch*>::iterator it = this->patches.begin(); it != this->patches.end(); it++)
    {
        //FIXME the area is computed in for rectangles only
        float area;
        int c1, c2;
        if ((*it)->vertices[0].position[0] == (*it)->vertices[1].position[0]
            && (*it)->vertices[1].position[0] == (*it)->vertices[2].position[0]
            && (*it)->vertices[2].position[0] == (*it)->vertices[3].position[0])
        {
            /* in x-axis direction */
            c1 = 1;
            c2 = 2;
        }
        else if ((*it)->vertices[0].position[1] == (*it)->vertices[1].position[1]
            && (*it)->vertices[1].position[1] == (*it)->vertices[2].position[1]
            && (*it)->vertices[2].position[1] == (*it)->vertices[3].position[1])
        {
            /* in y-axis direction */
            c1 = 0;
            c2 = 2;
        }
        else if ((*it)->vertices[0].position[2] == (*it)->vertices[1].position[2]
            && (*it)->vertices[1].position[2] == (*it)->vertices[2].position[2]
            && (*it)->vertices[2].position[2] == (*it)->vertices[3].position[2])
        {
            /* in z-axis direction */
            c1 = 0;
            c2 = 1;
        }
        else
        {
            /* general direction */
            return; //NOT IMPLEMENTED
        }
        float l1 = MAX(
                       MAX((*it)->vertices[0].position[c1], (*it)->vertices[1].position[c1]),
                       MAX((*it)->vertices[2].position[c1], (*it)->vertices[3].position[c1]))
            - MIN(
                  MIN((*it)->vertices[0].position[c1], (*it)->vertices[1].position[c1]),
                  MIN((*it)->vertices[2].position[c1], (*it)->vertices[3].position[c1]));
        float l2 = MAX(
                       MAX((*it)->vertices[0].position[c2], (*it)->vertices[1].position[c2]),
                       MAX((*it)->vertices[2].position[c2], (*it)->vertices[3].position[c2]))
            - MIN(
                  MIN((*it)->vertices[0].position[c2], (*it)->vertices[1].position[c2]),
                  MIN((*it)->vertices[2].position[c2], (*it)->vertices[3].position[c2]));
        area = l1 * l2;

        if (area > this->maxArea)
        {
            vector<PGR_patch*> vec = (*it)->divide(this->maxArea);

            for (vector<PGR_patch*>::iterator tmp = vec.begin(); tmp != vec.end(); tmp++)
            {
                tmpPatches.push_back(*tmp);
            }

        }
        else
        {
            tmpPatches.push_back((*it));
        }

    }

    while (!this->patches.empty())
    {
        PGR_patch *p = this->patches.back();
        delete p;
        this->patches.pop_back();
    }

    this->patches = tmpPatches;
}