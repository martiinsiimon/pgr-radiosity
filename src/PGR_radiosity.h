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
    PGR_radiosity(PGR_model * m, int c);
    PGR_radiosity(const PGR_radiosity& orig);
    virtual ~PGR_radiosity();

    void compute();
    bool isComputed();
    void setFramebuffer(GLuint fbo);
    void setTexture(GLuint tex);

private:
    void computeRadiosity();
    double formFactor(glm::vec3, glm::vec3, glm::vec3, glm::vec3, float);

    void computeRadiosityCL();
    int prepareCL();
    void releaseCL();
    void runRadiosityKernelCL();

    const char *CLErrorString(cl_int _err);
    void CheckOpenCLError(cl_int _ciErr, const char *_sMsg, ...);
    char* loadProgSource(const char* cFilename);

    /* OpenCL structures */
    cl_context context;
    cl_command_queue queue;
    cl_kernel radiosityKernel, sortKernel;
    cl_mem indicesCountCL; // count of all patches needs to be global
    cl_mem indicesCL; // indices of N most energized items
    cl_mem patchesColorsCL; // struct of all patches colors
    cl_mem patchesGeoCL; // struct of all patches geometries
    cl_mem patchesEnergiesCL; //struct of all patches energies
    cl_mem maximalEnergyCL; // maximal energy needs to be global
    cl_mem diffColorsCL; //diferencies of colors
    cl_mem intensitiesCL; //intenesition of light
    cl_mem texturesCL; //textures of viewFromPatch
    cl_mem visitedCL; //array of flags of visition patch
    cl_program program; // OpenCL program
    cl_float8 * raw_patchesGeo;
    cl_uchar3 * raw_patchesColors;
    cl_uint * raw_indices;
    cl_float * raw_patchesEnergies;
    cl_uchar3* raw_textures;
    cl_uchar3* raw_diffColors;
    cl_float* raw_intensities;
    uint maxWorkGroupSize;
    uint workGroupSize;

    GLuint framebuffer;
    GLuint texture;

    PGR_model * model;
    bool computedRadiosity;
    int core;
};

#endif	/* PGR_RADIOSITY_H */

