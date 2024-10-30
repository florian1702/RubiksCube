#version 330

in vec3 vertColor;
out vec4 color;

void main() {
	color = vec4(vertColor, 1.0f);
	//gl_FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}