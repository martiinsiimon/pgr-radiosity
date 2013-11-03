/*
 * File:   PGR_model.h
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 2013-10-13, 21:30
 *
 *
 */

#ifndef PGR_MODEL_H
#define	PGR_MODEL_H

using namespace std;

struct Point {
    float position[3];
    float color[3];
    float normal[3];
};

const Point roomVertices[] = {
    //floor
    {
        {-4.0, -1.5, -3.0},
        {0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0}
    },
    {
        {-4.0, -1.5, 3.0},
        {0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0}
    },
    {
        {4.0, -1.5, -3.0},
        {0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0}
    },
    {
        {4.0, -1.5, 3.0},
        {0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0}
    },
    //roof
    {
        {-4.0, 1.5, -3.0},
        {1.0, 1.0, 0.5},
        {0.0, -1.0, 0.0}
    },
    {
        {-4.0, 1.5, 3.0},
        {1.0, 1.0, 0.5},
        {0.0, -1.0, 0.0}
    },
    {
        {4.0, 1.5, -3.0},
        {1.0, 1.0, 0.5},
        {0.0, -1.0, 0.0}
    },
    {
        {4.0, 1.5, 3.0},
        {1.0, 1.0, 0.5},
        {0.0, -1.0, 0.0}
    },
    //wall left
    {
        {-4.0, -1.5, -3.0},
        {0.9, 0.9, 0.9},
        {1.0, 0.0, 0.0}
    },
    {
        {-4.0, 1.5, -3.0},
        {0.9, 0.9, 0.9},
        {1.0, 0.0, 0.0}
    },
    {
        {-4.0, -1.5, 3.0},
        {0.9, 0.9, 0.9},
        {1.0, 0.0, 0.0}
    },
    {
        {-4.0, 1.5, 3.0},
        {0.9, 0.9, 0.9},
        {1.0, 0.0, 0.0}
    },
    //wall back
    {
        {-4.0, -1.5, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {-1.0, -0.25, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {0.0, -1.5, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {1.0, -0.25, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        { 4.0, -1.5, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        { 4.0, 0.0, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        { 1.0, 1.0, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        { 4.0, 1.5, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        { 0.0, 1.5, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        { -1.0, 1.0, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        { -4.0, 1.5, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        { -4.0, 0.0, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    //wall right
    {
        {4.0, -1.5, -3.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.0, -0.25, -1.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.0, -1.5, 0.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.0, -0.25, 1.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.0, -1.5, 3.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.0, 0.0, 3.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.0, 1.0, 1.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.0, 1.5, 3.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.0, 1.5, 0.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.0, 1.0, -1.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.0, 1.5, -3.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.0, 0.0, -3.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    //window decoration
    {
        {-0.05, -0.25, -3.2},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {0.05, -0.25, -3.2},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {-0.05, 1.0, -3.2},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {0.05, 1.0, -3.2},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {4.2, -0.25, -0.05},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {4.2, -0.25, 0.05},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {4.2, 1, -0.05},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {4.2, 1, 0.05},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    // Bottom parapet backWindow
    {
        {-1.0, -0.25, -2.95},
        {0.55, 0.27, 0.07},
        {0.0, 1.0, 0.0}
    },
    {
        {-1.0, -0.25, -3.2},
        {0.55, 0.27, 0.07},
        {0.0, 1.0, 0.0}
    },
    {
        {1.0, -0.25, -2.95},
        {0.55, 0.27, 0.07},
        {0.0, 1.0, 0.0}
    },
    {
        {1.0, -0.25, -3.2},
        {0.55, 0.27, 0.07},
        {0.0, 1.0, 0.0}
    },
    // Top-parapet backWindow
    {
        {-1.0, 1.0, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, -1.0, 0.0}
    },
    {
        {-1.0, 1.0, -3.2},
        {0.9, 0.9, 0.9},
        {0.0, -1.0, 0.0}
    },
    {
        {1.0, 1.0, -3.0},
        {0.9, 0.9, 0.9},
        {0.0, -1.0, 0.0}
    },
    {
        {1.0, 1.0, -3.2},
        {0.9, 0.9, 0.9},
        {0.0, -1.0, 0.0}
    },
    //left parapet backWindow
    {
        {-1.0, -0.25, -3.0},
        {0.9, 0.9, 0.9},
        {1.0, 0.0, 0.0}
    },
    {
        {-1.0, -0.25, -3.2},
        {0.9, 0.9, 0.9},
        {1.0, 0.0, 0.0}
    },
    {
        {-1.0, 1.0, -3.0},
        {0.9, 0.9, 0.9},
        {1.0, 0.0, 0.0}
    },
    {
        {-1.0, 1.0, -3.2},
        {0.9, 0.9, 0.9},
        {1.0, 0.0, 0.0}
    },
    //right parapet backWindow
    {
        {1.0, -0.25, -3.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {1.0, -0.25, -3.2},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {1.0, 1.0, -3.0},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    {
        {1.0, 1.0, -3.2},
        {0.9, 0.9, 0.9},
        {-1.0, 0.0, 0.0}
    },
    //bottom parapet rightWindow
    {
        {3.95, -0.25, -1.0},
        {0.55, 0.27, 0.07},
        {0.0, 1.0, 0.0}
    },
    {
        {3.95, -0.25, 1.0},
        {0.55, 0.27, 0.07},
        {0.0, 1.0, 0.0}
    },
    {
        {4.2, -0.25, -1.0},
        {0.55, 0.27, 0.07},
        {0.0, 1.0, 0.0}
    },
    {
        {4.2, -0.25, 1.0},
        {0.55, 0.27, 0.07},
        {0.0, 1.0, 0.0}
    },
    //top parapet rightWindow
    {
        {4.0, 1.0, -1.0},
        {0.9, 0.9, 0.9},
        {0.0, 1.0, 0.0}
    },
    {
        {4.2, 1.0, -1.0},
        {0.9, 0.9, 0.9},
        {0.0, -1.0, 0.0}
    },
    {
        {4.0, 1.0, 1.0},
        {0.9, 0.9, 0.9},
        {0.0, -1.0, 0.0}
    },
    {
        {4.2, 1.0, 1.0},
        {0.9, 0.9, 0.9},
        {0.0, -1.0, 0.0}
    },
    //right parapet rightWindow
    {
        {4.0, -0.25, -1.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {4.2, -0.25, -1.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {4.0, 1.0, -1.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    {
        {4.2, 1.0, -1.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, 1.0}
    },
    //left parapet rightWindow
    {
        {4.0, -0.25, 1.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, -1.0}
    },
    {
        {4.2, -0.25, 1.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, -1.0}
    },
    {
        {4.0, 1.0, 1.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, -1.0}
    },
    {
        {4.2, 1.0, 1.0},
        {0.9, 0.9, 0.9},
        {0.0, 0.0, -1.0}
    },
};

const unsigned char room[] = {
    //floor
    0, 1, 2,
    1, 2, 3,
    //roof
    4, 5, 6,
    5, 6, 7,
    //left wall
    8, 9, 10,
    9, 10, 11,
    //back wall
    12, 13, 14,
    13, 14, 15,
    14, 15, 16,
    15, 16, 17,
    15, 17, 18,
    17, 18, 19,
    18, 19, 20,
    18, 20, 21,
    20, 21, 22,
    21, 22, 23,
    21, 23, 13,
    23, 13, 12,
    //right wall
    24, 25, 26,
    25, 26, 27,
    26, 27, 28,
    27, 28, 29,
    27, 29, 30,
    29, 30, 31,
    30, 31, 32,
    30, 32, 33,
    32, 33, 34,
    33, 34, 35,
    33, 35, 25,
    35, 25, 24,
    //windowBack decoration
    36, 37, 38,
    37, 38, 39,
    40, 41, 42,
    41, 42, 43,
    44, 45, 46,
    45, 46, 47,
    48, 49, 50,
    49, 50, 51,
    52, 53, 54,
    53, 54, 55,
    56, 57, 58,
    57, 58, 59,
    //windowRight decoration
    60, 61, 62,
    61, 62, 63,
    64, 65, 66,
    65, 66, 67,
    68, 69, 70,
    69, 70, 71,
    72, 73, 74,
    73, 74, 75
};

const Point winBackVertices[] = {
    {
        {-1.0, -0.25, -3.2},
        {0.8, 0.8, 1.0},
        {0.0, 0.0, 1.0}
    },
    {
        {-0.05, -0.25, -3.2},
        {0.8, 0.8, 1.0},
        {0.0, 0.0, 1.0}
    },
    {
        {-1.0, 1.0, -3.2},
        {0.8, 0.8, 1.0},
        {0.0, 0.0, 1.0}
    },
    {
        {-0.05, 1.0, -3.2},
        {0.8, 0.8, 1.0},
        {0.0, 0.0, 1.0}
    },
    {
        {0.05, -0.25, -3.2},
        {0.8, 0.8, 1.0},
        {0.0, 0.0, 1.0}
    },
    {
        {1.0, -0.25, -3.2},
        {0.8, 0.8, 1.0},
        {0.0, 0.0, 1.0}
    },
    {
        {0.05, 1.0, -3.2},
        {0.8, 0.8, 1.0},
        {0.0, 0.0, 1.0}
    },
    {
        {1.0, 1.0, -3.2},
        {0.8, 0.8, 1.0},
        {0.0, 0.0, 1.0}
    },
};

const unsigned char winBack[] = {
    0, 1, 2,
    1, 2, 3,
    4, 5, 6,
    5, 6, 7
};

const Point winRightVertices[] = {
    {
        {4.2, -0.25, -1.0},
        {0.8, 0.8, 1.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.2, -0.25, -0.05},
        {0.8, 0.8, 1.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.2, 1.0, -1.0},
        {0.8, 0.8, 1.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.2, 1.0, -0.05},
        {0.8, 0.8, 1.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.2, -0.25, 1.0},
        {0.8, 0.8, 1.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.2, -0.25, 0.05},
        {0.8, 0.8, 1.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.2, 1.0, 1.0},
        {0.8, 0.8, 1.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {4.2, 1.0, 0.05},
        {0.8, 0.8, 1.0},
        {-1.0, 0.0, 0.0}
    },
};

const unsigned char winRight[] = {
    0, 1, 2,
    1, 2, 3,
    4, 5, 6,
    5, 6, 7
};

const Point topLightVertices[] = {
    {
        {-0.5, 1.4, -0.5},
        {1.0, 1.0, 1.0},
        {0.0, -1.0, 0.0}
    },
    {
        {-0.5, 1.4, 0.5},
        {1.0, 1.0, 1.0},
        {0.0, -1.0, 0.0}
    },
    {
        {0.5, 1.4, -0.5},
        {1.0, 1.0, 1.0},
        {0.0, -1.0, 0.0}
    },
    {
        {0.5, 1.4, 0.5},
        {1.0, 1.0, 1.0},
        {0.0, -1.0, 0.0}
    },
};

const unsigned char topLight[] = {
    0, 1, 2,
    1, 2, 3,
};

const Point tableVertices[] = {
    //table legs
    //left front
    {
        {2.1, -1.5, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {2.3, -1.5, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {2.1, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {2.3, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },

    {
        {2.1, -1.5, 1.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {2.3, -1.5, 1.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {2.1, -0.5, 1.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {2.3, -0.5, 1.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },

    {
        {2.1, -1.5, 2.0},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {2.1, -1.5, 1.8},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {2.1, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {2.1, -0.5, 1.8},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },

    {
        {2.3, -1.5, 2.0},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {2.3, -1.5, 1.8},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {2.3, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {2.3, -0.5, 1.8},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    //right front
    {
        {3.1, -1.5, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {3.3, -1.5, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {3.1, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {3.3, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },

    {
        {3.1, -1.5, 1.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {3.3, -1.5, 1.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {3.1, -0.5, 1.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {3.3, -0.5, 1.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },

    {
        {3.1, -1.5, 2.0},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {3.1, -1.5, 1.8},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {3.1, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {3.1, -0.5, 1.8},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },

    {
        {3.3, -1.5, 2.0},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {3.3, -1.5, 1.8},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {3.3, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {3.3, -0.5, 1.8},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    //left back
    {
        {2.1, -1.5, -0.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {2.3, -1.5, -0.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {2.1, -0.5, -0.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {2.3, -0.5, -0.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },

    {
        {2.1, -1.5, -1.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {2.3, -1.5, -1.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {2.1, -0.5, -1.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {2.3, -0.5, -1.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },

    {
        {2.1, -1.5, -1.0},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {2.1, -1.5, -0.8},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {2.1, -0.5, -1.0},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {2.1, -0.5, -0.8},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },

    {
        {2.3, -1.5, -1.0},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {2.3, -1.5, -0.8},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {2.3, -0.5, -1.0},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {2.3, -0.5, -0.8},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    //right back
    {
        {3.1, -1.5, -0.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {3.3, -1.5, -0.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {3.1, -0.5, -0.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {3.3, -0.5, -0.8},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },

    {
        {3.1, -1.5, -1.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {3.3, -1.5, -1.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {3.1, -0.5, -1.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {3.3, -0.5, -1.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },

    {
        {3.1, -1.5, -1.0},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {3.1, -1.5, -0.8},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {3.1, -0.5, -1.0},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {3.1, -0.5, -0.8},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },

    {
        {3.3, -1.5, -1.0},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {3.3, -1.5, -0.8},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {3.3, -0.5, -1.0},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {3.3, -0.5, -0.8},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    //desk
    //bottom desk
    {
        {2.0, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 1.0, 0.0}
    },
    {
        {3.4, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 1.0, 0.0}
    },
    {
        {2.0, -0.4, -1.1},
        {0.72, 0.54, 0.0},
        {0.0, 1.0, 0.0}
    },
    {
        {3.4, -0.4, -1.1},
        {0.72, 0.54, 0.0},
        {0.0, 1.0, 0.0}
    },

    //top desk
    {
        {2.0, -0.4, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, -1.0, 0.0}
    },
    {
        {3.4, -0.4, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, -1.0, 0.0}
    },
    {
        {2.0, -0.4, -1.1},
        {0.72, 0.54, 0.0},
        {0.0, -1.0, 0.0}
    },
    {
        {3.4, -0.4, -1.1},
        {0.72, 0.54, 0.0},
        {0.0, -1.0, 0.0}
    },
    //front desk side
    {
        {2.0, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {3.4, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {2.0, -0.4, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    {
        {3.4, -0.4, 2.0},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, 1.0}
    },
    //back desk side
    {
        {2.0, -0.5, -1.1},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {3.4, -0.5, -1.1},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {2.0, -0.4, -1.1},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    {
        {3.4, -0.4, -1.1},
        {0.72, 0.54, 0.0},
        {0.0, 0.0, -1.0}
    },
    //left desk side
    {
        {2.0, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {2.0, -0.5, -1.1},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {2.0, -0.4, 2.0},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    {
        {2.0, -0.4, -1.1},
        {0.72, 0.54, 0.0},
        {-1.0, 0.0, 0.0}
    },
    //right desk side
    {
        {3.4, -0.5, 2.0},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {3.4, -0.5, -1.1},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {3.4, -0.4, 2.0},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
    {
        {3.4, -0.4, -1.1},
        {0.72, 0.54, 0.0},
        {1.0, 0.0, 0.0}
    },
};

const unsigned char table[] = {
    //left front
    0, 1, 2,
    1, 2, 3,

    4, 5, 6,
    5, 6, 7,

    8, 9, 10,
    9, 10, 11,

    12, 13, 14,
    13, 14, 15,

    //right front
    16, 17, 18,
    17, 18, 19,

    20, 21, 22,
    21, 22, 23,

    24, 25, 26,
    25, 26, 27,

    28, 29, 30,
    29, 30, 31,

    //left back
    32, 33, 34,
    33, 34, 35,

    36, 37, 38,
    37, 38, 39,

    40, 41, 42,
    41, 42, 43,

    44, 45, 46,
    45, 46, 47,

    //right back
    48, 49, 50,
    49, 50, 51,

    52, 53, 54,
    53, 54, 55,

    56, 57, 58,
    57, 58, 59,

    60, 61, 62,
    61, 62, 63,

    //desk top
    64, 65, 66,
    65, 66, 67,

    //desk bottom
    68, 69, 70,
    69, 70, 71,

    //desk front
    72, 73, 74,
    73, 74, 75,

    //desk back
    76, 77, 78,
    77, 78, 79,

    //desk left
    80, 81, 82,
    81, 82, 83,

    //desk right
    84, 85, 86,
    85, 86, 87,
};

#endif	/* PGR_MODEL_H */

