#version 330 core

in vec4 fColor; // This name must match the name in the vertex shader in order to work
out vec4 outColor;

in vec2 vTexcoord;   // passthrough shading for interpolated textures
uniform sampler2D tex;

void main() {
  outColor = fColor;
  // outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
