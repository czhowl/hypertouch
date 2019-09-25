#version 400

uniform float angle;
uniform vec2 resolution;
uniform sampler2DRect maskTex;
uniform float bingoPoint;

uniform int isXW;
uniform int isZW;
uniform int isYW;

uniform int prevW;
uniform float time;
uniform float changePoint;

in vec4 vColor;

out vec4 outputColor;

//    Simplex 3D Noise
//    by Ian McEwan, Ashima Arts
//
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(vec3 v){
    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);
    
    // First corner
    vec3 i  = floor(v + dot(v, C.yyy) );
    vec3 x0 =   v - i + dot(i, C.xxx) ;
    
    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );
    
    //  x0 = x0 - 0. + 0.0 * C
    vec3 x1 = x0 - i1 + 1.0 * C.xxx;
    vec3 x2 = x0 - i2 + 2.0 * C.xxx;
    vec3 x3 = x0 - 1. + 3.0 * C.xxx;
    
    // Permutations
    i = mod(i, 289.0 );
    vec4 p = permute( permute( permute(
                                       i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
                              + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
                     + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));
    
    // Gradients
    // ( N*N points uniformly over a square, mapped onto an octahedron.)
    float n_ = 1.0/7.0; // N=7
    vec3  ns = n_ * D.wyz - D.xzx;
    
    vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)
    
    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)
    
    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);
    
    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );
    
    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));
    
    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;
    
    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);
    
    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;
    
    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                 dot(p2,x2), dot(p3,x3) ) );
}

void main() {
    
    vec2 p = (gl_FragCoord.xy * 2.0 - resolution) / min(resolution.x, resolution.y);
    
    vec4 src = texture(maskTex, vec2(gl_FragCoord.x,resolution.y-gl_FragCoord.y)).rgba;
    float r, g, b;
    float rp, gp, bp;
    float a;
//    float r = 0.03 / abs(0.9 - length(p)) * abs(sin(p.x + p.y + time));
//    float g = 0.03 / abs(0.9 - length(p)) * abs(sin(p.x + p.y + time * 0.5) * cos(p.x + p.y + time * 0.5));
//    float b = 0.03 / abs(0.9 - length(p)) * abs(cos(p.x + p.y + time));
//
    float noise = snoise(vec3(p.x * 0.2, p.x * 0.2, time * 0.3)) * 1.0 + 1.0 - p.y * 1.0;
    if(abs(isYW) == 1){
        r = 0.02 / abs(0.9 - length(p)) * abs(cos(p.x + p.y + angle))*0.3 - noise;
        g = 0.03 / abs(0.9 - length(p)) * (abs(cos(p.x + p.y + angle + 1) * sin(p.x + p.y + angle)) * 1.5)- noise;
        b = 0.08 / abs(0.9 - length(p)) * (abs(sin(p.x + p.y + angle)) * 0.7 + 0.2) + noise;
    }else if(abs(isZW) == 1){
        r = 0.05 / abs(0.9 - length(p)) * abs(cos(p.x + p.y + angle+0.5));
        g = 0.01 / abs(0.9 - length(p)) * (cos(p.x + p.y + angle) * sin(p.x + p.y + angle) + 0.5) - noise;
        b = 0.06 / abs(0.9 - length(p)) * abs(sin(p.x + p.y + angle));
    }else if(abs(isXW) == 1){
        r = 0.05 / abs(0.9 - length(p)) * abs(sin(p.x + p.y + angle + 0.5) * cos(p.x + p.y + angle + 0.5));
        g = 0.05 / abs(0.9 - length(p)) * abs(sin(p.x + p.y + angle + 1));
        b = 0.05 / abs(0.9 - length(p)) * abs(cos(p.x + p.y + angle)) - noise * 1.5;
    }else{
        r = 0.05 / abs(0.9 - length(p));
        g = 0.05 / abs(0.9 - length(p));
        b = 0.05 / abs(0.9 - length(p));
    }
    
    if(prevW == 2){
        rp = 0.02 / abs(0.9 - length(p)) * abs(cos(p.x + p.y + angle))*0.3 - noise;
        gp = 0.03 / abs(0.9 - length(p)) * (abs(cos(p.x + p.y + angle + 1) * sin(p.x + p.y + angle)) * 1.5)- noise;
        bp = 0.08 / abs(0.9 - length(p)) * (abs(sin(p.x + p.y + angle)) * 0.7 + 0.2) + noise;
    }else if(prevW == 3){
        rp = 0.05 / abs(0.9 - length(p)) * abs(cos(p.x + p.y + angle+0.5));
        gp = 0.01 / abs(0.9 - length(p)) * (cos(p.x + p.y + angle) * sin(p.x + p.y + angle) + 0.5) - noise;
        bp = 0.06 / abs(0.9 - length(p)) * abs(sin(p.x + p.y + angle));
    }else if(prevW == 1){
        rp = 0.05 / abs(0.9 - length(p)) * abs(sin(p.x + p.y + angle + 0.5) * cos(p.x + p.y + angle + 0.5));
        gp = 0.05 / abs(0.9 - length(p)) * abs(sin(p.x + p.y + angle + 1));
        bp = 0.05 / abs(0.9 - length(p)) * abs(cos(p.x + p.y + angle)) - noise * 1.5;
    }else{
        rp = 0.05 / abs(0.9 - length(p));
        gp = 0.05 / abs(0.9 - length(p));
        bp = 0.05 / abs(0.9 - length(p));
    }
    
    a = 0.05 / abs(0.9 - length(p));
    
    float changeBlend = 1;
    if(time - changePoint < 3.1415926535897932384626433832795/2){
        changeBlend = sin((time - changePoint));
    }
    
    vec4 prevColor = vec4(rp, gp, bp, a) + noise;
    vec4 color = vec4(r, g, b, a) + noise;
    color = mix(prevColor,color,changeBlend);
    
    float bingoBlend = 0.9;
    if(time - bingoPoint < 0.6){
        float blendAngle = (time - bingoPoint)/0.6 + 1;
        bingoBlend = cos(sin(blendAngle)*tan(blendAngle*3.1415926535897932384626433832795)*3.1415926535897932384626433832795/8) * 2 - 1.1;
    }
        color = mix(color,src,bingoBlend);
//    }
    
//    color = mix(vec4(noise),color,color.r - noise);
//    color = vec4(noise + color.x, color.y, color.z, color.w);
    outputColor = vec4(color);
}
