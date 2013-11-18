/*
 * File:   PGR_Patch.h
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 16. listopad 2013, 17:11
 */

#ifndef PGR_PATCH_H
#define	PGR_PATCH_H

#include "model.h"
#include <vector>

using namespace std;

class PGR_patch {
public:
    PGR_patch();
    PGR_patch(const PGR_patch& orig);
    virtual ~PGR_patch();

    vector<PGR_patch*> divide(float area);
    //void addVertex(Point p);
    void addVertices(Point p0, Point p1, Point p2, Point p3);
public:
    Point vertices[4];
};

#endif	/* PGR_PATCH_H */

