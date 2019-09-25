#pragma once

#include "ofMain.h"
#include "ofxBullet.h"
#include "Cube.hpp"
//#include "ofxPostProcessing.h"
#include "ofxDeferredShading.h"

#define WALLNUM 8
#define BAUD 9600
//using namespace ofxDeferred;
static std::vector<float> createGaussianWeights(int radius, float variance);
static float Gaussian(float x, float mean, float variance);
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void bloomE();
    
    
    void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c);
    void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d);
    void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d, ofVec3f e, ofVec3f f, ofVec3f g, ofVec3f h);

    ofEasyCam camE;

//    Cube cube[8];
    Cube* cube;
    Tesseract tess;
    
    vector<ofMesh> walls;
    
    ofShader shader;
    
    ofVec3f targetPos;
    ofVec3f cameraRotation;

    
    ofShader blurShader, bloom;
    ofFbo fbo, onePassFbo, twoPassFbo, finalfbo;
    
    float angle = 0;
    
    int isXW,isYW,isZW,isXY,isXZ,isYZ;
    int prevW, currW;
    float changePoint = 0, bingoPoint = 0;
    
    char        input;
    char        prevInput;
    ofSerial    serial;
//    unsigned char debug = 0b00010001;
    bool change = false;
    bool bingo = false;
    bool prevBingo = false;
    
    ofSoundPlayer  bgm;
    ofSoundPlayer  blink;
    
    bool isShaderDirty;
    bool isScreenDirty;
};
