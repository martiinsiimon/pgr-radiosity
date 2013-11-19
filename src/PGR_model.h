/*
 * File:   PGR_model.h
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 16. listopad 2013, 17:12
 */

#ifndef PGR_MODEL_H
#define	PGR_MODEL_H

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#include "model.h"
#include "PGR_patch.h"
#include <vector>

using namespace std;


class PGR_model {
public:
    PGR_model();
    PGR_model(const PGR_model& orig);
    virtual ~PGR_model();

    void updateArrays();
    void updatePatches();
    void setMaxArea(float area);
    float * getVertices();
    unsigned int getVerticesCount();

    unsigned int * getIndices();
    unsigned int getIndicesCount();
    void divide();

    vector<PGR_patch*> patches;
private:
    void deletePatches();

    Point *vertices;
    unsigned int verticesCount;

    unsigned int * indices;
    unsigned int indicesCount;

    float maxArea;
};

#endif	/* PGR_MODEL_H */

