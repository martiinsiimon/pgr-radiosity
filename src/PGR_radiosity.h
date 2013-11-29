/*
 * File:   PGR_radiosity.h
 * Author: martin
 *
 * Created on 29. listopad 2013, 10:10
 */

#ifndef PGR_RADIOSITY_H
#define	PGR_RADIOSITY_H

#include "PGR_model.h"
#include <glm/gtc/type_ptr.hpp>


#define PGR_GPU 1
#define PGR_CPU 2
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
    glm::vec3 formFactor(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, glm::vec3);
    //TODO here should be more function used to compute radiosity on CPU

    void computeRadiosityCL();
    void prepareCL();
    void releaseCL();

    /* Radiosity structures */
    /*struct factors;*/


    /* OpenCL structures */
    /*struct factorsCL;*/

    PGR_model * model;
    bool computedFactors;
    bool computedRadiosity;
    int core;
};

#endif	/* PGR_RADIOSITY_H */

