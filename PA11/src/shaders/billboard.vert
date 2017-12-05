#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

uniform vec3 billboardLocation;
uniform float aspect;

out vec2 uvCoord;

void main() {
	gl_Position = vec4(position.xy * billboardLocation.z * vec2(aspect, 1.0) + billboardLocation.xy, 0.0, 1.0);
	uvCoord = uv;
}
