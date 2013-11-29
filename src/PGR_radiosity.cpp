/*
 * File:   PGR_radiosity.cpp
 * Author: martin
 *
 * Created on 29. listopad 2013, 10:10
 */

#include "PGR_radiosity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


PGR_radiosity::PGR_radiosity()
{
}

PGR_radiosity::PGR_radiosity(PGR_model * m)
{
    this->model = m;
    this->computedFactors = false;
    this->computedRadiosity = false;
    this->core = PGR_CPU;
}

PGR_radiosity::PGR_radiosity(const PGR_radiosity& orig)
{
}

PGR_radiosity::~PGR_radiosity()
{
}


void PGR_radiosity::compute()
{
    if (this->core = PGR_CPU)
    {
        this->computeRadiosity();
    }
    else
    {
        this->computeRadiosityCL();
    }
}

void PGR_radiosity::computeRadiosity()
{
    /**
     * Compute form factors, set this->computedFactors = true
     * Compute radiosity, set this->computedRadiosity = true
     */
}

/*
 * Computation form factor
 * @param glm::vec3 RecvPos - world-space position of this element
 * @param glm::vec3 ShootPos - world-space position of shooter
 * @param glm::vec3 RecvNormal - world-space normal of this element
 * @param glm::vec3 ShootNormal - world-space normal of shooter
 * @param glm::vec3 ShooterEnergy - energy from shooter residual texture
 * @param float ShootDArea - the delta area of the shooter
 * @param glm::vec3 RecvColor - the reflectivity of this element
 */
glm::vec3 PGR_radiosity::formFactor(glm::vec3 RecvPos, glm::vec3 ShootPos, glm::vec3 RecvNormal, glm::vec3 ShootNormal, glm::vec3 ShooterEnergy, float ShootDArea, glm::vec3 RecvColor)
{
    // a normalized vector from shooter to receiver
    glm::vec3 r = ShootPos - RecvPos;
    float distance2 = glm::dot(r, r);
    r = glm::normalize(r);

    // the angles of the receiver and the shooter from r
    float cosi = glm::dot(RecvNormal, r);
    float cosj = -glm::dot(ShootNormal, r);

    // compute the disc approximation form factor
    float Fij = max(cosi * cosj, (float) 0) / (M_PI * distance2 + ShootDArea);

    // Modulate shooter's energy by the receiver's reflectivity
    // and the area of the shooter.
    glm::vec3 delta = ShooterEnergy * RecvColor * ShootDArea * Fij;

    return delta;
}

void PGR_radiosity::computeRadiosityCL()
{
    /* Prepare CL structures */
    this->prepareCL();

    if (!this->computedFactors)
    {
        //TODO begin scope to compute factors
    }
    else
    {
        //TODO set factors to this->factorsCL
    }

    //TODO compute radiosity until this->model->getMaximalEnergy <= limit


    //TODO read buffers from GPU and copy that back to this->model

    /* Release CL structures */
    this->releaseCL();
}

void PGR_radiosity::prepareCL()
{
    //TODO prepare OpenCL structures
}

void PGR_radiosity::releaseCL()
{
    //TODO release OpenCL structures
}