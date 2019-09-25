#pragma once

#include <stdio.h>
#include "ofMain.h"
float** vecToMatrix(ofVec4f v);
ofVec3f matrixToVec3f(float** m);

ofVec4f matrixToVec4f(float** m);


void logMatrix(float** m, int c, int r);


ofVec3f matmul(float** a, int cA, int rA, ofVec4f b);

ofVec4f matmul(float** a, int cA, int rA, ofVec4f b, bool fourth);

float** matmul(float** a, float** b, int cA, int rA, int cB, int rB);
