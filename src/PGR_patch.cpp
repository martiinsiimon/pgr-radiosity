/*
 * File:   PGR_Patch.cpp
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 16. listopad 2013, 17:11
 */

#include "PGR_patch.h"
#include <iostream>
#include <cstring>

using namespace std;


PGR_patch::PGR_patch()
{
    this->vertices[0] = Point();
    this->vertices[1] = Point();
    this->vertices[2] = Point();
    this->vertices[3] = Point();
    this->area = 0;
    this->energy = 0;

    this->newDiffColor[0] = 0;
    this->newDiffColor[1] = 0;
    this->newDiffColor[2] = 0;

    this->intensity = 0;
}

PGR_patch::PGR_patch(const PGR_patch& orig)
{
    this->vertices[0] = orig.vertices[0];
    this->vertices[1] = orig.vertices[0];
    this->vertices[2] = orig.vertices[0];
    this->vertices[3] = orig.vertices[0];
    this->area = orig.area;
    this->energy = orig.energy;

    this->newDiffColor[0] = orig.newDiffColor[0];
    this->newDiffColor[1] = orig.newDiffColor[1];
    this->newDiffColor[2] = orig.newDiffColor[2];

    this->intensity = orig.intensity;

}

PGR_patch::PGR_patch(PGR_patch *orig)
{
    this->vertices[0] = orig->vertices[0];
    this->vertices[1] = orig->vertices[0];
    this->vertices[2] = orig->vertices[0];
    this->vertices[3] = orig->vertices[0];
    this->area = orig->area;
    this->energy = orig->energy;

    this->newDiffColor[0] = orig->newDiffColor[0];
    this->newDiffColor[1] = orig->newDiffColor[1];
    this->newDiffColor[2] = orig->newDiffColor[2];

    this->intensity = orig->intensity;
}

PGR_patch::~PGR_patch()
{
}

void PGR_patch::setVertices(Point p0, Point p1, Point p2, Point p3)
{
    this->vertices[0] = p0;
    this->vertices[1] = p1;
    this->vertices[2] = p2;
    this->vertices[3] = p3;

    this->computeArea();
}

void PGR_patch::printVertices()
{
    cout << "v1: [" << this->vertices[0].position[0] << "," << this->vertices[0].position[1] << "," << this->vertices[0].position[2] << "], " <<
        "v2: [" << this->vertices[1].position[0] << "," << this->vertices[1].position[1] << "," << this->vertices[1].position[2] << "], " <<
        "v3: [" << this->vertices[2].position[0] << "," << this->vertices[2].position[1] << "," << this->vertices[2].position[2] << "], " <<
        "v4: [" << this->vertices[3].position[0] << "," << this->vertices[3].position[1] << "," << this->vertices[3].position[2] << "]";
}

void PGR_patch::printNormals()
{
    cout << "n1: [" << this->vertices[0].normal[0] << "," << this->vertices[0].normal[1] << "," << this->vertices[0].normal[2] << "], " <<
        "n2: [" << this->vertices[1].normal[0] << "," << this->vertices[1].normal[1] << "," << this->vertices[1].normal[2] << "], " <<
        "n3: [" << this->vertices[2].normal[0] << "," << this->vertices[2].normal[1] << "," << this->vertices[2].normal[2] << "], " <<
        "n4: [" << this->vertices[3].normal[0] << "," << this->vertices[3].normal[1] << "," << this->vertices[3].normal[2] << "]";
}

void PGR_patch::setArea(float a)
{
    this->area = a;
}

void PGR_patch::setEnergy(double e)
{
    this->energy = e;
}

void PGR_patch::setIntensity(float i)
{
    this->intensity = i;
}

double PGR_patch::getEnergy()
{
    return this->energy;
}

void PGR_patch::computeArea()
{
    float l1, l2;
    this->getLengths(&l1, &l2);

    this->area = l1 * l2;
}

bool PGR_patch::getOrientation(int * c1, int * c2)
{
    if (this->vertices[0].position[0] == this->vertices[1].position[0]
        && this->vertices[1].position[0] == this->vertices[2].position[0]
        && this->vertices[2].position[0] == this->vertices[3].position[0])
    {
        /* in x-axis direction */
        (*c1) = 1;
        (*c2) = 2;
    }
    else if (this->vertices[0].position[1] == this->vertices[1].position[1]
        && this->vertices[1].position[1] == this->vertices[2].position[1]
        && this->vertices[2].position[1] == this->vertices[3].position[1])
    {
        /* in y-axis direction */
        (*c1) = 0;
        (*c2) = 2;
    }
    else if (this->vertices[0].position[2] == this->vertices[1].position[2]
        && this->vertices[1].position[2] == this->vertices[2].position[2]
        && this->vertices[2].position[2] == this->vertices[3].position[2])
    {
        /* in z-axis direction */
        (*c1) = 0;
        (*c2) = 1;
    }
    else
    {
        /* general direction */
        return false; //NOT IMPLEMENTED
    }
    return true;
}

bool PGR_patch::getLengths(float *l1, float *l2)
{
    int c1, c2;

    if (!this->getOrientation(&c1, &c2))
    {
        return false;
    }

    (*l1) = MAX(
                MAX(this->vertices[0].position[c1], this->vertices[1].position[c1]),
                MAX(this->vertices[2].position[c1], this->vertices[3].position[c1]))
        - MIN(
              MIN(this->vertices[0].position[c1], this->vertices[1].position[c1]),
              MIN(this->vertices[2].position[c1], this->vertices[3].position[c1]));
    (*l2) = MAX(
                MAX(this->vertices[0].position[c2], this->vertices[1].position[c2]),
                MAX(this->vertices[2].position[c2], this->vertices[3].position[c2]))
        - MIN(
              MIN(this->vertices[0].position[c2], this->vertices[1].position[c2]),
              MIN(this->vertices[2].position[c2], this->vertices[3].position[c2]));

    return true;
}

float PGR_patch::getLongerLength()
{
    float l1, l2;

    if (!this->getLengths(&l1, &l2))
    {
        return 0;
    }

    float res = l1 > l2 ? l1 : l2;

    return res;
}

float PGR_patch::getShorterLength()
{
    float l1, l2;

    if (!this->getLengths(&l1, &l2))
    {
        return 0;
    }

    float res = l1 < l2 ? l1 : l2;

    return res;
}

void PGR_patch::divide(float area, vector<PGR_patch*> *result)
{
    vector<PGR_patch*> todo;
    PGR_patch * p = new PGR_patch();
    PGR_patch * tmp;
    PGR_patch * tmp1;
    PGR_patch * tmp2;
    float lx, rx, ty, by;
    int c1, c2;
    PGR_patch a, b;
    p->setVertices(this->vertices[0], this->vertices[1], this->vertices[2], this->vertices[3]);
    p->setEnergy(this->getEnergy());
    todo.push_back(p);
    while (!todo.empty())
    {
        tmp = new PGR_patch();
        tmp->setVertices(todo.back()->vertices[0],
                         todo.back()->vertices[1],
                         todo.back()->vertices[2],
                         todo.back()->vertices[3]);
        tmp->setEnergy(todo.back()->energy);
        delete todo.back();
        todo.pop_back();

        if (tmp->area > area)
        {
            /* The area of given patch is bigger then the maximal area */
            tmp1 = new PGR_patch();
            tmp2 = new PGR_patch();
            tmp->getOrientation(&c1, &c2);

            /* Find out which vertex is which one */
            lx = MIN(MIN(tmp->vertices[0].position[c1],
                         tmp->vertices[1].position[c1]),
                     MIN(tmp->vertices[2].position[c1],
                         tmp->vertices[3].position[c1]));

            rx = MAX(MAX(tmp->vertices[0].position[c1],
                         tmp->vertices[1].position[c1]),
                     MAX(tmp->vertices[2].position[c1],
                         tmp->vertices[3].position[c1]));

            by = MIN(MIN(tmp->vertices[0].position[c2],
                         tmp->vertices[1].position[c2]),
                     MIN(tmp->vertices[2].position[c2],
                         tmp->vertices[3].position[c2]));

            ty = MAX(MAX(tmp->vertices[0].position[c2],
                         tmp->vertices[1].position[c2]),
                     MAX(tmp->vertices[2].position[c2],
                         tmp->vertices[3].position[c2]));

            copy(&a, tmp);
            copy(&b, tmp);

            a.vertices[0].position[c1] = lx;
            a.vertices[0].position[c2] = by;
            a.vertices[1].position[c2] = by;
            a.vertices[3].position[c1] = lx;

            b.vertices[1].position[c1] = rx;
            b.vertices[2].position[c1] = rx;
            b.vertices[2].position[c2] = ty;
            b.vertices[3].position[c2] = ty;

            if ((rx - lx) > (ty - by))
            {
                a.vertices[1].position[c1] = (rx - lx) / 2.0 + lx;
                a.vertices[2].position[c1] = (rx - lx) / 2.0 + lx;
                a.vertices[2].position[c2] = ty;
                a.vertices[3].position[c2] = ty;

                b.vertices[0].position[c1] = (rx - lx) / 2.0 + lx;
                b.vertices[0].position[c2] = by;
                b.vertices[1].position[c2] = by;
                b.vertices[3].position[c1] = (rx - lx) / 2.0 + lx;
            }
            else
            {
                a.vertices[1].position[c1] = rx;
                a.vertices[2].position[c1] = rx;
                a.vertices[2].position[c2] = (ty - by) / 2.0 + by;
                a.vertices[3].position[c2] = (ty - by) / 2.0 + by;

                b.vertices[0].position[c1] = lx;
                b.vertices[0].position[c2] = (ty - by) / 2.0 + by;
                b.vertices[1].position[c2] = (ty - by) / 2.0 + by;
                b.vertices[3].position[c1] = lx;
            }


            tmp1->setVertices(a.vertices[0], a.vertices[1], a.vertices[2], a.vertices[3]);
            tmp1->setEnergy(tmp->getEnergy());
            tmp2->setVertices(b.vertices[0], b.vertices[1], b.vertices[2], b.vertices[3]);
            tmp2->setEnergy(tmp->getEnergy());

            delete tmp;

            todo.push_back(tmp1);
            todo.push_back(tmp2);
        }
        else
        {
            /* The size is equal or smaller */
            result->push_back(tmp);
        }
    }
}

void PGR_patch::copy(PGR_patch* a, PGR_patch* src)
{
    this->copyColors(a, src);
    this->copyCoordinates(a, src);
    this->copyNormals(a, src);
}

void PGR_patch::copyColors(PGR_patch* a, PGR_patch* src)
{
    a->vertices[0].color[0] = src->vertices[0].color[0];
    a->vertices[0].color[1] = src->vertices[0].color[1];
    a->vertices[0].color[2] = src->vertices[0].color[2];

    a->vertices[1].color[0] = src->vertices[1].color[0];
    a->vertices[1].color[1] = src->vertices[1].color[1];
    a->vertices[1].color[2] = src->vertices[1].color[2];

    a->vertices[2].color[0] = src->vertices[2].color[0];
    a->vertices[2].color[1] = src->vertices[2].color[1];
    a->vertices[2].color[2] = src->vertices[2].color[2];

    a->vertices[3].color[0] = src->vertices[3].color[0];
    a->vertices[3].color[1] = src->vertices[3].color[1];
    a->vertices[3].color[2] = src->vertices[3].color[2];
}

void PGR_patch::copyCoordinates(PGR_patch* a, PGR_patch* src)
{
    a->vertices[0].position[0] = src->vertices[0].position[0];
    a->vertices[0].position[1] = src->vertices[0].position[1];
    a->vertices[0].position[2] = src->vertices[0].position[2];

    a->vertices[1].position[0] = src->vertices[1].position[0];
    a->vertices[1].position[1] = src->vertices[1].position[1];
    a->vertices[1].position[2] = src->vertices[1].position[2];

    a->vertices[2].position[0] = src->vertices[2].position[0];
    a->vertices[2].position[1] = src->vertices[2].position[1];
    a->vertices[2].position[2] = src->vertices[2].position[2];

    a->vertices[3].position[0] = src->vertices[3].position[0];
    a->vertices[3].position[1] = src->vertices[3].position[1];
    a->vertices[3].position[2] = src->vertices[3].position[2];
}

void PGR_patch::copyNormals(PGR_patch* a, PGR_patch* src)
{
    a->vertices[0].normal[0] = src->vertices[0].normal[0];
    a->vertices[0].normal[1] = src->vertices[0].normal[1];
    a->vertices[0].normal[2] = src->vertices[0].normal[2];

    a->vertices[1].normal[0] = src->vertices[1].normal[0];
    a->vertices[1].normal[1] = src->vertices[1].normal[1];
    a->vertices[1].normal[2] = src->vertices[1].normal[2];

    a->vertices[2].normal[0] = src->vertices[2].normal[0];
    a->vertices[2].normal[1] = src->vertices[2].normal[1];
    a->vertices[2].normal[2] = src->vertices[2].normal[2];

    a->vertices[3].normal[0] = src->vertices[3].normal[0];
    a->vertices[3].normal[1] = src->vertices[3].normal[1];
    a->vertices[3].normal[2] = src->vertices[3].normal[2];
}

void PGR_patch::updateColor()
{
    this->vertices[0].color[0] += this->newDiffColor[0];
    this->vertices[1].color[0] += this->newDiffColor[0];
    this->vertices[2].color[0] += this->newDiffColor[0];
    this->vertices[3].color[0] += this->newDiffColor[0];

    this->vertices[0].color[1] += this->newDiffColor[1];
    this->vertices[1].color[1] += this->newDiffColor[1];
    this->vertices[2].color[1] += this->newDiffColor[1];
    this->vertices[3].color[1] += this->newDiffColor[1];

    this->vertices[0].color[2] += this->newDiffColor[2];
    this->vertices[1].color[2] += this->newDiffColor[2];
    this->vertices[2].color[2] += this->newDiffColor[2];
    this->vertices[3].color[2] += this->newDiffColor[2];
}