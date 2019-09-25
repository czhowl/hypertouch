#version 150
in vec4 color;
in vec4 pos;
out vec4 outColor;
void main(){
	outColor = color + pow(clamp(pos.y * 0.1, 0.0, 1.0) * 0.5, 2);
}
