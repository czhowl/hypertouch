//
//  BloomPass.hpp
//  4dbox_mesh
//
//  Created by Zihao Chen on 12/1/18.
//

#ifndef BloomPass_hpp
#define BloomPass_hpp

#include <stdio.h>
#include "ofMain.h"
#include "Blur.h"

class BloomPass {
private:
    ofFbo blurred;
    Blur blur;
    
    ofShader lumaShader;
    ofFbo lumaFbo;
    
    float lumaThres;
    
public:
    using Ptr = shared_ptr<BloomPass>;
    
    BloomPass(const glm::vec2& size);
    void update(const ofCamera& cam) {}
    void render(ofFbo& readFbo, ofFbo& writeFbo, GBuffer& gbuffer);
    
    void setThreshold(float thres) { lumaThres = thres; }
};
#endif /* BloomPass_hpp */
