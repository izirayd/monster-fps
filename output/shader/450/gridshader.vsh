#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//uniform mat4 MVP;

layout (location = 0) in  vec3 v3Position;
layout (location = 1) in  vec2 v2Texcoord;
layout (location = 2) in  vec3 v3Normal;
layout (location = 3) in  vec3 v3Color;

layout (location = 4) uniform mat4 CameraV;
layout (location = 5) uniform mat4 CameraP;
layout (location = 6) uniform mat4 Model;

layout (location = 7) uniform vec3 v3Meshcolor;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 outMeshColor;

void main(){
	//gl_Position =  MVP * vec4(v3Position, 1);
	gl_Position =  CameraP * CameraV * Model * vec4(v3Position, 1.0);
	outUV     = v2Texcoord;
	outColor  = v3Color;
	outNormal = v3Normal;
	outMeshColor = v3Meshcolor;
}
