#version 330

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

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

   //vec4 color = texture(ColorMap, outUV) * vec4(outColor, 1.0);

   float lightInten = 0.1;
   vec4 color =  vec4((texture( texture_diffuse0, outUV) + texture( texture_diffuse1, outUV) + texture( texture_diffuse2, outUV) + texture( texture_specular0, outUV) + texture( texture_specular1, outUV) + texture( texture_specular2, outUV)) * lightInten);
 //  outFragColor = vec4(texture2D(texture_diffuse0, outUV).rgb * lightInten);
	

     if (color.x != 0.0 && color.y != 0.0 && color.z != 0.0) 
     outFragColor = color;
	  else 
	 if (outMeshColor.x == 0.0 && outMeshColor.y == 0.0 && outMeshColor.x == 0.0)
	 outFragColor = vec4(outColor, 1.0);
	  else 
	  outFragColor = vec4(outMeshColor.xyz, 1.0);
}
