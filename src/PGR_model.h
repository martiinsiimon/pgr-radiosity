/*
 * File:   PGR_model.h
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 2013-10-13, 21:30
 *
 * License note: This OBJ file loader is designed by tutorial at
 * http://www.tutorialized.com/tutorial/Write-a-WaveFront-OpenGL-3D-object-loader-in-C++/59679
 * by James Waples under license CC BY-NC-SA http://creativecommons.org/licenses/by-nc-sa/3.0/
 *
 */

#ifndef PGR_MODEL_H
#define	PGR_MODEL_H

#include <vector>
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;
typedef unsigned int uint;

typedef struct {
    float x;
    float y;
    float z;
} Vector;

typedef struct {
    float x;
    float y;
} Vector2D;

typedef struct {
    uint v1, v2, v3;
    uint n1, n2, n3;
    uint t1, t2, t3;
} Triangle;

/**
 * Class represents model mesh. The class contains method to load an file model.
 */
class PGR_model {
public:
    PGR_model();
    PGR_model(string file);
    PGR_model(const PGR_model& orig);
    virtual ~PGR_model();
    bool loadModel();

    /* Every model's vertex is represented by 3 coordinates - x, y and z */
    vector<Vector> verticesCoords;
    /* Every texture coordinate has 2 values */
    vector<Vector2D> textureCoords;
    /* Every vertex has normal vector represented by 3 coordinates */
    vector<Vector> normalsCoords;

    /* Triangle represented by 3 vertices, 3 normal vectors and 3 texture vectors.
     * All of these are indexes to lists with coords */
    vector<Triangle> triangles;

private:
    string _file;
    void parseLine(string);

    void parseVertex(string);
    void parseNormal(string);
    void parseTexture(string);
    void parseFace(string);
};

#endif	/* PGR_MODEL_H */

