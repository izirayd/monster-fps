#pragma once

#include "graphic_base.hpp"
#include "camera.hpp"

struct ray_t
{
	vec3 start;
	vec3 end;
};

/*
float x = (2.0f * mouse_pos_2d.x) / size_win.x - 1.0f;
float y = 1.0f - (2.0f * mouse_pos_2d.y) / size_win.y;

vec4 ray_clip = vec4(x, y, -1.0, 1.0);
vec4 ray_eye = inverse(Camera.mvp.proj) * ray_clip;
ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

vec3 ray_wor = vec3(inverse(Camera.mvp.view) * ray_eye);

ray.end   = glm::normalize(ray_wor);
ray.start = Camera.position;
*/

void RayEye(vec3 &ray, const vec2 &mouse_pos_2d, const vec2 &size_win, const engine::camera &Camera)
{
	vec4 ray_eye = inverse(Camera.mvp.proj) * vec4((2.0f * mouse_pos_2d.x) / size_win.x - 1.0f, 1.0f - (2.0f * mouse_pos_2d.y) / size_win.y, -1.0, 1.0);
	ray = glm::normalize(vec3(inverse(Camera.mvp.view) *  vec4(ray_eye.x, ray_eye.y, -1.0, 0.0)));
}

void GetRayScreen2(ray_t &ray, const vec2 &mouse_pos_2d, const vec2 &size_win, const engine::camera &Camera)
{
	vec4 ray_eye = inverse(Camera.mvp.proj) * vec4((2.0f * mouse_pos_2d.x) / size_win.x - 1.0f, 1.0f - (2.0f * mouse_pos_2d.y) / size_win.y, -1.0, 1.0);
	ray.end   = glm::normalize(vec3(inverse(Camera.mvp.view) *  vec4(ray_eye.x, ray_eye.y, -1.0, 0.0)));
	ray.start = Camera.position;
}

void GetRayScreen(ray_t &ray, const vec2 mouse_pos_2d, const vec2 size_win, const engine::camera &Camera)
{
	vec2 mouse    = { mouse_pos_2d.x / (size_win.x  * 0.5f) - 1.0f,  mouse_pos_2d.y / (size_win.y  * 0.5f) - 1.0f};
	mat4 view     = glm::lookAt(glm::vec3(0.0f), Camera.direction, Camera.up);
	mat4 inv_vp   = glm::inverse(view * Camera.mvp.proj);
	vec4 screen   = { mouse.x, -mouse.y, 1.0f, 1.0f };
	vec4 worldPos = inv_vp * screen;
	vec3 dir = glm::normalize(glm::vec3(worldPos));

	ray.start = Camera.position;// glm::normalize(glm::vec3(worldPos));

	ray.end   = ray.start + dir * 100.f; // Camera.zfar;
}

void ScreenPosToWorldRay(
	int32_t mouseX, int32_t mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
	int32_t screenWidth, int32_t screenHeight,  // Window size, in pixels
	glm::mat4 ViewMatrix,               // Camera position and orientation
	glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
	glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
	glm::vec3& out_direction,            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
	glm::vec3& out_dir_no_normal
) {

	// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
	glm::vec4 lRayStart_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
		-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
	);

	glm::vec4 lRayEnd_NDC(
		((float)mouseX / (float)screenWidth - 0.5f)  * 2.0f,
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
		0.0,
		1.0f
	);

	lRayStart_NDC.y = -lRayStart_NDC.y;
	lRayEnd_NDC.y   = -lRayEnd_NDC.y;

	glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);
	glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);
	glm::vec4 lRayStart_camera = InverseProjectionMatrix    * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
	glm::vec4 lRayStart_world  = InverseViewMatrix          * lRayStart_camera; lRayStart_world  /= lRayStart_world.w;
	glm::vec4 lRayEnd_camera   = InverseProjectionMatrix    * lRayEnd_NDC;      lRayEnd_camera   /= lRayEnd_camera.w;
	glm::vec4 lRayEnd_world    = InverseViewMatrix          * lRayEnd_camera;   lRayEnd_world    /= lRayEnd_world.w;

	// Faster way (just one inverse)
//	glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
//	glm::vec4 lRayStart_world = M * lRayStart_NDC; 
//	glm::vec4 lRayEnd_world = M * lRayEnd_NDC;

	//lRayStart_world /= lRayStart_world.w;
	//lRayEnd_world   /= lRayEnd_world.w;


	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	out_dir_no_normal = lRayEnd_world;
	lRayDir_world = glm::normalize(lRayDir_world);

	out_origin    = glm::vec3(lRayStart_world);
	out_direction = glm::normalize(lRayDir_world);
}


bool TestRayOBBIntersection(
	const vec3 &ray_origin,        // Ray origin, in world space
	const vec3 &ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
	const vec3 &aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
	const vec3 &aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
	const mat4 &ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
	float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
) {

	// Intersection method from Real-Time Rendering and Essential Mathematics for Games

	float tMin = 0.0f;
	float tMax = 1000.0f;

	glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

	glm::vec3 delta = OBBposition_worldspace - ray_origin;

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
		float e = glm::dot(xaxis, delta);
		float f = glm::dot(ray_direction, xaxis);

		if (fabs(f) > 0.0001f) { // Standard case

			float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
			float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
											 // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

											 // We want t1 to represent the nearest intersection, 
											 // so if it's not the case, invert t1 and t2
			if (t1>t2) {
				float w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax)
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin)
				tMin = t1;

			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (tMax < tMin)
				return false;

		}
		else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
		float e = glm::dot(yaxis, delta);
		float f = glm::dot(ray_direction, yaxis);

		if (fabs(f) > 0.0001f) {

			float t1 = (e + aabb_min.y) / f;
			float t2 = (e + aabb_max.y) / f;

			if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
		float e = glm::dot(zaxis, delta);
		float f = glm::dot(ray_direction, zaxis);

		if (fabs(f) > 0.0001f) {

			float t1 = (e + aabb_min.z) / f;
			float t2 = (e + aabb_max.z) / f;

			if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
				return false;
		}
	}

	intersection_distance = tMin;
	return true;
}