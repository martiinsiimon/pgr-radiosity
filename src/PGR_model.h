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
#include <iostream>
#include <CL/cl.h>

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

    /* Return number of patches */
    int getPatchesCount();

    /* Fill given opencl memory object with patches */
    int getPatchesCL(cl_float4 *data);

    /* Fill given opencl memory object with patches geometry */
    int getPatchesGeometryCL(cl_float16 *data);

    /* Fill a list indices if N most energized patches. The index is in vector
     * patches */
    int getIdsOfNMostEnergizedPatches(int **ids);

    /* Get the energy of the most energized patch. Used to decide to terminate
     * radiosity computation */
    double getMaximalEnergy();

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

