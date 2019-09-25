//
//  Tesseract.cpp
//  bulletTest
//
//  Created by Zihao Chen on 11/27/18.
//

#include "Tesseract.hpp"

void Tesseract::init(){
    vertices[0] = ofVec4f(-1, -1, -1, 1);
    vertices[1] = ofVec4f(1, -1, -1, 1);
    vertices[2] = ofVec4f(1, 1, -1, 1);
    vertices[3] = ofVec4f(-1, 1, -1, 1);
    vertices[4] = ofVec4f(-1, -1, 1, 1);
    vertices[5] = ofVec4f(1, -1, 1, 1);
    vertices[6] = ofVec4f(1, 1, 1, 1);
    vertices[7] = ofVec4f(-1, 1, 1, 1);
    vertices[8] = ofVec4f(-1, -1, -1, -1);
    vertices[9] = ofVec4f(1, -1, -1, -1);
    vertices[10] = ofVec4f(1, 1, -1, -1);
    vertices[11] = ofVec4f(-1, 1, -1, -1);
    vertices[12] = ofVec4f(-1, -1, 1, -1);
    vertices[13] = ofVec4f(1, -1, 1, -1);
    vertices[14] = ofVec4f(1, 1, 1, -1);
    vertices[15] = ofVec4f(-1, 1, 1, -1);
    updateProjection(0,0,0,0,0,0);
};

void Tesseract::updateProjection(int isXW, int isYW, int isZW, int isXY, int isXZ, int isYZ){
    speed = 0.005;
    if(isXW == 1) angleXW += speed;
    else if(isXW == -1) angleXW -= speed;
    if(isYW == 1) angleYW += speed;
    else if(isYW == -1) angleYW -= speed;
    if(isZW == 1) angleZW += speed;
    else if(isZW == -1) angleZW -= speed;
    if(isXY == 1) angleXY += speed;
    else if(isXY == -1) angleXY -= speed;
    if(isXZ == 1) angleXZ += speed;
    else if(isXZ == -1) angleXZ -= speed;
    if(isYZ == 1) angleYZ += speed;
    else if(isYZ == -1) angleYZ -= speed;
    
    float rXW[4][4] = {
        {cos(angleXW), 0, 0, sin(angleXW)},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {-sin(angleXW), 0, 0, cos(angleXW)}
    };
    float** rotationXW = new float*[4];
    for (int i = 0; i < 4; i++)
    {
        rotationXW[i] = new float[4];
        for (int j = 0; j < 4; j++) {
            rotationXW[i][j] = rXW[i][j];
        }
        
    }
    float rXY[4][4] = {
        {cos(angleXY), sin(angleXY), 0, 0},
        {-sin(angleXY), cos(angleXY), 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
    float** rotationXY = new float*[4];
    for (int i = 0; i < 4; i++)
    {
        rotationXY[i] = new float[4];
        for (int j = 0; j < 4; j++) {
            rotationXY[i][j] = rXY[i][j];
        }
        
    }
    float rXZ[4][4] = {
        {cos(angleXZ), 0, -sin(angleXZ), 0},
        {0, 1, 0, 0},
        {sin(angleXZ), 0, cos(angleXZ), 0},
        {0, 0, 0, 1}
    };
    float** rotationXZ = new float*[4];
    for (int i = 0; i < 4; i++)
    {
        rotationXZ[i] = new float[4];
        for (int j = 0; j < 4; j++) {
            rotationXZ[i][j] = rXZ[i][j];
        }
        
    }
    float rYW[4][4] = {
        {1, 0, 0, 0},
        {0, cos(angleYW), 0, -sin(angleYW)},
        {0, 0, 1, 0},
        {0, sin(angleYW), 0, cos(angleYW)}
    };
    float** rotationYW = new float*[4];
    for (int i = 0; i < 4; i++)
    {
        rotationYW[i] = new float[4];
        for (int j = 0; j < 4; j++) {
            rotationYW[i][j] = rYW[i][j];
        }
        
    }
    float rZW[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, cos(angleZW), -sin(angleZW)},
        {0, 0, sin(angleZW), cos(angleZW)}
    };
    float** rotationZW = new float*[4];
    for (int i = 0; i < 4; i++)
    {
        rotationZW[i] = new float[4];
        for (int j = 0; j < 4; j++) {
            rotationZW[i][j] = rZW[i][j];
        }
        
    }
    float rYZ[4][4] = {
        {1, 0, 0, 0},
        {0, cos(angleYZ), sin(angleYZ), 0},
        {0, -sin(angleYZ), cos(angleYZ), 0},
        {0, 0, 0, 1}
    };
    float** rotationYZ = new float*[4];
    for (int i = 0; i < 4; i++)
    {
        rotationYZ[i] = new float[4];
        for (int j = 0; j < 4; j++) {
            rotationYZ[i][j] = rYZ[i][j];
        }
        
    }
    
    
    for (int i = 0; i < VERTICES; i++) {
        ofVec4f v = vertices[i];
        v = matmul(rotationXW, 4, 4, v, true);
        v = matmul(rotationYW, 4, 4, v, true);
        v = matmul(rotationXY, 4, 4, v, true);
        v = matmul(rotationXZ, 4, 4, v, true);
        v = matmul(rotationZW, 4, 4, v, true);
        v = matmul(rotationYZ, 4, 4, v, true);
        float distance = 8;
        float w = 1 / (distance - v.w);
        float p[3][4] = {
            {w, 0, 0, 0},
            {0, w, 0, 0},
            {0, 0, w, 0}
        };
        
        float** projection = new float*[3];
        for (int i = 0; i < 3; i++)
        {
            projection[i] = new float[4];
            for (int j = 0; j < 4; j++) {
                projection[i][j] = p[i][j];
            }
            
        }
        ofVec3f projection3D = matmul(projection, 4, 3, v);
        projected[i] = projection3D  * SCALE;
        
        for (int i = 0; i < 3; i++)
        {
            delete [] projection[i];
        }
        delete [] projection;
    }
    
    for (int i = 0; i < 4; i++)
    {
        delete [] rotationXW[i];
        delete [] rotationXY[i];
        delete [] rotationXZ[i];
        delete [] rotationYW[i];
        delete [] rotationZW[i];
        delete [] rotationYZ[i];
    }
    delete [] rotationXW;
    delete [] rotationXY;
    delete [] rotationXZ;
    delete [] rotationYW;
    delete [] rotationZW;
    delete [] rotationYZ;
    
};

void Tesseract::drawTest(){
//    for (int i = 0; i < VERTICES; i++) {
////        ofNoFill();
//                ofSetColor(255);
//                ofDrawSphere(projected[i], 1);
//    }
//    ofSetLineWidth(1.0f);

    for (int i = 0; i < 4; i++) {
//        cout<<i<<endl;
        ofDrawLine(projected[i], projected[(i+1) % 4]);

        //        cout<< i << "     : " << (i+1) % 4 <<endl;
        ofDrawLine(projected[i + 4], projected[((i+1) % 4) + 4]);
        ofDrawLine(projected[i], projected[i + 4]);
    }
    
    for (int i = 0; i < 4; i++) {
//        cout<<i<<endl;
        ofDrawLine(projected[i + 8], projected[(i+1) % 4 + 8]);
        ofDrawLine(projected[i + 4 + 8], projected[((i+1) % 4) + 4 + 8]);
        ofDrawLine(projected[i + 8], projected[i+4 + 8]);
    }
    
    for (int i = 0; i < 8; i++) {
//        cout<<i<<endl;
        ofDrawLine(projected[i], projected[i + 8]);
    }
}
