/*
 * File:   PGR_renderer.h
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 2013-10-13, 18:20
 */

#ifndef PGR_RENDERER_H
#define	PGR_RENDERER_H

#include <iostream>
#include "PGR_model.h"

#include <glm/gtc/type_ptr.hpp>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

using namespace std;

class PGR_renderer {
public:
    PGR_renderer();
    PGR_renderer(const PGR_renderer& orig);
    PGR_renderer(string);
    virtual ~PGR_renderer();
    void init();
    void setMaxArea(float area);
    void drawSceneDefault(glm::mat4);
    void drawSceneRadiosity(glm::mat4);
private:
    void createBuffers();
    bool divide();
    void printPatches(); //DBG method
    glm::vec3 formFactor(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, glm::vec3);

private:
    PGR_model * model;
    float maxArea;
    bool divided;
};

#endif	/* PGR_RENDERER_H */

