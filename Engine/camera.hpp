/*
  Класс камеры. Писал Серж в 2017 в 30 сентября
*/


#pragma once
#include "graphic_base.hpp"

typedef uint8_t signal_t;

enum class camera_signal_t : signal_t
{
	MoveLeft = 1 << 0,
	MoveRight = 1 << 1,
	MoveForward = 1 << 2,
	MoveBack = 1 << 3,
	MoveSpeedUP = 1 << 4
};

inline void add_signal(signal_t &signal, camera_signal_t add) { signal |= (signal_t)add;}
inline void del_signal(signal_t &signal, camera_signal_t del) { signal &= ~(signal_t)del;}
inline bool check_signal(const signal_t &signal, camera_signal_t flag){ return (signal & (signal_t)flag) ? true : false; }


namespace engine
{
	struct vp_t
	{
		vp_t(const mat4& v, const mat4& p) { view = v; proj = p; }
		vp_t() = default;

		inline mat4 GetView() const { return view; }
		inline mat4 GetProj() const { return proj; }
		inline mat4 GetVP()   const { return view * proj; }

		mat4 view;
		mat4 proj;
	};

	struct mvp_t
	{
		mvp_t(const mat4& v, const mat4& p, const mat4& m) { view = v; proj = p; model= m;	}
		mvp_t() = default;

		inline mat4 GetView() const { return view; }
		inline mat4 GetProj() const { return proj; }
		inline mat4 GetVP()   const { return view * proj; }
		inline mat4 GetMVP()  const { return model * view * proj;	}

		mat4 view;
		mat4 proj;
		mat4 model;
	};

	class obj_t
	{
	 public:
		vec3  scale     = { 1.0, 1.0, 1.0 };
		vec3  position;
		quat  rotation;
		mvp_t mvp;

		void SetPosition(const vec3& p)   { position  = p;  build_model_mat(); }
		void ApplyPosition(const vec3& p) { position += p;  build_model_mat(); }
		void SetScale(const vec3& s)      { scale     = s;  build_model_mat(); }
		void ApplyScale(const vec3& s)    { scale    += s;  build_model_mat(); }
		void SetRotation(const quat& r)   { rotation  = r;  build_model_mat(); }
		void ApplyRotation(const quat& r) { rotation += r;  build_model_mat(); }

		inline void build_model_mat() {	mvp.model = glm::translate(mat4(), position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(), scale); }

	};

	class camera : public engine::obj_t
	{
	  public:
		  float movespeed = 7.5f, rotationspeed = 1.0f;
		  float fov, max_sight, min_sight, aspect;

		  vec2  size_window;	
		  vec3  direction = vec3(0, 0, 0), up = vec3(0, 1.f, 0);
		  vec3 front, right, worldup = vec3(0, 1.f, 0);
	
		  float yaw = -90.f, pitch = 0, zoom = 45.0f, mouse_sens = 0.25f;
		  signal_t signal;
	
		  camera()   {
			  signal = 0;
		  }
	
		  void  SetPerspective(const float &fov, const float &aspect, const float &min_sight, const float &max_sight)
		  {
			  bool isEdit = false;

			  if (this->fov       != fov)         { this->fov = fov;             isEdit = true; }
			  if (this->max_sight != max_sight)   { this->max_sight = max_sight; isEdit = true; }
			  if (this->min_sight != min_sight)   { this->min_sight = min_sight; isEdit = true; }
			  if (this->aspect != aspect && aspect > 0.0f && aspect < 10.0f)  {   this->aspect = aspect;   isEdit = true;  }
			    
			  if (isEdit)
				  mvp.proj = glm::perspective(glm::radians(fov), aspect, min_sight, max_sight);
		  };

		  void rotate(vec3 v)     { direction += v; update_look(); }
		  void update_look()	  { 
			
			  mvp.view = glm::lookAt(position, position + direction, up);	 
			//  printf("Update %f\n", mvp.view[0].x);
		  }
		  void rotate_camera(float amount, const vec3& axis) {    direction = glm::rotate(direction, amount * rotationspeed, axis); }

		  inline bool moving() const {
			  return signal != 0 ? true : false;
		  }

		  void move(float deltaTime) {

			  if (moving())
			  {
				  float move = check_signal(signal, camera_signal_t::MoveSpeedUP) ? movespeed * 10 : movespeed;
				 
				  move *= deltaTime;

				  if (check_signal(signal, camera_signal_t::MoveForward))
					  position -= direction * move;
			
				  if (check_signal(signal, camera_signal_t::MoveBack))
					  position += direction * move;

				  if (check_signal(signal, camera_signal_t::MoveRight))
					  position += glm::cross(direction, up) * move;

				 if (check_signal(signal, camera_signal_t::MoveLeft))
					  position -= glm::cross(direction, up) * move;

				  update_look();
			  }
		  }

		  void mouse_move(vec2 mouse_pos) {
 
			  mouse_pos = -mouse_pos;

			  mouse_pos.x *= mouse_sens;
			  mouse_pos.y *= mouse_sens;

			  yaw   += mouse_pos.x;
			  pitch += mouse_pos.y;

			  if (pitch > 89.0f)  pitch =  89.0f;
			  if (pitch < -89.0f) pitch = -89.0f;

			  update_camera_vectors();
		  }

		  void update_camera_vectors()
		  {
			  front = vec3();
			  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			  front.y = sin(glm::radians(pitch));
			  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

			  direction = glm::normalize(front);

			  right = glm::normalize(glm::cross(direction, worldup));
			  up    = glm::normalize(glm::cross(right, direction));

			  update_look();
		  }
	};
}