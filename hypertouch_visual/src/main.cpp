#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLWindowSettings settings;
    settings.setGLVersion(4, 1);
    settings.setSize(1280, 768);
    ofCreateWindow(settings);
    ofRunApp(new ofApp());
    
//    ofSetupOpenGL(1024, 768, OF_FULLSCREEN);            // <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
//    ofRunApp(new ofApp());

}
