#version 330

in vec2 uvCoord;

uniform sampler2D gSampler;

out vec4 frag_color;

void main() {
	gl_FragColor = texture2D(gSampler, uvCoord);
}
