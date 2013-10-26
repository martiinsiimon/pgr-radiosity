/*
 * File:   PGR_model.cpp
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 2013-10-13, 21:30 - changed
 *
 *  * License note: This OBJ file loader is designed by tutorial at
 * http://www.tutorialized.com/tutorial/Write-a-WaveFront-OpenGL-3D-object-loader-in-C++/59679
 * by James Waples under license CC BY-NC-SA http://creativecommons.org/licenses/by-nc-sa/3.0/
 */

#include "PGR_model.h"

PGR_model::PGR_model()
{
}

PGR_model::PGR_model(string file)
{
    this->_file = file;
}

PGR_model::PGR_model(const PGR_model& orig)
{
}

PGR_model::~PGR_model()
{
}

/**
 * OBJ file loader. See file header for author information
 * @return bool True if loading is successful
 */
bool PGR_model::loadModel()
{
    fstream objFile;
    objFile.open(this->_file.c_str());

    if (objFile.is_open())
    {
        string line;

        // Parse object file line by line
        while (objFile.good())
        {
            //objFile.getline(line, 255);
            getline(objFile, line);
            parseLine(line);
        }

        objFile.close();
    }
    else
    {
        cout << "Could not open WFObject file '" << this->_file << endl;
        return false;
    }

    return true;
}

/**
 * Parse line given in parameter. Start relevant parser due the recognized
 * element type
 * @param line Line to be parsed
 */
void PGR_model::parseLine(string line)
{
    /* If string is empty, stop parsing */
    if (line.empty())
    {
        return;
    }

    string lineType;
    lineType = strtok(strdup(line.c_str()), " ");

    /* Decide what to do */
    if (!lineType.compare("v")) // Vertex
    {
        parseVertex(line);
    }
    else if (!lineType.compare("vn")) // Normal
    {
        parseNormal(line);
    }
    else if (!lineType.compare("vt")) // Texture
    {
        parseTexture(line);
    }
    else if (!lineType.compare("f")) // Face
    {
        parseFace(line);
    }
}

/**
 * Parse given line into vertex vector
 * @param line Line to be parsed
 */
void PGR_model::parseVertex(string line)
{
    /* Add a new element to the vertices array */
    verticesCoords.push_back(Vector());

    /* Update newly added vector */
    sscanf(line.c_str(), "v %f %f %f", &verticesCoords.back().x, &verticesCoords.back().y, &verticesCoords.back().z);
}

/**
 * Parse given line into normal vector
 * @param line Line to be parsed
 */
void PGR_model::parseNormal(string line)
{
    /* Add a new element to the normals array */
    normalsCoords.push_back(Vector());

    /* Update newly added vector */
    sscanf(line.c_str(), "vn %f %f %f", &normalsCoords.back().x, &normalsCoords.back().y, &normalsCoords.back().z);
}

/**
 * Parse given line into texture tuple
 * @param line Line to be parsed
 */
void PGR_model::parseTexture(string line)
{
    /* Add a new element to the textures array */
    textureCoords.push_back(Vector2D());

    /* Update newly added vector */
    sscanf(line.c_str(), "vt %f %f", &textureCoords.back().x, &textureCoords.back().y);
}

/**
 * Parse given line into triangle structure
 * @param line Line to be parsed
 */
void PGR_model::parseFace(string line)
{
    /* Add a new element to the triangles array */
    triangles.push_back(Triangle());

    /* Read face line. If texture indicies aren't present, don't read them. */
    if (sscanf(line.c_str(), "f %d//%d %d//%d %d//%d",
               &triangles.back().v1,
               &triangles.back().n1,
               &triangles.back().v2,
               &triangles.back().n2,
               &triangles.back().v3,
               &triangles.back().n3) <= 1)
    {
        sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
               &triangles.back().v1,
               &triangles.back().t1,
               &triangles.back().n1,
               &triangles.back().v2,
               &triangles.back().t1,
               &triangles.back().n2,
               &triangles.back().v3,
               &triangles.back().t1,
               &triangles.back().n3);
    }
}