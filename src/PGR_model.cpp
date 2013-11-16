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
}

PGR_model::PGR_model(const PGR_model& orig)
{
}

PGR_model::~PGR_model()
{
    //delete [] this->indices;
    //delete [] this->vertices;
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
