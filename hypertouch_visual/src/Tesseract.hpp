#pragma once

#define VERTICES 16
#define SCALE 1400

#include <stdio.h>
#include "ofMain.h"
#include "Matrix.hpp"

class Tesseract{
    
public:
    void init();
    void updateProjection(int isXW, int isYW, int isZW, int isXY, int isXZ, int isYZ);
    void drawTest();
    
    ofVec4f vertices[VERTICES];
    ofVec3f projected[VERTICES];
    
    float angleXW = 0;
    float angleXY = 0;
    float angleXZ = 0;
    float angleYW = 0;
    float angleZW = 0;
    float angleYZ = 0;
    
    float speed = 0;
};
