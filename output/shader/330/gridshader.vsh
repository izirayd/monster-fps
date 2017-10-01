#version 330

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//uniform mat4 MVP;

 in  vec3 v3Position;
 in  vec2 v2Texcoord;
 in  vec3 v3Normal;
 in  vec3 v3Color;

 uniform mat4 CameraV;
 uniform mat4 CameraP;
 uniform mat4 Model;

 uniform vec3 v3Meshcolor;

 out vec3 outNormal;
 out vec3 outColor;
 out vec2 outUV;
 out vec3 outMeshColor;

void main(){
	//gl_Position =  MVP * vec4(v3Position, 1);
	gl_Position =  CameraP * CameraV * Model * vec4(v3Position, 1.0);
	outUV     = v2Texcoord;
	outColor  = v3Color;
	outNormal = v3Normal;
	outMeshColor = v3Meshcolor;
}
