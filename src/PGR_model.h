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
#include <GL/glew.h>
#include <iostream>
#include <CL/cl.h>

using namespace std;


class PGR_model {
public:
    PGR_model();
    PGR_model(int t);
    PGR_model(const PGR_model& orig);
    virtual ~PGR_model();

    void appendModel(PGR_model *m);

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
    int getPatchesCL(cl_float4 *data, cl_double *energies);

    /* Fill given opencl memory object with patches geometry */
    int getPatchesGeometryCL(cl_float16 *data);

    /* Fill a vector of indices of N most energized patches. The index is in vector
     * patches */
    int getIdsOfNMostEnergizedPatches(vector<uint> *ids, int n, double limit);
    cl_uint getIdsOfNMostEnergizedPatchesCL(cl_uint *indices, int n, double limit);

    /* Get the energy of the most energized patch. Used to decide to terminate
     * radiosity computation */
    double getMaximalEnergy();

    void decodePatchesGeometryCL(cl_float16 *data, uint size);
    void decodePatchesCL(cl_float4 *data, cl_double *energies, uint size);


    void getViewFromPatch(int i, GLuint fbo, GLuint texture, cl_float3 **texFront, cl_float3 **texTop, cl_float3 **texBottom, cl_float3 **texLeft, cl_float3 **texRight);

    void idToUniqueColor(int id, cl_float3* uniqueColor);
    int uniqueColorToId(cl_float3 uniqueColor);
    void generateUniqueColor();


    vector<PGR_patch*> patches;
private:
    void deletePatches();
    void addLightEnergy(double e);
    void drawUniqueColorScene();

    Point *vertices;
    unsigned int verticesCount;

    unsigned int * indices;
    unsigned int indicesCount;

    float maxArea;


};

#endif	/* PGR_MODEL_H */

