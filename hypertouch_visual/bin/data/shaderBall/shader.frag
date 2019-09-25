#version 400
precision mediump float;

uniform mat4 inv;
uniform vec3 lightDirection;
uniform vec3 eyeDirection;
uniform vec4 ambientColor;
uniform vec2 resolution;
uniform float life;

in vec3 vNormal;
in vec4 vColor;
in vec2 vPosition;
in vec2 vTexCoord;

out vec4 outputColor;

void main(){
    vec2 p0 = gl_FragCoord.xy;
    float w = p0.x / resolution.x;
    float h = p0.y / resolution.y;
    vec2 p = vTexCoord;

    float dist = sqrt(dot(p0, p));

    vec3  invLight  = normalize(inv * vec4(lightDirection, 0.0)).xyz;
    vec3  invEye    = normalize(inv * vec4(eyeDirection, 0.0)).xyz;
    vec3  halfLE    = normalize(invLight + invEye);
    float diffuse   = clamp(dot(vNormal, invEye), 0.0, 1.0 );
    float specular  = pow(clamp(dot(vNormal, halfLE), 0.0, 1.0), 5.0);
//    vec3 specular  = vec3(0.1,0.0,0.2);
    vec4  destColor = vColor * vec4(vec3(diffuse), 1.0) + ambientColor + vec4(vec3(specular), 1.0);
//    vec4  destColor = (vColor * vec4(vec3(diffuse), 1.0) + ambientColor);

    destColor = vec4(vec3(destColor), life);
//    destColor = vec4(1.0 * w, 1.0 * h, 0.3, life);
//    if(dist <20){
//        destColor = vec4(1.0,1.0,1.0, life);
//    }else{
//        destColor = vec4(vec3(destColor), life);}
    outputColor = destColor;
}
