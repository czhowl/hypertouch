//
//  Cube.cpp
//  4dbox_mesh
//
//  Created by Zihao Chen on 11/28/18.
//

#include "Cube.hpp"

void Cube::init(ofCamera *cam, ofMesh& box, int _ballNum, ofColor _color){
    
    matrices.resize(_ballNum);
    buffer.allocate();
    buffer.bind(GL_TEXTURE_BUFFER);
    buffer.setData(matrices,GL_STREAM_DRAW);
    tex.allocateAsBufferTexture(buffer,GL_RGBA32F);
    
    shader.load("vert.glsl","frag.glsl");
    
    ofSpherePrimitive tmpSphere;
    // set the size to be 2 units.
    tmpSphere.set(3, 15);
    mesh = ofVboMesh(tmpSphere.getMesh());
    mesh.enableColors();
    
    mesh.getColors().resize(matrices.size());
    for(size_t i=0;i<mesh.getColors().size();i++){
        mesh.getColors()[i] = _color;
    }
    mesh.getVbo().setAttributeDivisor(ofShader::COLOR_ATTRIBUTE,1);
    
    color = _color;
    ballNum = _ballNum;
    
    bounds_shape_ptr = new ofxBulletCustomShape();
    world.setup();
    world.setCamera(cam);
    world.setGravity(glm::vec3(0.f, 5.f, 0.f));
//    world.setGravity(glm::vec3(0.f, -50.f, 0.f));
    
    vector< glm::vec3 >& verticies = box.getVertices();
    btVector3 centroid = btVector3(0,0,0);
    for(int i = 0; i < verticies.size(); i++) {
        box.addColor(ofFloatColor(color.r,color.g,color.b, 0.02));;
        btVector3 tempVec = btVector3(verticies[i].x, verticies[i].y, verticies[i].z);
        centroid += tempVec;
    }
    centroid /= (float)verticies.size();
    center = ofVec3f(centroid.getX(), centroid.getY(), centroid.getZ());
    
    const vector< glm::vec3 >& normals = box.getFaceNormals();
    float distance[6];
    distance[0] = center.dot(normals[0]) - ((ofVec3f)verticies[0]).dot(normals[0]);
    distance[1] = center.dot(normals[6]) - ((ofVec3f)verticies[6]).dot(normals[6]);
    distance[2] = center.dot(normals[12]) - ((ofVec3f)verticies[0]).dot(normals[12]);
    distance[3] = center.dot(normals[18]) - ((ofVec3f)verticies[0]).dot(normals[18]);
    distance[4] = center.dot(normals[24]) - ((ofVec3f)verticies[6]).dot(normals[24]);
    distance[5] = center.dot(normals[30]) - ((ofVec3f)verticies[6]).dot(normals[30]);
    float smallest = abs(distance[0]);
    for(int i = 1; i < 6; i++){
        if(smallest > abs(distance[i])){
            smallest = abs(distance[i]);
        }
    }
    smallest -= 10;
    
    bulletMesh = shared_ptr< ofxBulletTriMeshShape >( new ofxBulletTriMeshShape() );
    bulletMesh->create( world.world, box, ofVec3f(0,0,0), 0.f, ofVec3f(-10000, -10000, -10000), ofVec3f(10000,10000,10000) );
    bulletMesh->add();
    bulletMesh->enableKinematic();
    bulletMesh->setActivationState( DISABLE_DEACTIVATION );
    // ballz

    for (int i = 0; i < ballNum; i++) {
        float life = ofRandom(500,1000);
        sLife.push_back(life);
        unique_ptr<ofxBulletSphere> sphere(new ofxBulletSphere);
        float size = 10;
        sphere->create(world.world, center + ofVec3f(ofRandom(-smallest, smallest), ofRandom(-smallest, smallest), ofRandom(-smallest, smallest)), 1, size);
        sphere->setRestitution(1.0);
        sphere->add();
        spheres.push_back(move(sphere));
    }
    for (int i = 0; i < spheres.size(); i++) {
        mesh.addVertex(spheres[i]->getPosition());
        float life = sLife[i]/1000;
        ofFloatColor colorA = ofFloatColor(color.r, color.g, color.b, life);
        mesh.addColor(colorA);
    }
};

void Cube::update(ofMesh box){
    vector< glm::vec3 >& verticies = box.getVertices();
    btVector3 centroid = btVector3(0,0,0);
    for(int i = 0; i < verticies.size(); i++) {
        btVector3 tempVec = btVector3(verticies[i].x, verticies[i].y, verticies[i].z);
        centroid += tempVec;
    }
    centroid /= (float)verticies.size();
    center = ofVec3f(centroid.getX(), centroid.getY(), centroid.getZ());
    
    bulletMesh->updateMesh( world.world, box);
    
    const vector< glm::vec3 >& normals = box.getFaceNormals();
    float distance[6];
    distance[0] = center.dot(normals[0]) - ((ofVec3f)verticies[0]).dot(normals[0]);
    distance[1] = center.dot(normals[6]) - ((ofVec3f)verticies[6]).dot(normals[6]);
    distance[2] = center.dot(normals[12]) - ((ofVec3f)verticies[0]).dot(normals[12]);
    distance[3] = center.dot(normals[18]) - ((ofVec3f)verticies[0]).dot(normals[18]);
    distance[4] = center.dot(normals[24]) - ((ofVec3f)verticies[6]).dot(normals[24]);
    distance[5] = center.dot(normals[30]) - ((ofVec3f)verticies[6]).dot(normals[30]);
    float smallest = abs(distance[0]);
    for(int i = 1; i < 6; i++){
        if(smallest > abs(distance[i])){
            smallest = abs(distance[i]);
        }
    }
    smallest -= 10;
    
    for (int i = 0; i < this->spheres.size(); i++) {

        for (int j = i + 1; j < this->spheres.size(); j++) {

            float distance = ((ofVec3f)spheres[i]->getPosition()).distance(spheres[j]->getPosition());
            if (distance < 80) {
                ofVec3f diff = (spheres[i]->getPosition() - spheres[j]->getPosition()) * 0.1;
                spheres[i]->applyCentralForce(diff);
                spheres[j]->applyCentralForce(-diff);
            }
        }

        float distance = ((ofVec3f)spheres[i]->getPosition()).distance(center);
        if(sLife[i] < 0){
            sLife[i] = ofRandom(500,1000);
            spheres[i].reset(new ofxBulletSphere());
            float size = 5;
            spheres[i]->create(world.world, center + ofVec3f(ofRandom(-smallest, smallest), ofRandom(-smallest, smallest), ofRandom(-smallest, smallest)), 1, size);
            spheres[i]->setRestitution(1.0);
            spheres[i]->add();
        }
        sLife[i]--;
    }
    
    world.update();
    
    for(size_t i=0;i<matrices.size();i++){
        ofNode node;
        node.setPosition(spheres[i]->getPosition());
        matrices[i] = node.getLocalTransformMatrix();
        mesh.getColors()[i] = ofFloatColor(color.r, color.g, color.b, sLife[i]/1000);
    }
    mesh.getVbo().setAttributeDivisor(ofShader::COLOR_ATTRIBUTE,1);
    buffer.updateData(0,matrices);
}

void Cube::draw(ofCamera *cam){
    shader.begin();
    shader.setUniformTexture("tex",tex,0);
    mesh.drawInstanced(OF_MESH_FILL,matrices.size());
    shader.end();
}
