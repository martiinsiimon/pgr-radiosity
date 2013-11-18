/*
 * File:   PGR_Patch.cpp
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 16. listopad 2013, 17:11
 */

#include "PGR_patch.h"
#include <iostream>

using namespace std;


PGR_patch::PGR_patch()
{
}

PGR_patch::PGR_patch(const PGR_patch& orig)
{
}

PGR_patch::~PGR_patch()
{
}

void PGR_patch::addVertices(Point p0, Point p1, Point p2, Point p3)
{
    this->vertices[0] = p0;
    this->vertices[1] = p1;
    this->vertices[2] = p2;
    this->vertices[3] = p3;
}

vector<PGR_patch*> PGR_patch::divide(float area)
{
    vector<PGR_patch*> vec;
    vec.push_back(this);
    return vec;
}