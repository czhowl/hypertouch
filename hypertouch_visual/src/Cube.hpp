//
//  Cube.hpp
//  4dbox_mesh
//
//  Created by Zihao Chen on 11/28/18.
//

#ifndef Cube_hpp
#define Cube_hpp

#include "ofMain.h"
#include "ofxBullet.h"
#include "Tesseract.hpp"
#include "Matrix.hpp"

class Cube{
    
public:
    void init(ofCamera *cam, ofMesh& walls, int _ballNum, ofColor _color);
    
    void update(ofMesh walls);
    void draw(ofCamera *cam);
    
    ofxBulletWorldRigid world;
    unique_ptr<ofxBulletCustomShape> bounds_shape;
    vector<unique_ptr<ofxBulletSphere>> spheres;
    vector<float> sLife;
    int ballNum;
    ofFloatColor color;
    ofVec3f center;
    ofxBulletCustomShape* bounds_shape_ptr;
    
    shared_ptr<ofxBulletTriMeshShape> bulletMesh;
    
    ofShader shader;
    ofVboMesh mesh;
    ofTexture tex;
    ofBufferObject buffer;
    vector<ofMatrix4x4> matrices;
};

#endif /* Cube_hpp */
