#pragma once
#include "base.hpp"


 #include <GL/glew.h>
 #include <glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>


using namespace glm;

enum class graphic_library_t {
	opengl     = 0,
	vulkan     = 1,
	directx_9  = 2,
	directx_10 = 3,
	directx_11 = 4,
	directx_12 = 5
};
