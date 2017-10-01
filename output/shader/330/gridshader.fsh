#version 330

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

uniform sampler2D ColorMap;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular0;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

 in vec3 outNormal;
 in vec3 outColor;
 in vec2 outUV;
 in vec3 outMeshColor;

 out vec4 outFragColor;

void main() {

  if (outMeshColor.x == 0.0 && outMeshColor.y == 0.0 && outMeshColor.x == 0.0)
	 outFragColor = vec4(outColor, 1.0);
	  else outFragColor = vec4(outMeshColor.xyz, 1.0);
}