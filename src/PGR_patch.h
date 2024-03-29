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
#include <string>
#include <CL/cl.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

using namespace std;

class PGR_patch {
public:
    PGR_patch();
    PGR_patch(const PGR_patch& orig);
    PGR_patch(PGR_patch *orig);
    virtual ~PGR_patch();

    void divide(float area, vector<PGR_patch*> *result);
    void setVertices(Point p0, Point p1, Point p2, Point p3);
    void setArea(float a);
    void setEnergy(double e);
    double getEnergy();
    void setIntensity(float i);
    float getIntensity();

    void computeArea();
    void computeCenter();
    float getShorterLength();
    float getLongerLength();
    bool getOrientation(int * c1, int * c2);

    void printVertices();
    void printNormals();
private:
    bool getLengths(float * l1, float * l2);
    void copy(PGR_patch * a, PGR_patch * src);
    void copyNormals(PGR_patch * a, PGR_patch * src);
    void copyCoordinates(PGR_patch * a, PGR_patch * src);
    void copyColors(PGR_patch * a, PGR_patch * src);
public:
    Point vertices[4];
    float area;
    cl_float3 center;
    double energy;
    cl_uchar3 uniqueColor;
    cl_uint3 newDiffColor;
    float intensity;
};

#endif	/* PGR_PATCH_H */

