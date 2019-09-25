//
//  Floater.hpp
//  4dbox_mesh
//
//  Created by Zihao Chen on 11/28/18.
//

#ifndef Floater_hpp
#define Floater_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxBullet.h"
#include "Tesseract.hpp"
#include "Matrix.hpp"

class Floater{
    
public:
    void init(ofCamera &cam);
    
    ofxBulletWorldRigid world;
    unique_ptr<ofxBulletCustomShape> bounds_shape;
    vector<unique_ptr<ofxBulletSphere>> spheres;
    
};


#endif /* Floater_hpp */
