#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
//    ofHideCursor();
    isShaderDirty = true;
    isScreenDirty = false;
    ofSetFrameRate(60);
    ofBackground(0);
    ofSetWindowTitle("4d mesh");
//    ofEnableSmoothing();
//    ofEnableAntiAliasing();
//    ofSetVerticalSync(true);
    ofEnableDepthTest();
    camE.setDistance(1000);
//    ofSetBackgroundAuto(false);
//    ofSetSphereResolution(3);
    
    // Create Tesseract
    
    tess.init();
    
    // Create Meshes
    
    for(int i = 0; i < WALLNUM; i++){
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        mesh.enableColors();
        walls.push_back(mesh);
    }
    
    addFace(walls[0], tess.projected[0], tess.projected[1], tess.projected[2], tess.projected[3] , tess.projected[4], tess.projected[5], tess.projected[6], tess.projected[7]);
    addFace(walls[1], tess.projected[8], tess.projected[9], tess.projected[10], tess.projected[11] , tess.projected[12], tess.projected[13], tess.projected[14], tess.projected[15]);
    addFace(walls[2], tess.projected[0], tess.projected[1], tess.projected[2], tess.projected[3] , tess.projected[8], tess.projected[9], tess.projected[10], tess.projected[11]);
    addFace(walls[3], tess.projected[4], tess.projected[5], tess.projected[6], tess.projected[7] , tess.projected[12], tess.projected[13], tess.projected[14], tess.projected[15]);
    addFace(walls[4], tess.projected[0], tess.projected[1], tess.projected[5], tess.projected[4] , tess.projected[8], tess.projected[9], tess.projected[13], tess.projected[12]);
    addFace(walls[5], tess.projected[2], tess.projected[3], tess.projected[7], tess.projected[6] , tess.projected[10], tess.projected[11], tess.projected[15], tess.projected[14]);
    addFace(walls[6], tess.projected[2], tess.projected[6], tess.projected[14], tess.projected[10] , tess.projected[1], tess.projected[5], tess.projected[13], tess.projected[9]);
    addFace(walls[7], tess.projected[3], tess.projected[7], tess.projected[4], tess.projected[0] , tess.projected[11], tess.projected[15], tess.projected[12], tess.projected[8]);
    
    // Create cube
    
    int ballzNum = 600;
    cube = new Cube[8];
    cube[0].init(&this->camE, walls[0], ballzNum, ofColor(224,118,102));
    cube[1].init(&this->camE, walls[1], ballzNum, ofColor(255,0,100));
    cube[2].init(&this->camE, walls[2], ballzNum, ofColor(150,0,255));
    cube[3].init(&this->camE, walls[3], ballzNum, ofColor(20,10,255));
    cube[4].init(&this->camE, walls[4], ballzNum, ofColor(0,120,255));
    cube[5].init(&this->camE, walls[5], ballzNum, ofColor(0,255,249));
    cube[6].init(&this->camE, walls[6], ballzNum, ofColor(116,238,21));
    cube[7].init(&this->camE, walls[7], ballzNum, ofColor(255, 128, 0));
    
    // shader
    // final shader
    
    // bloom shader
    std::vector<float> coefficients;
    coefficients.clear();
    coefficients = createGaussianWeights(50, 0.08f);
    blurShader.load("shader/vfx/PassThru.vert", "shader/vfx/Blur.frag");
    blurShader.begin();
    blurShader.setUniform1i("blurRes", 50);
    blurShader.setUniform1f("sampleStep", 0.5);
    blurShader.setUniform1fv("coefficients", coefficients.data(), coefficients.size());
//    blurShader.setUniform1i("preShrink", 2);
    blurShader.end();
    bloom.load("shader/bloom.vert", "shader/bloom.frag");
    // buffer
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA16F);
    onePassFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA16F);
    twoPassFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA16F);
    finalfbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA16F);
    
    // Shader control
    
    isXW = isYW = isZW = isXY = isXZ = isYZ = 0;
    prevW = 0;
    currW = 0;

    // Serial input
    
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    int baud = 9600;
    serial.setup(0, baud); //open the first device
    input = 0;
    prevInput = 0;
    
    input = 0b00000000;
    
    
    bgm.load("synth.wav");
    blink.load("blink.mp3");
    bgm.setLoop(true);
    bgm.setVolume(1.f);
    blink.setVolume(0.1f);
    bgm.play();
}

//--------------------------------------------------------------
void ofApp::update(){
//    input = serial.readByte();
    if(isScreenDirty){
        isScreenDirty = false;
        fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA16F);
        onePassFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA16F);
        twoPassFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA16F);
        finalfbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA16F);
    }
    if (isShaderDirty){
        shader.load("shader/shader.vert", "shader/shader.frag");
        isShaderDirty = false;
    }
    while(serial.readBytes(&input, 1) > 0);
//    cout<<std::bitset<8>(input)<<endl;
    
    

    if(prevInput != input){
        change = true;
        prevInput = input;
    }
    
    if(change){
        prevW = currW;
        isXW = isYW = isZW = isXY = isXZ = isYZ = 0;
        currW = 0;
        if(input!=0){
            if((input & 0b11110000) == 0b11110000) {isXY = 1;currW = 0;}
            else if((input & 0b01010000) == 0b01010000) {isXY = -1;currW = 0;}
            if((input & 0b11001100) == 0b11001100) {isXZ = 1;currW = 0;}
            else if((input & 0b01000100) == 0b01000100) {isXZ = -1;currW = 0;}
            if((input & 0b00111100) == 0b00111100) {isYZ = 1;currW = 0;}
            else if((input & 0b00010100) == 0b00010100) {isYZ = -1;currW = 0;}
            if((input & 0b11000011) == 0b11000011) { isXW = 1; currW = 1; }
            else if((input & 0b01000001) == 0b01000001) {isXW = -1; currW = 1;}
            if((input & 0b00110011) == 0b00110011) {isYW = 1;currW = 2;}
            else if((input & 0b00010001) == 0b00010001) {isYW = -1;currW = 2;}
            if((input & 0b00001111) == 0b00001111) {isZW = 1;currW = 3;}
            else if((input & 0b00000101) == 0b00000101) {isZW = -1;currW = 3;}
        }
        changePoint = ofGetElapsedTimef();
        change = false;
    }
    
    prevBingo = bingo;
//    cout<<abs(tess.angleYW - 1.45)<<endl;
    if(abs(fmodf(tess.angleYW, PI) - 1.45) < 0.02){
        bingo = true;
    }else{
        bingo = false;
    }
    if(abs(fmodf(tess.angleXW, PI) - 2.18) < 0.02){
        bingo = true;
    }else{
        bingo = false;
    }
    if(abs(fmodf(tess.angleZW, PI) - 0.76) < 0.02){
        bingo = true;
    }else{
        bingo = false;
    }
    
    if(bingo && !prevBingo){
        bingoPoint = ofGetElapsedTimef();
        blink.play();
    }
    
    angle += 0.005;
    
    tess.updateProjection(isXW, isYW, isZW, isXY, isXZ, isYZ);
    
    for(int i = 0; i < WALLNUM; i++){
        walls[i].clearVertices();
        walls[i].clearIndices();
    }
    addFace(walls[0], tess.projected[0], tess.projected[1], tess.projected[2], tess.projected[3] , tess.projected[4], tess.projected[5], tess.projected[6], tess.projected[7]);
    addFace(walls[1], tess.projected[8], tess.projected[9], tess.projected[10], tess.projected[11] , tess.projected[12], tess.projected[13], tess.projected[14], tess.projected[15]);
    addFace(walls[2], tess.projected[0], tess.projected[1], tess.projected[2], tess.projected[3] , tess.projected[8], tess.projected[9], tess.projected[10], tess.projected[11]);
    addFace(walls[3], tess.projected[4], tess.projected[5], tess.projected[6], tess.projected[7] , tess.projected[12], tess.projected[13], tess.projected[14], tess.projected[15]);
    addFace(walls[4], tess.projected[0], tess.projected[1], tess.projected[5], tess.projected[4] , tess.projected[8], tess.projected[9], tess.projected[13], tess.projected[12]);
    addFace(walls[5], tess.projected[2], tess.projected[3], tess.projected[7], tess.projected[6] , tess.projected[10], tess.projected[11], tess.projected[15], tess.projected[14]);
    addFace(walls[6], tess.projected[2], tess.projected[6], tess.projected[14], tess.projected[10] , tess.projected[1], tess.projected[5], tess.projected[13], tess.projected[9]);
    addFace(walls[7], tess.projected[3], tess.projected[7], tess.projected[4], tess.projected[0] , tess.projected[11], tess.projected[15], tess.projected[12], tess.projected[8]);
    
    
    for(int i = 0; i < 8; i++){
        cube[i].update(walls[i]);
    }
    
    fbo.begin();
    ofClear(0, 0, 0, 0);
    camE.begin();
    for(int i = 0; i < 8; i++){
        cube[i].draw(&this->camE);
    }
    ofSetColor(255, 100);
    tess.drawTest();
    
        for(int i = 0; i < 8; i++){
            walls[i].draw();
        }

    camE.end();
    fbo.end();
    bloomE();
}

//--------------------------------------------------------------
void ofApp::draw(){
//    ofDrawCircle(500, 500, 0, 100);
//    std::stringstream strm;
//    strm << "fps: " << ofGetFrameRate();
//    ofSetWindowTitle(strm.str());
//    std::stringstream sttm;
//    sttm << "time: " << ofGetElapsedTimef();
//    ofSetColor(255);
//    ofDrawBitmapString(strm.str(), 100, 100);
//    ofDrawBitmapString(sttm.str(), 100, 120);
//
    
    
    this->shader.begin();
    this->shader.setUniformTexture("maskTex", finalfbo.getTexture(), 1 );
    this->shader.setUniform1f("angle", angle);
    this->shader.setUniform1i("prevW", prevW);
    this->shader.setUniform1f("time", ofGetElapsedTimef());
    this->shader.setUniform1f("changePoint", changePoint);
    this->shader.setUniform2f("resolution", ofGetWidth()
                              , ofGetHeight());
    this->shader.setUniform1f("bingoPoint", bingoPoint);
    this->shader.setUniform1i("isXW", isXW);
    this->shader.setUniform1i("isYW", isYW);
    this->shader.setUniform1i("isZW", isZW);
    
    finalfbo.draw(0,0);

    this->shader.end();
}


void ofApp::bloomE(){
    
    // X方向のガウシアンブラー
    onePassFbo.begin();
    ofClear(0, 0, 0, 0);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    blurShader.begin();
    
    blurShader.setUniform1i("isHorizontal", 0);
    fbo.draw(0, 0);
    
    blurShader.end();
    
    onePassFbo.end();
    //
    // Y方向のガウシアンブラー
    twoPassFbo.begin();
    ofClear(0, 0, 0, 0);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    blurShader.begin();
    blurShader.setUniform1i("isHorizontal", 1);
    
    onePassFbo.draw(0, 0);
    blurShader.end();
    
    twoPassFbo.end();
    
    //     ガウシアンブラーと元の描画内容を加算合成z
    finalfbo.begin();
    ofClear(0, 0, 0, 0);
    bloom.begin();
    bloom.setUniformTexture("tex", fbo.getTexture(), 0);
    bloom.setUniformTexture("blur", twoPassFbo.getTexture(), 1);

    
    fbo.draw(0, 0);
    
    bloom.end();
    finalfbo.end();
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'q') input |= 0b00110011;
    if (key == 'e') input |= 0b00001111;
    if (key == 'w') input |= 0b11000011;
    if (key == 'x') input |= 0b11110000;
    if (key == 'z') input |= 0b11001100;
    if (key == 'c') input |= 0b00111100;
    if (key == 'a') input = 0b00000000;
    if (key == ' '){bingoPoint = ofGetElapsedTimef();blink.play();}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key) {
        case 's':
            isShaderDirty = true;
            // mark the shader as dirty - this will reload the shader.
            break;
        case 'f':
            ofToggleFullscreen();
            isScreenDirty = true;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d, ofVec3f e, ofVec3f f, ofVec3f g, ofVec3f h) {
    mesh.addVertex(a);
    mesh.addVertex(b);
    mesh.addVertex(c);
    mesh.addVertex(d);
    mesh.addVertex(e);
    mesh.addVertex(f);
    mesh.addVertex(g);
    mesh.addVertex(h);
    //1
    mesh.addIndex(0); //connect the first vertex we made, v0                 
    mesh.addIndex(1); //to v1
    mesh.addIndex(2); //to v2 to complete the face
    mesh.addIndex(0); //now start a new face beginning with v1
    mesh.addIndex(2); //that is connected to v2
    mesh.addIndex(3); //and we complete the face with v3
    //2
    mesh.addIndex(4); //now start a new face beginning with v1
    mesh.addIndex(5); //that is connected to v2
    mesh.addIndex(6); //and we complete the face with v3
    mesh.addIndex(4); //now start a new face beginning with v1
    mesh.addIndex(6); //that is connected to v2
    mesh.addIndex(7); //and we complete the face with v3
    //3
    mesh.addIndex(3); //now start a new face beginning with v1
    mesh.addIndex(0); //that is connected to v2
    mesh.addIndex(4); //and we complete the face with v3
    mesh.addIndex(3); //now start a new face beginning with v1
    mesh.addIndex(4); //that is connected to v2
    mesh.addIndex(7); //and we complete the face with v3
    //4
    mesh.addIndex(0); //now start a new face beginning with v1
    mesh.addIndex(1); //that is connected to v2
    mesh.addIndex(5); //and we complete the face with v3
    mesh.addIndex(0); //now start a new face beginning with v1
    mesh.addIndex(5); //that is connected to v2
    mesh.addIndex(4); //and we complete the face with v3
    //5
    mesh.addIndex(1); //now start a new face beginning with v1
    mesh.addIndex(2); //that is connected to v2
    mesh.addIndex(6); //and we complete the face with v3
    mesh.addIndex(1); //now start a new face beginning with v1
    mesh.addIndex(6); //that is connected to v2
    mesh.addIndex(5); //and we complete the face with v3
    //6
    mesh.addIndex(2); //now start a new face beginning with v1
    mesh.addIndex(3); //that is connected to v2
    mesh.addIndex(7); //and we complete the face with v3
    mesh.addIndex(2); //now start a new face beginning with v1
    mesh.addIndex(7); //that is connected to v2
    mesh.addIndex(6); //and we complete the face with v3
}

static std::vector<float> createGaussianWeights(int radius, float variance) {
    
    int rowSize = 1 + radius;
    float sum = 0.f;
    std::vector<float> row;
    
    for (int i = 0; i < rowSize; i++) {
        
        float x = ofMap(i, 0, radius, 0, 1.2f);
        float w = Gaussian(x, 0, variance);
        
        row.push_back(w);
        
        if (i != 0) w *= 2.f;
        sum += w;
    }
    
    for (int i = 0; i < row.size(); i++) {
        row[i] /= sum;
    }
    
    return row;
}

static float Gaussian(float x, float mean, float variance) {
    x -= mean;
    return (1. / sqrt(TWO_PI * variance)) * exp(-(x * x) / (2. * variance));
}
