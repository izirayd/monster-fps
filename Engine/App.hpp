#pragma once
#pragma execution_character_set("utf-8")

#include "graphic_base.hpp"
#include "error.hpp"

#include "shader.hpp"
#include "path.hpp"
#include "file.hpp"
#include "texture.hpp"
#include "mesh.hpp"

#include "cube.hpp"
#include "fps.hpp"
#include "engine_config.hpp"
#include "camera.hpp"
#include "picking.hpp"

#include "imgui_impl_glfw_gl3.h"
#include <imgui.h>

#include <iostream>


#define CountMesh 32


CGroupMesh *EditModel;

class CRender
{
  public:
	  std::vector<CGroupMesh> ListGroupMesh;
	  bool isRender = true;
	  void Add(const CGroupMesh &obj)
	  {
		  ListGroupMesh.push_back(obj);
	  }

	  void Render(CShader *Shader)
	  {
		  if (isRender)
		    for (auto &it : ListGroupMesh)
			  it.Render(Shader);
	  }

	  void Clear()
	  {
		  ListGroupMesh.clear();
		  ListGroupMesh.resize(0);
	  }
};


class CWorldModel
{
public:
	std::vector<CRender> GroupModelsGroup;

	template <typename T>
	inline CWorldModel operator=(T m) { Add(m); return *this; }

	CWorldModel() = default;
	CWorldModel(engine::camera *NewCamera) {
		Camera = NewCamera;
	}

	engine::camera *Camera;

	CWorldModel operator= (engine::camera *NewCamera)
	{
		Camera = NewCamera;
		return *this;
	}

	CWorldModel operator= (engine::camera &NewCamera)
	{
		Camera = &NewCamera;
		return *this;
	}

	// Функция вернет Index модели в мире
	uint32_t LoadModel(wchar_t *Path, wchar_t *ClientDir, bool Render = true)
	{
		CGroupMesh Model = Camera;

		Model.LoadModel(Path, ClientDir);

		for (auto &it : Model.ListMesh)
			it.isRender = Render;
	
		Add(Model);

		return GroupModelsGroup.size() - 1;
	}


	void Add(CRender GroupGroupMesh)
	{
		GroupModelsGroup.push_back(GroupGroupMesh);
	}

	void Add(CGroupMesh GroupMesh)
	{
		CRender r;
		r.Add(GroupMesh);
		Add(r);
	}

	void Add(CMesh Mesh)
	{
		CGroupMesh gm;
		gm.AddMesh(Mesh);
		Add(gm);
	}

	void AddGroup(CRender ModelsGroup)
	{
		GroupModelsGroup.push_back(ModelsGroup);
	}

	void StopRender(uint32_t index)
	{
		if (index > GroupModelsGroup.size()) return;
		GroupModelsGroup[index].isRender = false;
	}

	void StartRender(uint32_t index)
	{
		if (index > GroupModelsGroup.size()) return;
		GroupModelsGroup[index].isRender = true;
	}

	void Render(CShader *Shader)
	{
		if (!isRender) return;

		for (auto &it : GroupModelsGroup)
			it.Render(Shader);
	}

	bool isRender = true;
};


class CApplication
{
public:

	int32_t VersionOpenGLMajor, VersionOpenGLMinor;
	std::string AdapterInfo;


	ImFont* font;

	static void error_callback(int error, const char* description)
	{
		fprintf(stderr, "Error %d: %s\n", error, description);
	}

	// Вернет true если ошибка
	bool SelectVersionOpengl(int32_t versionmajor, int32_t versionminor)
	{
		VersionOpenGLMajor = versionmajor;
		VersionOpenGLMinor = versionminor;

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VersionOpenGLMajor);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VersionOpenGLMinor);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(SizeWidth, SizeHeight, TitleApp.c_str(), NULL, NULL);

		return window == NULL;
	}

	CError Init(const std::string TitleNameApplication, const  int32_t width, const int32_t height, const graphic_library_t graphic_library = graphic_library_t::opengl)
	{
		CError Result;

		setlocale(0, "RUSSIAN");

		CPath Path;

		setlocale(0, "RUSSIAN");
		ImGuiIO& io = ImGui::GetIO();

		font = io.Fonts->AddFontFromFileTTF(L"C:\\Windows\\Fonts\\Tahoma.ttf", 14.0f);
		io.Fonts->Build();

		glfwSetErrorCallback(error_callback);

		if (!glfwInit())
			return Result = "Failed to initialize GLFW\n";

		TitleApp = TitleNameApplication;

		if (width == 0 || width > 8128 || height == 0 || height > 8128)
			return Result = "Error size parametr width or height";

		SizeWidth = width;
		SizeHeight = height;

		if (SelectVersionOpengl(4, 5))
			if (SelectVersionOpengl(3, 3))
			{
				Result.format("Failed to open GLFW window.If you have an Intel GPU, they are not %d.%d compatible.", VersionOpenGLMajor, VersionOpenGLMinor);

#ifdef PLATFORM_WINDOWS
				MessageBoxW(0L, L"Не удаётся выбрать адаптер. Версия OpenGL не доступна.", L"Ошибка!", 0);
#endif		
				return Result;
			}

		glfwMakeContextCurrent(window);

		glewInit() != GLEW_OK ? Result = "Failed to initialize GLEW" : Result;

		char ibuf[512];

		sprintf(ibuf, "Version OpenGL: %d.%d: %s\nVendor: %s\nRender: %s\nShader version: %s\n", VersionOpenGLMajor, VersionOpenGLMinor,
			glGetString(GL_VERSION),
			glGetString(GL_VENDOR),
			glGetString(GL_RENDERER),
			glGetString(GL_SHADING_LANGUAGE_VERSION)
		);

		AdapterInfo = ibuf;

		ImGui_ImplGlfwGL3_Init(window, true);

		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, KeyboardHandler);
		glfwSetMouseButtonCallback(window, MouseHandler);
		glfwSetCursorPosCallback(window, MouseMoveHandler);
		glfwSetScrollCallback(window, MouseScrollHandler);
		glfwSetFramebufferSizeCallback(window, FramebufferSizeHandler);

		glClearDepth(1.0f);

		glFrontFace(GL_CW);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		LoadData();

		glfwSwapInterval(0);

		return Result;
	}

	vec2 mousePos = vec2();

	static void MouseMoveHandler(GLFWwindow* window, double posx, double posy) {

		CApplication* example = (CApplication*) glfwGetWindowUserPointer(window);

		vec2 newPos = vec2(posx, posy);

		glm::vec2 deltaPos = example->mousePos - newPos;

		if (deltaPos.x == 0 && deltaPos.y == 0) 
			return;
		
		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))	
				example->camera.mouse_move(deltaPos);			
		

		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE)) 
			example->camera.ApplyPosition(glm::vec3(-deltaPos.x * 0.01f, -deltaPos.y * 0.01f, 0.0f));
		
		example->mousePos = newPos;
		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
			example->SelectSystem(true); else
		example->SelectSystem();
	}

	static void FramebufferSizeHandler(GLFWwindow* window, int width, int height) {
		CApplication* example = (CApplication*)glfwGetWindowUserPointer(window);
		example->SizeHeight = height;
		example->SizeWidth = width;
	}

	static void MouseScrollHandler(GLFWwindow* window, double xoffset, double yoffset) {
		CApplication* example = (CApplication*)glfwGetWindowUserPointer(window);
		if (yoffset > 0)	
			example->camera.movespeed *= 2.2f;
		else	
			example->camera.movespeed *= 0.4f;
	}

	static void MouseHandler(GLFWwindow* window, int button, int action, int mods) {

		CApplication* example = (CApplication*)glfwGetWindowUserPointer(window);

		if (action == GLFW_PRESS) {
			glm::dvec2 mousePos; 
			glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
			example->mousePos = mousePos;

			if (button == GLFW_MOUSE_BUTTON_LEFT)
				example->SelectSystem(true);
			
		}
	}

	ray_t ray;
	glm::vec3 aabb_min = { -1.0f, -1.0f, -1.0f }, aabb_max = { 1.0f, 1.0f, 1.0f };
	float intersection_distance = 0.f;
	vec3 r;

	bool isSelectObj = false;
	vec3 StartSelectPosition;
	vec3 EndSelectPosition;
	

	void SelectSystem(bool isPressLeft = false)
	{
		if (isPressLeft) {

			RayEye(r, mousePos, vec2(SizeWidth, SizeHeight), camera);

			if (!isSelectObj)
			{
				MultiSelectRectangle[0].EnableRender();
				MultiSelectRectangle[1].EnableRender();
				MultiSelectRectangle[2].EnableRender();
				MultiSelectRectangle[3].EnableRender();

				StartSelectPosition = r;
			}

			EndSelectPosition = r;

			vec3 pos_line_start = camera.position + StartSelectPosition;
			vec3 pos_end        = camera.position + EndSelectPosition;
			//vec3 pos_line_end(pos_line_start.x, pos_end.y, pos_end.z);
	
			MultiSelectRectangle[0].Start();
			MultiSelectRectangle[0].AddVertex(pos_line_start, vec2(), vec3(), vec3(1.0f, 0, 0));
			MultiSelectRectangle[0].AddVertex(pos_end, vec2(), vec3(), vec3(1.0f, 0, 0));
			MultiSelectRectangle[0].End();

			//MultiSelectRectangle[1].Start();
		///	MultiSelectRectangle[1].AddVertex(pos_line_start, vec2(), vec3(), vec3(1.0f, 0, 0));
			//MultiSelectRectangle[1].AddVertex(pos_line, vec2(), vec3(), vec3(1.0f, 0, 0));
			//MultiSelectRectangle[1].End();

			isSelectObj = true;
		}
		else isSelectObj = false;
		 
		if (!isSelectObj) {
			MultiSelectRectangle[0].DisableRender();
			MultiSelectRectangle[1].DisableRender();
			MultiSelectRectangle[2].DisableRender();
			MultiSelectRectangle[3].DisableRender();
		}

		GetRayScreen2(ray, mousePos, vec2(SizeWidth, SizeHeight), camera);

		for (auto &wit : WorldModel.GroupModelsGroup)
			for (auto &git : wit.ListGroupMesh)
			{
				bool isSelect = false;
				for (auto &lit : git.ListMesh)			
				  if (!lit.isRender) 
						 break;
					else 
					{
						if (!isSelect)
							if (TestRayOBBIntersection(ray.start, ray.end, aabb_min, aabb_max, lit.Model, intersection_distance)) {
								isSelect = true;
								intersection_distance = 0;
							}

						isSelect ? lit.Color = { 1.0, 0, 0 } : lit.Color = { 1.0, 1.0, 0 };
					}
			}
	}

	static void KeyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {

		CApplication* example = (CApplication*)glfwGetWindowUserPointer(window);

		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			if (key == GLFW_KEY_W)
				add_signal(example->camera.signal, camera_signal_t::MoveBack);
			 
			if (key == GLFW_KEY_S)
			    add_signal(example->camera.signal, camera_signal_t::MoveForward);

			if (key == GLFW_KEY_D)			
				add_signal(example->camera.signal, camera_signal_t::MoveRight);
	
			if (key == GLFW_KEY_A)	 
			    add_signal(example->camera.signal, camera_signal_t::MoveLeft);

			if (key == GLFW_KEY_LEFT_SHIFT)
				add_signal(example->camera.signal, camera_signal_t::MoveSpeedUP);
			
			if (key == GLFW_KEY_E)
				for (auto &it: example->WorldModel.GroupModelsGroup[0].ListGroupMesh[0].ListMesh)			
			      it.ApplyScale({0.01f, 0.01f, 0.01f});

			if (key == GLFW_KEY_Q)
				for (auto &it : example->WorldModel.GroupModelsGroup[0].ListGroupMesh[0].ListMesh)
					it.ApplyScale({ -0.01f, -0.01f, -0.01f });

			if (key == GLFW_KEY_UP)
				for (auto &it : example->WorldModel.GroupModelsGroup[0].ListGroupMesh[0].ListMesh)
					it.ApplyPosition({ 0.0, 0.0, -1.0 });

			if (key == GLFW_KEY_DOWN)
				for (auto &it : example->WorldModel.GroupModelsGroup[0].ListGroupMesh[0].ListMesh)
					it.ApplyPosition({ 0.0, 0.0, 1.0 });

			if (key == GLFW_KEY_RIGHT)
				for (auto &it : example->WorldModel.GroupModelsGroup[0].ListGroupMesh[0].ListMesh)
					it.ApplyPosition({ 1.0, 0.0, 0.0 });

			if (key == GLFW_KEY_LEFT)
				for (auto &it : example->WorldModel.GroupModelsGroup[0].ListGroupMesh[0].ListMesh)
					it.ApplyPosition({ -1.0, 0.0, 0.0 });

			if (example->camera.moving())
				example->SelectSystem();
		}

		if (action == GLFW_RELEASE)
		{
			if (key == GLFW_KEY_W)
				del_signal(example->camera.signal, camera_signal_t::MoveBack);

			if (key == GLFW_KEY_S)
				del_signal(example->camera.signal, camera_signal_t::MoveForward);

			if (key == GLFW_KEY_D)
				del_signal(example->camera.signal, camera_signal_t::MoveRight);
						
			if (key == GLFW_KEY_A)
				del_signal(example->camera.signal, camera_signal_t::MoveLeft);

			if (key == GLFW_KEY_LEFT_SHIFT)		
				del_signal(example->camera.signal, camera_signal_t::MoveSpeedUP);
		}

		if (key == GLFW_KEY_F1 && (action == GLFW_PRESS || action == GLFW_REPEAT))
		{
			example->isWireframe = !example->isWireframe;
			if (example->isWireframe)
			   printf("Wireframe mode on\n"); else printf("Wireframe mode off\n");
		}

		if (key == GLFW_KEY_F2 && (action == GLFW_PRESS || action == GLFW_REPEAT))
		{
			example->camera.SetPosition({0, 0, 0});
			example->camera.movespeed = 25;
		}
		
	}

	class CGUIManager
	{
	public:

		bool isFPSWindow = true;
		bool isObjScene  = true;
		bool isSettingsGraphics = true;

		engine::camera *Camera;
		CGUIManager operator= (engine::camera *NewCamera) { Camera = NewCamera;  return *this; }
		CGUIManager operator= (engine::camera &NewCamera) { Camera = &NewCamera; return *this; }

		CGUIManager operator= (CApplication *NewApp) { App = NewApp;  return *this; }
		CGUIManager operator= (CApplication &NewApp) { App = &NewApp; return *this; }

		CApplication *App;

		bool isFrontGL_CW   = false;
		bool isCullFace     = false;
		bool isDEPTH_TEST   = true;
		bool isGL_LESS      = true;
		bool isGL_CULL_FACE = false;
		vec3 tmp;

		void Scene()
		{
			if (isSettingsGraphics)
			{
				ImGui::SetNextWindowPos(ImVec2(0, 250), ImGuiSetCond_Once);
				ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiSetCond_Once);
				ImGui::Begin("Settings graphics", &isSettingsGraphics);

				bool isTmp;

				//При описании полигона, вершины перечисляются одна за другой, чтобы не нарушить требование несамопересечения. Тем не менее существует две возможности для перечисления: по часовой стрелке (clockwise, CW) и против часовой (counterclockwise, CCW). По умолчанию, та сторона полигона, для которой порядок перечисления вершин совпадает с обходом против часовой стрелки, считается лицевой.		
				//GL_CCW
			    //Лицевая сторона задается обходом против часовой стрелки.Значение по умолчанию.
			    //GL_CW
				//Лицевая сторона задается обходом по часовой стрелке.
				isTmp = isFrontGL_CW;
				ImGui::Checkbox("GL_CW", &isFrontGL_CW);

				if (isTmp != isFrontGL_CW)
					isTmp ? glFrontFace(GL_CW) : glFrontFace(GL_CCW);


				// Рисование задних граний
				isTmp = isCullFace;
				ImGui::Checkbox("CullFace", &isCullFace);

				if (isTmp != isCullFace)
					if (isTmp) {
						glEnable(GL_CULL_FACE);
						glCullFace(GL_BACK);
					}
					else			
						glDisable(GL_CULL_FACE);
					
				isTmp = isDEPTH_TEST;
				ImGui::Checkbox("GL_DEPTH_TEST", &isDEPTH_TEST);

				if (isTmp != isDEPTH_TEST)
					if (isTmp) 
						glEnable(GL_DEPTH_TEST);			
					else
						glDisable(GL_DEPTH_TEST);

				ImGui::End();
			}

			if (isFPSWindow)
			{
				ImGui::SetNextWindowPos(ImVec2(0, 0),      ImGuiSetCond_Once);
				ImGui::SetNextWindowSize(ImVec2(350, 250), ImGuiSetCond_Once);

				ImGui::Begin("FPS/Camera information", &isFPSWindow);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	
				ImGui::SliderFloat("", &Camera->movespeed, 0.0f, 1000.0f, "camera speed move = %.3f");
				ImGui::Text("Position camera x: %.1f y: %.1f z: %.1f\n", Camera->position.x, Camera->position.y, Camera->position.z);

				tmp = Camera->position;
				ImGui::SliderFloat3("position camera", glm::value_ptr(tmp), 0.0f, 512.0f);

				if (Camera->position != tmp)
				{
					Camera->position = tmp;
					Camera->update_look();
				}
				
				ImGui::Text("Ray  Start x: %.5f y: %.5f z: %.5f\n", App->ray.start.x, App->ray.start.y, App->ray.start.z);
				ImGui::Text("Ray  End x: %.5f y: %.5f z: %.5f\n",   App->ray.end.x,   App->ray.end.y,   App->ray.end.z);
		

				ImGui::Text("R x: %.5f y: %.5f z: %.5f\n", App->r.x, App->r.y, App->r.z);
				ImGui::Text("%s", App->AdapterInfo.c_str());

				ImGui::End();
			}

		if (isObjScene)
			{
			ImGui::SetNextWindowPos(ImVec2(0, 100), ImGuiSetCond_Once);
			ImGui::SetNextWindowSize(ImVec2(250, 150), ImGuiSetCond_Once);
			ImGui::Begin("Model list", &isObjScene);

			if (ImGui::TreeNode("Global Scene"))
			{
				if (ImGui::TreeNode((void*)(intptr_t)0, "Grid"))
				{
					ImGui::Checkbox("Enable/Disable render", &App->Grid.isRender);

					if (ImGui::TreeNode("Recreate"))
					{
						int32_t tmp1 = App->GridSettings.Size, tmp2 = App->GridSettings.Step;;

						ImGui::InputInt("Size Grid", &App->GridSettings.Size, 32, 32768);
						ImGui::InputInt("Step Grid", &App->GridSettings.Step, 1,  256);

						if ((tmp1 != App->GridSettings.Size || tmp2 != App->GridSettings.Step) && (App->GridSettings.Size >= 32 && App->GridSettings.Step >= 1)) {
							
							App->Grid.delete_allocate_in_gpu();

							App->GridSettings.CountIt = (App->GridSettings.Size / App->GridSettings.Step);

							App->Grid.Vao.Bind();
							App->Grid.StartVertex();

							for (int32_t i = -App->GridSettings.CountIt; i <= App->GridSettings.CountIt; i++)
							{
								App->Grid.AddVertex(vec3(-App->GridSettings.Size,    0, i * App->GridSettings.Step), vec2(), vec3(), App->GridSettings.Color);
								App->Grid.AddVertex(vec3(App->GridSettings.Size,     0, i * App->GridSettings.Step), vec2(), vec3(), App->GridSettings.Color);
								App->Grid.AddVertex(vec3(i * App->GridSettings.Step, 0, -App->GridSettings.Size),    vec2(), vec3(), App->GridSettings.Color);
								App->Grid.AddVertex(vec3(i * App->GridSettings.Step, 0,  App->GridSettings.Size),    vec2(), vec3(), App->GridSettings.Color);
							}

							App->Grid.EndVertex();
							App->Grid.Vao.Unbind();
						}

						ImGui::TreePop();
					}					
					ImGui::TreePop();
				}


				if (ImGui::TreeNode((void*)(intptr_t)1, "Plane"))
				{
					ImGui::Checkbox("Enable/Disable render", &App->ObjModel.isRender);

					if (ImGui::TreeNode("Recreate"))
					{
					   float   tmp1 = App->ObjSize;
					   int32_t tmp2 = App->size_x;
					   int32_t tmp3 = App->size_z;

						ImGui::InputFloat("Size segment", &App->ObjSize, 0.01f, 256);
						ImGui::InputInt("Size x", &App->size_x, 1, 100000);
						ImGui::InputInt("Size z", &App->size_z, 1, 100000);

						if (
							(tmp2 != App->size_x || tmp3 != App->size_z || tmp1 != App->ObjSize) 
						     && 
							(App->ObjSize >= 0.01f && App->ObjSize < 64000000 && App->size_x >= 1 && App->size_z >= 1)
							)					
							   App->ObjModel.ConvertInPlane(App->ObjSize, App->size_x, App->size_z, App->ObjColor);
						

						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
		
				ImGui::TreePop();
			}

			ImGui::End();


				/*if (ImGui::Button("Grid"))
				Grid.isRender = !Grid.isRender;

				if (ImGui::Button("All render"))
				WorldModel.isRender = !WorldModel.isRender;

				if (ImGui::Button("Chairs"))
				WorldModel.GroupModelsGroup[1].isRender = !WorldModel.GroupModelsGroup[1].isRender;

				if (ImGui::Button("Sphere"))
				WorldModel.GroupModelsGroup[0].isRender = !WorldModel.GroupModelsGroup[0].isRender;


				if (ImGui::Button("Rand Sphere"))
				WorldModel.GroupModelsGroup[2].isRender = !WorldModel.GroupModelsGroup[2].isRender;

				if (ImGui::Button("Move"))
				{
				CameraMesh.SetPosition(vec3((-1) * Camera.position.x, (-1) * Camera.position.y, -(1) * Camera.position.z));
				}

				ImGui::InputFloat3("test", v3);

				if (ImGui::Button("SetColor"))
				{
				CameraMesh.Color = vec3(v3[0], v3[1], v3[1]);
				}*/
			}
		}
	};

	CGUIManager GuiFpsCameraInfo;

	// Подготовка к рендеру того что будет в GUI
	inline void GUIlogics()  {	GuiFpsCameraInfo.Scene();	}


	void LoadData()
	{

		camera.movespeed = 7.5f;

		camera.SetPerspective(70, (float)SizeWidth / (float)SizeHeight, 0.1f, 256000.f);
		camera.SetPosition({ 30.f, 30.f, 30.f });
		camera.rotate({1.f, 1.f, 0});
		camera.update_camera_vectors();
		camera.update_look();

		/*Camera.type = CCamera::CameraType::firstperson;
		Camera.movementSpeed = 7.5f;
		Camera.setTranslation({ -15.0f, 13.5f, 0.0f });
		Camera.setRotation(glm::vec3(35.0f, 35.0f, 0.0f));
		Camera.setPerspective(70, (float)SizeWidth / (float)SizeHeight, 0.1f, 256000.0f);
		vec3 startpos = vec3(1.5f, -2.1f, -3.9f);
		Camera.matrices.view = glm::lookAt(startpos, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
		Camera.setPosition(startpos);
		Camera.updateViewMatrix();*/

		WorldModel = camera;
		GuiFpsCameraInfo = camera;
		GuiFpsCameraInfo = *this;

		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		window_flags |= ImGuiWindowFlags_NoCollapse;

		ImGuiStyle& style = ImGui::GetStyle();

		style.ChildWindowRounding = 3.f;
		style.GrabRounding = 0.f;
		style.WindowRounding = 0.f;
		style.ScrollbarRounding = 3.f;
		style.FrameRounding = 3.f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);

		CPath Path;
		CFile VertexShader, FragmentShader, ImageFile, Model;

		VertexShader.OpenFile(Path.GetPathDir(L"\\..\\shader\\%d%d0\\pos.vsh", VersionOpenGLMajor, VersionOpenGLMinor));
		FragmentShader.OpenFile(Path.GetPathDir(L"\\..\\shader\\%d%d0\\pos.fsh", VersionOpenGLMajor, VersionOpenGLMinor));

		ShaderGroup.OpenGroup(VertexShader,   shader_type_t::vertex);
		ShaderGroup.OpenGroup(FragmentShader, shader_type_t::fragment);

		ShaderGroup.AttachGroup();

		VertexShader.CloseFile();
		FragmentShader.CloseFile();

		VertexShader.OpenFile(Path.GetPathDir(L"\\..\\shader\\%d%d0\\gridshader.vsh", VersionOpenGLMajor, VersionOpenGLMinor));
		FragmentShader.OpenFile(Path.GetPathDir(L"\\..\\shader\\%d%d0\\gridshader.fsh", VersionOpenGLMajor, VersionOpenGLMinor));

		GridShaderGroup.OpenGroup(VertexShader,   shader_type_t::vertex);
		GridShaderGroup.OpenGroup(FragmentShader, shader_type_t::fragment);

		GridShaderGroup.AttachGroup();

		VertexShader.CloseFile();
		FragmentShader.CloseFile();
		
		ilInit();
		iluInit();
		ilutInit();

		Mesh.Vao.Bind();
		COpenTexture t;
		Mesh.ConvertInSphere(5, 32, 32, vec3(0.5f, 0.5f, 0.8f));

		DrawMesh = camera;
		DrawMesh.ConvertInSphere(5, 32, 32, vec3(0.5f, 0.5f, 0.8f));

		t.OpenTexture(Path.GetPathDir(L"\\..\\data\\null32x32.bmp"), &Mesh.Texture);
		Mesh.Vao.Unbind();

		VertexShader.OpenFile(Path.GetPathDir(L"\\..\\shader\\%d%d0\\pos.vsh", VersionOpenGLMajor, VersionOpenGLMinor));
		Shader.Open(VertexShader, shader_type_t::vertex);
		VertexShader.CloseFile();

		Mesh = camera;
		Line = camera;
		Grid = camera;

		Line.isRender = true;

		Line.Vao.Bind();
		Line.StartVertex();

		Line.AddVertex(vec3(15, 80, 69), vec2(), vec3(), {1.0f, 0,0});
		Line.AddVertex(vec3(45, 80, 69), vec2(), vec3(), { 1.0f, 0,0 });

		Line.EndVertex();
		Line.Vao.Unbind();

	

		Grid.Vao.Bind();
		Grid.StartVertex();

		uint16_t ind = 0;
		for (int32_t i = -GridSettings.CountIt; i <= GridSettings.CountIt; i++)
		{
			Grid.AddVertex(vec3(-GridSettings.Size, 0, i * GridSettings.Step), vec2(), vec3(), GridSettings.Color);
			Grid.AddVertex(vec3(GridSettings.Size, 0,  i * GridSettings.Step), vec2(), vec3(), GridSettings.Color);

			Grid.AddVertex(vec3(i * GridSettings.Step, 0, -GridSettings.Size), vec2(), vec3(), GridSettings.Color);
			Grid.AddVertex(vec3(i * GridSettings.Step, 0 , GridSettings.Size), vec2(), vec3(), GridSettings.Color);
		}

		Grid.EndVertex();
		Grid.Vao.Unbind();

		CPath PathClient;
		CGroupMesh m_Model;

		GlobalRender.Clear();
	
		m_Model = camera;
		m_Model.LoadModel(Path.GetPathDir(L"\\..\\data\\model\\base_engine\\move_edit.obj"), PathClient.GetPathDir(L"\\..\\data\\"));
		GlobalRender.Add(m_Model);
		WorldModel.AddGroup(GlobalRender);

		Mesh.ConvertInSphere(1, 32, 32, vec3(0.5f, 0.5f, 0.8f));

		for (size_t i = 0; i < 50; i++)
		{
			GlobalRender.Clear();
			m_Model.Clear();

			Mesh.SetPosition({ i * 4, 0, 0 });
	
			m_Model.AddMesh(Mesh);
			GlobalRender.Add(m_Model);
			WorldModel.AddGroup(GlobalRender);
		}

	}

	CDrawMesh DrawMesh;

	CPlane  ObjModel;
	float   ObjSize = 2.0f;
	int32_t size_x = 1;
	int32_t size_z = 1;
	vec3    ObjColor = vec3(0.97f, 0.9f, 0.9f);

	CMesh   Mesh;
	CMesh   Line = GL_LINES;

	CMesh   MultiSelectRectangle[4] = {{ GL_LINES, false}, { GL_LINES, false }, { GL_LINES, false }, { GL_LINES, false }};

	CShader Shader;
	CShader ShaderGroup;
	CShader GridShaderGroup;

	struct grid_t
	{
		int32_t Size = 256;
		int32_t Step = 8;
		int32_t CountIt = (Size / Step);

		vec3 Color = vec3(0.93f, 0.9f, 0.9f);
	};

	CMesh   Grid = GL_LINES;
	grid_t  GridSettings;

	engine::camera camera;

	
	CError Draw()
	{
		CError Result;

		Grid.Render(&GridShaderGroup);
		WorldModel.Render(&ShaderGroup);

		for (size_t i = 0; i < 4; i++)		
			MultiSelectRectangle[i].Render(&ShaderGroup, camera);

		return Result;
	}

	bool Move = false;
	bool window_gui = true;
	ImGuiWindowFlags window_flags = 0;

	CRender     GlobalRender;
	CWorldModel WorldModel;
	
	float AngleView = 70;             // Угол обзора
	float MinSpaceDistance = 0.001f;  // Минимальная  дальность, которую видит камера
	float MaxSpaceDistance = 2560000;  // Максимальная дальность, которую видит камера
	
	bool addfont = false;

	CError Render()
	{
		CError Result;
		float  frameTimer = 0.0f;

		do { 

			glViewport(0, 0, SizeWidth, SizeHeight);
			camera.SetPerspective(AngleView, (float)SizeWidth / (float)SizeHeight, MinSpaceDistance, MaxSpaceDistance);

			glfwPollEvents();
			ImGui_ImplGlfwGL3_NewFrame();

			GUIlogics();

			auto tStart = std::chrono::high_resolution_clock::now();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			if (isWireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
			Result = Draw();

			if (isWireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			if (Result)
				return Result;

			auto tEnd  = std::chrono::high_resolution_clock::now();
			auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
			frameTimer = (float)tDiff / 1000.0f;

			camera.move(frameTimer);
			
			ImGui::Render();

			glfwSwapBuffers(window);
			
			if (!addfont)		
				addfont = true;
			

		} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

		return Result;
	}


	void EndWork()
	{
		glfwTerminate();
	}

	std::string TitleApp = "App";
	std::stringstream TitleStream;
	int32_t SizeWidth = 0;
	int32_t SizeHeight = 0;
	CFps    Fps;
	GLFWwindow* window;
	bool isWireframe = false;
};
