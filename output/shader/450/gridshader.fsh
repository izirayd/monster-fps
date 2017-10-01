#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

uniform sampler2D ColorMap;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular0;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

layout (location = 0) in vec3 outNormal;
layout (location = 1) in vec3 outColor;
layout (location = 2) in vec2 outUV;
layout (location = 3) in vec3 outMeshColor;

layout (location = 0) out vec4 outFragColor;

void main() {

  if (outMeshColor.x == 0.0 && outMeshColor.y == 0.0 && outMeshColor.x == 0.0)
	 outFragColor = vec4(outColor, 1.0);
	  else outFragColor = vec4(outMeshColor.xyz, 1.0);
}