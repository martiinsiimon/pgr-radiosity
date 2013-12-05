/*
 * File:   PGR_radiosity.h
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 2013-11-29, 10:10
 */

#ifndef PGR_RADIOSITY_H
#define	PGR_RADIOSITY_H

#include "PGR_model.h"
#include <glm/gtc/type_ptr.hpp>
#include <CL/opencl.h>
#include <stdarg.h>
#include <cstdio>


#define PGR_GPU 1
#define PGR_CPU 2
#define N 10
#define LIMIT 0.05
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

using namespace std;


class PGR_radiosity {
public:
    PGR_radiosity();
    PGR_radiosity(PGR_model * m);
    PGR_radiosity(const PGR_radiosity& orig);
    virtual ~PGR_radiosity();

    void compute();

private:
    void computeRadiosity();
    double formFactor(glm::vec3, glm::vec3, glm::vec3, glm::vec3, float);
    bool visible();
    //TODO here should be more function used to compute radiosity on CPU

    void computeRadiosityCL();
    int prepareCL();
    void releaseCL();
    void runRadiosityKernelCL();

    const char *CLErrorString(cl_int _err);
    void CheckOpenCLError(cl_int _ciErr, const char *_sMsg, ...);
    char* loadProgSource(const char* cFilename);
    int printTiming(cl_event event, const char* title);

    /* Radiosity structures */
    /*struct factors;*/


    /* OpenCL structures */
    /*struct factorsCL;*/
    cl_context context;
    cl_command_queue commandQueue;
    cl_kernel radiosityKernel; //, sortKernel;
    cl_mem factorsCL; // form factors
    cl_mem indicesCL; // indices of N most energized items
    cl_mem patchesCL; // struct of all patches
    cl_mem patchesGeometryCL; // struct of all patches
    cl_program program; // OpenCL program
    cl_float16 * raw_patchesGeometry;
    cl_float4 * raw_patches;
    cl_uint * raw_indices;
    uint maxWorkGroupSize;
    uint workGroupSize;



    PGR_model * model;
    bool computedFactors;
    bool computedRadiosity;
    int core;
};

#endif	/* PGR_RADIOSITY_H */

