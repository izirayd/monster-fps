#pragma once


#include "graphic_base.hpp"
#include "error.hpp"

#include "shader.hpp"
#include "path.hpp"
#include "file.hpp"
#include "camera.hpp"

//#define _USE_MATH_DEFINES
#include <math.h>

namespace std { template<class T> inline void clear(std::vector<T> &vec) { std::vector<T>().swap(vec); } }

uint32_t GetIndex(uint32_t pos_x, uint32_t pos_y, uint32_t size_x, uint32_t numerate_start = 0)
{
	size_x++;
	uint32_t result = pos_x + numerate_start + pos_y * size_x;
	return result;
}

class CGroupMesh;

struct vertex_t
{
	vertex_t() = default;
	vertex_t(const vec3 pos, const vec2 texcord) {	position = pos; texture = texcord;	}
	vertex_t(const vec3 pos, const vec2 texcord, const vec3 norm) {	position = pos; texture = texcord; normal = norm; }
	vertex_t(const vec3 pos, const vec2 texcord, const vec3 norm, const vec3 _color) { position = pos; texture = texcord; normal = norm; color = _color; }

	vec3 position;
	vec2 texture;
	vec3 normal;
	vec3 color;
};

class CVAOManager
{
   public:

	   uint32_t index = 0;
	   bool isLoad = false;
	   inline operator uint32_t() { return index; }

	   void Create()
	   {
		   if (isLoad)
			   return;

		   glGenVertexArrays(1, &index);
		   isLoad = true;
	   }

	   void Delete()
	   {
		   glDeleteVertexArrays(1, &index);
		   isLoad = false;
	   }


	   bool    isOk() const
	   {
		   return glIsVertexArray(index);
	   }

	   void    Bind()
	   {
		   if (!isLoad)
			   Create();

		   glBindVertexArray(index);
	   }

	   void    Unbind()
	   {
		   glBindVertexArray(0);
	   }
};

class CBaseMesh
{
public:
	uint32_t index = 0;
	uint32_t count = 0;
	int32_t  typeBuffer = GL_ARRAY_BUFFER;

	CBaseMesh() = default;
	CBaseMesh(int32_t type)
	{
		typeBuffer = type;
	}

	bool isLoad = false;

	inline operator uint32_t() { return index; }

	void TypeElementArrayBuffer()
	{
		typeBuffer = GL_ELEMENT_ARRAY_BUFFER;
	}

	void TypeArrayBuffer()
	{
		typeBuffer = GL_ARRAY_BUFFER;
	}

	void Create()
	{
		if (!isLoad)
		{
			glGenBuffers(1, &index);
			isLoad = true;
		}
	}

	void Delete()
	{
		if (isLoad) {
			glDeleteBuffers(1, &index);
			isLoad = false;
		}
	}

	inline void Send(const std::vector<vertex_t> m_Vertices)
	{
		if (m_Vertices.size() == 0)
			return;

		Send(m_Vertices.size(), m_Vertices.size() * sizeof(vertex_t), &m_Vertices[0]);
	}


	inline void Send(const std::vector<uint32_t> m_Indices)
	{
		if (m_Indices.size() == 0)
			return;

		Send(m_Indices.size(), m_Indices.size() * sizeof(uint32_t), &m_Indices[0]);
	}

	void Send(const uint32_t Count, const uint32_t Size, const void *point)
	{
		Create();
		count = Count;
		Bind();
		glBufferData(typeBuffer, Size, point, GL_STATIC_DRAW);
		Unbind();
	}

	inline void Bind()   { glBindBuffer(typeBuffer, index); }
	inline void Unbind() { glBindBuffer(typeBuffer, 0);     }

};

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

class CDrawMesh
{
public:

	uint32_t count = 0;
	int32_t  typeBuffer = GL_ARRAY_BUFFER;

	uint32_t m_Buffers[4];
	uint32_t m_index_buffer_ind;

	CVAOManager Vao;

	CDrawMesh() = default;
	CDrawMesh(int32_t type)
	{
		typeBuffer = type;
	}

	bool isLoad = false;


	void TypeElementArrayBuffer()
	{
		typeBuffer = GL_ELEMENT_ARRAY_BUFFER;
	}

	void TypeArrayBuffer()
	{
		typeBuffer = GL_ARRAY_BUFFER;
	}

	void Create()
	{
		if (!isLoad)
		{
			glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
			glGenBuffers(1, &m_index_buffer_ind);
			isLoad = true;
		}
	}

	void Delete()
	{
		if (isLoad) {
			glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
			glDeleteBuffers(1, &m_index_buffer_ind);
			isLoad = false;		
		}
	}

	struct model_t
	{
		std::vector<vec3>     Positions;
		std::vector<vec2>     TexCord;
		std::vector<vec3>     Normales;
		std::vector<vec3>     Colors;
		std::vector<uint32_t> Indices;

		void Add(vec3 pos, vec2 tex, vec3 nor, vec3 col, uint32_t ind)
		{
			Positions.push_back(pos);
			TexCord.push_back(tex);
			Normales.push_back(nor);
			Colors.push_back(col);
			Indices.push_back(ind);
		}

		void Add(vec3 pos, vec2 tex, vec3 nor, vec3 col)
		{
			Positions.push_back(pos);
			TexCord.push_back(tex);
			Normales.push_back(nor);
			Colors.push_back(col);
		}

		void Add(uint32 ind)
		{
			Indices.push_back(ind);
		}

		void Add(vertex_t v, uint32_t i)
		{
			Add(v.position, v.texture, v.normal, v.color, i);
		}

		void Clear()
		{
			Positions.clear(); Positions.resize(0);
			TexCord.clear();   TexCord.resize(0);
			Normales.clear();  Normales.resize(0);
			Colors.clear();    Colors.resize(0);
			Indices.clear();   Indices.resize(0);
		}
	};

	void allocate_gpu(
		model_t model

	)
	{
		Vao.Create();
		Create();
		Bind();

		CountVertex  = model.Positions.size();
		CountIndices = model.Indices.size();

		glBufferData(GL_ARRAY_BUFFER, sizeof(model.Positions[0]) * model.Positions.size(), &model.Positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBufferData(GL_ARRAY_BUFFER, sizeof(model.TexCord[0]) * model.TexCord.size(), &model.TexCord[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBufferData(GL_ARRAY_BUFFER, sizeof(model.Normales[0]) * model.Normales.size(), &model.Normales[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBufferData(GL_ARRAY_BUFFER, sizeof(model.Colors[0]) * model.Colors.size(), &model.Colors[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

	
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model.Indices[0]) * model.Indices.size(), &model.Indices[0], GL_STATIC_DRAW);

		Unbind();
		Vao.Unbind();
	}

	void ConvertInSphere(float radius, uint32_t rings, uint32_t sectors, vec3 _color = vec3())
	{
		model_t model;

		auto R = 1.0f / (float)(rings - 1);
		auto S = 1.0f / (float)(sectors - 1);

		for (uint32_t r = 0; r < rings; r++)
			for (uint32_t s = 0; s < sectors; s++)
			{
				auto y = sin(-M_PI_2 + M_PI * r * R);
				auto x = cos(2.0f * M_PI * s * S) * sin(M_PI * r * R);
				auto z = sin(2.0f * M_PI * s * S) * sin(M_PI * r * R);

				model.Add(vec3(x, y, z) * radius, vec2(r * R, s * S), vec3(x, y, z), _color);
			}

		for (uint32_t r = 0; r < rings - 1; r++)
			for (uint32_t s = 0; s < sectors - 1; s++) {

				model.Add(s + r * sectors);
				model.Add(s + (r + 1) * sectors);
				model.Add((s + 1) + (r + 1) * sectors);

				model.Add(s + r * sectors);
				model.Add((s + 1) + (r + 1) * sectors);
				model.Add((s + 1) + r * sectors);
			}

		allocate_gpu(model);
		model.Clear();
	}

	uint32_t CountIndices = 0;
	uint32_t CountVertex  = 0;


	engine::camera *Camera;

	CDrawMesh operator= (engine::camera *NewCamera)
	{
		Camera = NewCamera;
		return *this;
	}

	CDrawMesh operator= (engine::camera &NewCamera)
	{
		Camera = &NewCamera;
		return *this;
	}

	int32_t pos_index;
	int32_t tex_index;
	int32_t nor_index;
	int32_t col_index;
	int32_t texture_index;
	int32_t cam_v, cam_p;
	int32_t model_index;

	bool isLoadNamesShader = false;

	mat4 Model;

	void Render(CShader *Shader)
	{
		Vao.Bind();
		Shader->Bind();

		if (!isLoadNamesShader)
		{
			pos_index = Shader->GetAttribute("v3Position");
			tex_index = Shader->GetAttribute("v2Texcoord");
			nor_index = Shader->GetAttribute("v3Normal");
			col_index = Shader->GetAttribute("v3Color");
			texture_index = Shader->GetUniform("ColorMap");

			cam_v = Shader->GetUniform("CameraV");
			cam_p = Shader->GetUniform("CameraP");
			model_index = Shader->GetUniform("Model");

			isLoadNamesShader = true;
		}

		if (cam_v > 0)
			glUniformMatrix4fv(cam_v, 1, GL_FALSE, glm::value_ptr(Camera->mvp.view));

		if (cam_p > 0)
			glUniformMatrix4fv(cam_p, 1, GL_FALSE, glm::value_ptr(Camera->mvp.proj));

		if (model_index > 0)
			glUniformMatrix4fv(model_index, 1, GL_FALSE, glm::value_ptr(this->Model));

		// TODO: переписать под vertex_t вид и передавать шейдеры
		// еще сделать это https://triplepointfive.github.io/ogltutor/tutorials/tutorial33.html


		glDrawElementsBaseVertex(GL_TRIANGLES,
			CountIndices,
			GL_UNSIGNED_INT,
			0,
			CountVertex / 2);
	
		Shader->Unbind();
		Vao.Unbind();
	}


	inline void Bind()   {
	
		for (size_t i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_Buffers) - 1; i++)	
			glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_ind);
		
	}

	inline void Unbind() { glBindBuffer(typeBuffer, 0);     }

};


class CMesh
{
public:
	uint32_t  index;
	uint32_t  textureindexmesh;
	CTexture  Texture;

	CGroupTexture GroupTexture;

	void AddTexture(COpenTexture *Texture)	{	*this = Texture->ResultTexture;	}
	void AddTexture(CTexture *Texture)      {	GroupTexture = Texture; 	    }


	CMesh operator= (COpenTexture *Texture) { AddTexture(Texture); return *this; }

	CMesh operator= (CTexture *Texture) {
		GroupTexture = Texture;
		return *this;
	}

	CMesh operator= (CTexture &Texture)
	{
		GroupTexture = Texture;
		return *this;
	}

	CBaseMesh   Base      = GL_ARRAY_BUFFER;
	CBaseMesh   BaseIndex = GL_ELEMENT_ARRAY_BUFFER;
	CVAOManager Vao;


	~CMesh()
	{

	}

	CMesh() {

	}

	// Удаляет объект из памяти GPU
	void delete_allocate_in_gpu()
	{
		Base.Delete();
		BaseIndex.Delete();
		Vao.Delete();

	}

	CMesh(const int32_t m_RenderType)	{	

		RenderType = m_RenderType;	
	}

	CMesh(const int32_t m_RenderType, bool render) {
		isRender = render;
		RenderType = m_RenderType;
	}

	void Start() {
		delete_allocate_in_gpu();
		Vao.Bind();
		StartVertex();
	}

	void End()
	{
		EndVertex();
		Vao.Unbind();
	}

	int32_t RenderType = GL_TRIANGLES;

	std::vector<vertex_t>       m_Vertices;
	std::vector<uint32_t>       m_Indices;

	vec3 VectorMove;

	private:

	vec3 Position = { 0.0, 0.0, 0.0 };
	vec3 Scale    = { 1.0, 1.0, 1.0 };
	quat Rotation; 

    public:
	inline vec3 GetPosition() const { return Position; }
	inline quat GetRotation() const { return Rotation; }
	inline vec3 GetScale()    const { return Scale;    }

	void ApplyPosition(vec3 pos)   {	SetPosition(GetPosition() + pos);}

	void BuildMatrixModel()	{  
		Model = glm::translate(mat4(), GetPosition()) * glm::mat4_cast(GetRotation()) *	glm::scale(glm::mat4(), GetScale());
	}

	inline void SetRotatation(quat newRotation) {
		Rotation = newRotation;
		BuildMatrixModel();
	}

	inline void SetPosition(vec3 newPos)	{
		Position = newPos;
		BuildMatrixModel();
	}

	inline void SetScale(vec3 newScale)	{
		Scale = newScale;
		BuildMatrixModel();
	}

	inline void ApplyScale(vec3 AddScale)	{
		Scale += AddScale;
		BuildMatrixModel();
	}

	inline void DisableRender() { isRender = false; }
	inline void EnableRender()  { isRender = true;  }

	bool isRender = true;

	float mass = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	std::vector<vertex_t> BufferVertex;
	std::vector<uint32_t> BufferIndices;

	engine::camera *Camera;

	CMesh operator= (engine::camera *NewCamera)
	{
		Camera = NewCamera;
		return *this;
	}

	CMesh operator= (engine::camera &NewCamera)
	{
		Camera = &NewCamera;
		return *this;
	}


	inline operator uint32_t() { return index; }


	int32_t pos_index = -1;
	int32_t tex_index = -1;
	int32_t nor_index = -1;
	int32_t col_index = -1;
	int32_t texture_index = -1;
	int32_t cam_v = -1, cam_p = -1;
	int32_t model_index = -1;
	int32_t meshcolor_index = -1;

	int32_t texture_diffuse[3]  = { -1, -1, -1 };
	int32_t texture_specular[3] = { -1, -1, -1 };

	vec3    Color;
	
	bool isLoadNamesShader = false;

	void Draw(CShader *Shader)
	{
		if (!isLoadNamesShader) 
		{
			 pos_index     = Shader->GetAttribute("v3Position");
			 tex_index     = Shader->GetAttribute("v2Texcoord");
			 nor_index     = Shader->GetAttribute("v3Normal");
			 col_index     = Shader->GetAttribute("v3Color");

			 cam_v         = Shader->GetUniform("CameraV");
			 cam_p         = Shader->GetUniform("CameraP");
			 model_index   = Shader->GetUniform("Model");

			 meshcolor_index = Shader->GetUniform("v3Meshcolor");

			 if (GroupTexture.ListTexture.size() > 0) {

				 texture_diffuse[0] = Shader->GetUniform("texture_diffuse0");
				 texture_diffuse[1] = Shader->GetUniform("texture_diffuse1");
				 texture_diffuse[2] = Shader->GetUniform("texture_diffuse2");

				 texture_specular[0] = Shader->GetUniform("texture_specular0");
				 texture_specular[1] = Shader->GetUniform("texture_specular1");
				 texture_specular[2] = Shader->GetUniform("texture_specular2");
			 }

			 isLoadNamesShader = true;
		}

		if (cam_v > -1)
		glUniformMatrix4fv(cam_v, 1, GL_FALSE, glm::value_ptr(Camera->mvp.view));

		if (cam_p > -1)
		glUniformMatrix4fv(cam_p, 1, GL_FALSE, glm::value_ptr(Camera->mvp.proj));

		if (model_index > -1)
		glUniformMatrix4fv(model_index, 1, GL_FALSE, glm::value_ptr(this->Model));

		if (meshcolor_index > -1)
		glUniform3fv(meshcolor_index, 1, glm::value_ptr(Color));
	
		Vao.Bind();
		Base.Bind();
		BaseIndex.Bind();
		
		if (pos_index > -1) {
			glEnableVertexAttribArray(pos_index);
			glVertexAttribPointer(pos_index, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)0);
		}

		if (tex_index > -1) {
			glEnableVertexAttribArray(tex_index);
			glVertexAttribPointer(tex_index, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)(sizeof(glm::vec3)));
		}

		if (nor_index > -1) {
			glEnableVertexAttribArray(nor_index);
			glVertexAttribPointer(nor_index, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
		}

		if (col_index > -1)	{
			glEnableVertexAttribArray(col_index);
			glVertexAttribPointer(col_index, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec3)));
		}

		if (texture_diffuse[0] > -1)
		{
			int32_t i = 0;
			int32_t it_texture_dif = 0, it_texture_spec = 0;

			for (auto &it : GroupTexture.ListTexture)
				if (it.isLoad) 		{
					it.Bind(i);

					if (it.type == type_texture_t::diffuse)				
						if (texture_diffuse[it_texture_dif] > -1)
						  glUniform1i(texture_diffuse[it_texture_dif++], i);
					
					if (it.type == type_texture_t::special)				
						if (texture_specular[it_texture_spec] > -1)
						 glUniform1i(texture_specular[it_texture_spec++], i);
				
					i++;
				}
		}

		if (BaseIndex.count > 0)
			glDrawElements(RenderType, BaseIndex.count, GL_UNSIGNED_INT, (void*)0);
		else
			glDrawArrays(RenderType, 0, Base.count);

		if (pos_index > -1)
			glDisableVertexAttribArray(pos_index);

		if (tex_index > -1)
			glDisableVertexAttribArray(tex_index);

		if (nor_index > -1)
			glDisableVertexAttribArray(nor_index);

		if (col_index > -1)	
			glDisableVertexAttribArray(col_index);
		
		GroupTexture.Unbind();
		Base.Unbind();
		BaseIndex.Unbind();
		Vao.Unbind();
	}

	mat4 Model;
	mat4 MvpBase;


	void Render(CShader *Shader, engine::camera &camera)
	{
		if (!isRender) return;
		this->Camera = &camera;

		Shader->Bind();
		Draw(Shader);
		Shader->Unbind();
	}

	void Render(CShader *Shader)
	{
		if (!isRender) return;

		Shader->Bind();
		 Draw(Shader);
		Shader->Unbind();
	}

	inline void StartVertex()
	{
		BufferVertex.clear();
		BufferVertex.resize(0);
		BufferVertex.reserve(0);

		std::clear(BufferVertex);
	}

	void AddVertex(const vec3 position, const vec2 texcord = vec2(), const vec3 normal = vec3(), const vec3 _color = vec3())
	{
		vertex_t v(position, texcord, normal, _color);
		BufferVertex.push_back(v);
	}

	inline void EndVertex()
	{
		SetVertices(BufferVertex);

		BufferVertex.clear();
		BufferVertex.resize(0);
		BufferVertex.reserve(0);
		std::clear(BufferVertex);

		m_Vertices.clear();
		m_Vertices.resize(0);
		m_Vertices.reserve(0);
		std::clear(m_Vertices);
	}

	// Добавляем лист вершин
	void AddVertices(const std::vector<vertex_t> val)
	{
		m_Vertices.reserve(m_Vertices.size() + val.size());
		m_Vertices.insert(m_Vertices.end(), val.begin(), val.end());

		Base.Send(val);
	}

	void SetVertices(const std::vector<vertex_t> val)
	{
		m_Vertices = val;
		Base.Send(m_Vertices);
	}

	void AddIndices(const std::vector<uint32_t> val)
	{
		m_Indices.reserve(m_Indices.size() + val.size());
		m_Indices.insert(m_Indices.end(), val.begin(), val.end());

		BaseIndex.Send(val);
	}

	void SetIndices(const std::vector<uint32_t> val)
	{
		m_Indices = val;
		BaseIndex.Send(val);
	}

	inline void StartIndices()
	{
		BufferIndices.clear();
		BufferIndices.reserve(0);
		BufferIndices.resize(0);

		std::clear(BufferIndices);
	}

	void AddIndices(const uint32_t val)
	{
		BufferIndices.push_back(val);
	}

	inline void EndIndices()
	{
		SetIndices(BufferIndices);

		BufferIndices.clear();
		BufferIndices.resize(0);
		BufferIndices.reserve(0); 
		std::clear(BufferIndices);

		m_Indices.clear();
		m_Indices.reserve(0);
		m_Indices.resize(0);
		std::clear(m_Indices);
	}


	void ConvertInSphere(const float radius, const uint32_t rings, const uint32_t sectors, const vec3 _color = vec3())
	{
		StartVertex();

		auto R = 1.0f / (float)(rings - 1);
		auto S = 1.0f / (float)(sectors - 1);

		for (uint32_t r = 0; r < rings; r++)
			for (uint32_t s = 0; s < sectors; s++)
			{
				auto y = sin(-M_PI_2 + M_PI * r * R);
				auto x = cos(2.0f * M_PI * s * S) * sin(M_PI * r * R);
				auto z = sin(2.0f * M_PI * s * S) * sin(M_PI * r * R);

				AddVertex(vec3(x, y, z) * radius, vec2(r * R, s * S), vec3(x, y, z), _color);
			}

		EndVertex();


		StartIndices();

		for (uint32_t r = 0; r < rings - 1; r++)
			for (uint32_t s = 0; s < sectors - 1; s++) {

				AddIndices(s + r * sectors);
				AddIndices(s + (r + 1) * sectors);
				AddIndices((s + 1) + (r + 1) * sectors);

				AddIndices(s + r * sectors);
				AddIndices((s + 1) + (r + 1) * sectors);
				AddIndices((s + 1) + r * sectors);
			}

		EndIndices();
	}

	void ExperimentalPnale(const float size, const uint32_t segmentH, const uint32_t segmentW, vec3 color = vec3())
	{
		delete_allocate_in_gpu();

		Vao.Bind();
		StartVertex();

		for (float pos_z = 0; pos_z <= segmentH; pos_z++)
			for (float pos_x = 0; pos_x <= segmentW; pos_x++) 
			{




			}

		EndVertex();
		Vao.Unbind();
	}


	void ConvertInPlane(const float size, const uint32_t size_x, const uint32_t size_z, vec3 color = vec3())
	{
		delete_allocate_in_gpu();
		Vao.Bind();

		StartVertex();

		printf("======\n");


		for (float pos_z = 0; pos_z <= size_z; pos_z++)
			for (float pos_x = 0; pos_x <= size_x; pos_x++) {
				//color.x = 1.0f / ((rand() % 5) + 1);
				//color.y = color.x;
				//color.z = color.x;
				//AddVertex(vec3(pos_x * size, color.x * size, pos_z * size), vec2(), vec3(), color);
				AddVertex(vec3(pos_x * size, 0, pos_z * size), vec2(), vec3(), color);
				//printf("%f %f %f %f %f\n", pos_x * size, 0, pos_z * size, pos_x, pos_z);
			}
		printf("======\n");

		BufferVertex[GetIndex(0, 0, size_x)].texture = vec2(0.0f, 0.0f);
		BufferVertex[GetIndex(size_x, 0, size_x)].texture = vec2(1.0f, 0.0f);
		BufferVertex[GetIndex(0, size_z, size_x)].texture = vec2(0.0f, 1.0f);
		BufferVertex[GetIndex(size_x, size_z, size_x)].texture = vec2(1.0f, 1.0f);

		/*BufferVertex[GetIndex(0, 0, size_x)].texture = vec2(0.0f, 0.0f);
		BufferVertex[GetIndex(1, 0, size_x)].texture = vec2(1.0f, 0.0f);
		BufferVertex[GetIndex(0, 1, size_x)].texture = vec2(0.0f, 1.0f);
		BufferVertex[GetIndex(1, 1, size_x)].texture = vec2(1.0f, 1.0f);*/


	//	BufferVertex[GetIndex(2, 3, size_x)].position.y = 1;
	//	BufferVertex[GetIndex(3, 3, size_x)].position.y = 1;

		EndVertex();

		StartIndices();
		register uint32_t index1, index2, index3, index4;

		for (uint32_t pos_z = 0; pos_z < size_z; pos_z++)
			for (uint32_t pos_x = 0; pos_x < size_x; pos_x++)
			{
				index1 = GetIndex(pos_x,     pos_z,         size_x);
				index2 = GetIndex(pos_x + 1, pos_z,         size_x);
				index3 = GetIndex(pos_x + 1, pos_z + 1,     size_x);
			    index4 = GetIndex(pos_x,     pos_z + 1,     size_x);

				AddIndices(index1);
				AddIndices(index2);
				AddIndices(index3);
				AddIndices(index1);
				AddIndices(index4);
				AddIndices(index3);
			}
		EndIndices();

		Vao.Unbind();
	}


};

class CPlane : public CMesh
{
   public:

	   // 1x1 1x1
	   void Create(uint32_t height, uint32_t width, uint32_t countsegmentH, uint32_t countsegmentW, vec3 color = vec3())
	   {
		   float size_one_segment_h = (float) height / (float)countsegmentH;
		   float size_one_segment_w = (float) width  / (float)countsegmentW;

		   delete_allocate_in_gpu();

		   Vao.Bind();
		   StartVertex();

		   for (float pos_z = 0; pos_z <= countsegmentH; pos_z++)
			   for (float pos_x = 0; pos_x <= countsegmentW; pos_x++) 
			   {
				   AddVertex(vec3(pos_x * size_one_segment_h, 0, pos_z * size_one_segment_w), vec2(), vec3(), color);
			   }

		   EndVertex();
		   Vao.Unbind();
	   }

};


class CGroupMesh
{
   public:
	   std::vector<CMesh>    ListMesh;
	   std::vector<CTexture> GroupTexture;

	   int32_t Count = 0;

	   CGroupMesh() = default;


	   CGroupMesh(engine::camera *newc) { Camera = newc; }


	   engine::camera *Camera = nullptr;

	   CError GetTexture(CTexture *Tex, uint32_t index)
	   {
		   CError Result;
		   if (index >= GroupTexture.size())
		   {
			   Tex->isLoad = false;
			   return Result = "No texture in list";
		   }

		   Tex->textureID = GroupTexture[index].textureID;
		   Tex->isLoad    = GroupTexture[index].isLoad;
		   return Result;
	   }

	   CGroupMesh operator= (engine::camera *NewCamera)
	   {
		   Camera = NewCamera;
		   return *this;
	   }

	   CGroupMesh operator= (engine::camera &NewCamera)
	   {
		   Camera = &NewCamera;
		   return *this;
	   }

	   void Render(CShader *Shader)
	   {
		   if (Camera == nullptr)
			   return;

		   for (auto &it : ListMesh) 
			     it.Render(Shader);
	   }

	   void Create(int32_t CountMesh)
	   {
		   if (CountMesh == 0) return;
		   Clear();
		   ListMesh.resize(CountMesh);
	   }
	   
	   void Clear() {
		   ListMesh.clear();
		   ListMesh.resize(0);
		   std::clear(ListMesh);
		   Count = 0;
	   }

	   void LoadModel(wchar_t *Path, wchar_t *ClientDir)
	   {
		   char CharPath[256];
		   Cpy(CharPath, Path);

		   Assimp::Importer Importer;
		   const aiScene* pScene = Importer.ReadFile(CharPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

		   if (pScene) {
			   InitFromScene(pScene, CharPath, ClientDir);
		   }
	   }


	   void InitFromScene(const aiScene* pScene, const std::string& Filename, wchar_t *ClientDir)
	   {
		   if (pScene->mNumMeshes == 0)
			   return;

		   GroupTexture.clear();

		   ilutEnable(ILUT_OPENGL_CONV);
		   // Вызовет Clear
		   Create(pScene->mNumMeshes);
		   for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
			   const aiMesh* paiMesh = pScene->mMeshes[i];
			   AddMesh(paiMesh, pScene, ClientDir);
		   }		 
	   }

	   void AddMesh(CMesh Mesh)
	   {
		   ListMesh.push_back(Mesh);
		   Count++;
	   }

	   void AddMesh(const aiMesh* paiMesh, const aiScene* pScene, wchar_t *ClientDir)
	   {
		   aiColor3D pColor(0.f, 0.f, 0.f);
		   pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);


		   ListMesh[Count].Vao.Delete();
		   ListMesh[Count].Vao.Bind();

		   ListMesh[Count] = Camera;
		   ListMesh[Count].StartVertex();

		   const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		   for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {

			   const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
			   const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
			   const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

			   //printf("%f %f %f %f %f\n", pPos->x, pPos->y, pPos->z, pTexCoord->x, pTexCoord->y);

			   ListMesh[Count].AddVertex(
				   vec3(pPos->x, pPos->y, pPos->z),
				   vec2(pTexCoord->x, pTexCoord->y),
				   vec3(pNormal->x, pNormal->y, pNormal->z),
				   vec3(pColor.r, pColor.g, pColor.b)
			   );
		   }

		   ListMesh[Count].EndVertex();

		   ListMesh[Count].StartIndices();

		   for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
		   {
			   const aiFace& Face = paiMesh->mFaces[i];
			 
			   /*assert(Face.mNumIndices == 3);
			   ListMesh[Count].AddIndices(Face.mIndices[0]);
			   ListMesh[Count].AddIndices(Face.mIndices[1]);
			   ListMesh[Count].AddIndices(Face.mIndices[2]);*/

			   for (GLuint j = 0; j < Face.mNumIndices; j++)
				   ListMesh[Count].AddIndices(Face.mIndices[j]);
		   }

		   ListMesh[Count].EndIndices();
		   ListMesh[Count].textureindexmesh = paiMesh->mMaterialIndex;
		 	 
		  // GetTexture(&ListMesh[Count].Texture, 0);

		   ListMesh[Count].Vao.Unbind();
			  
		   if (paiMesh->mMaterialIndex >= 0)
		   {
			   aiMaterial* material = pScene->mMaterials[paiMesh->mMaterialIndex];

			   if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {

				   wchar_t tmp[1024];
				   Cpy(tmp, ClientDir);
				   COpenTexture OTexture;

				   for (GLuint i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
				   {
					   aiString Path;
					   material->GetTexture(aiTextureType_DIFFUSE, i, &Path);

					   CTexture Texture;
					   Cat(tmp, Path.data);
					   OTexture.OpenTexture(tmp, &Texture);

					   Texture.type = type_texture_t::diffuse;
					   ListMesh[Count].GroupTexture.ListTexture.push_back(Texture);
				   }
			   }

			   if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
			   {
				   wchar_t tmp[1024];
				   Cpy(tmp, ClientDir);
				   COpenTexture OTexture;

				   for (GLuint i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR); i++)
				   {
					   aiString Path;
					   material->GetTexture(aiTextureType_SPECULAR, i, &Path);

					   CTexture Texture;
					   Cat(tmp, Path.data);
					   OTexture.OpenTexture(tmp, &Texture);

					   Texture.type = type_texture_t::diffuse;
					   ListMesh[Count].GroupTexture.ListTexture.push_back(Texture);
				   }
			   }
		   }

		   Count++;
	   }

};