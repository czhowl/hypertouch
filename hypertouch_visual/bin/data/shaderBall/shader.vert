#version 400
precision mediump float;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 mvp;
uniform mat4 modelViewProjectionMatrix;


in vec2 texcoord;
in vec3 position;
in vec3 normal;
in vec4 color;

out vec2 vTexCoord;
out vec3 vNormal;
out vec4 vColor;
out vec2 vPosition;

void main() {
    vNormal = normal;
    vColor = color;
    vTexCoord = texcoord;
//    gl_Position = projection * view * model * vec4(position, 1.0);
    gl_Position = mvp * vec4(position, 2.0);
//    gl_Position = modelViewProjectionMatrix * position;
    vPosition = (projection * vec4(position, 1.0)).xy;
    
    
}
//
//#version 150
//precision mediump float;
//
//uniform mat4 modelViewProjectionMatrix;
//
//in vec2 texcoord;
//in vec4 color;
//in vec4 position;
//in vec4 normal;
//
//out vec2 vTexCoord;
//out vec2 vPosition;
//out vec4 vColor;
//
//void main() {
//    vTexCoord = texcoord;
//    vColor = color;
//    gl_Position = modelViewProjectionMatrix * position;
//    vPosition = gl_Position.xy;
//}
//
