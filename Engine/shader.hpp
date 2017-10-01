#pragma once

#include "base.hpp"
#include "error.hpp"
#include "file.hpp"
#include "graphic_base.hpp"
#include <glm\gtc\type_ptr.hpp>

enum class shader_type_t
{
	vertex   ,
	fragment ,
	geometry ,
	compute  ,
	tess_control ,
	tess_evalution 

};

class CFileShader
{
public:
	inline operator bool()     { return isLoad; }
	inline operator uint32_t() { return index;  }

	void Init(shader_type_t shader_type)
	{
		switch (shader_type)
		{
		case shader_type_t::vertex:
			index = glCreateShader(GL_VERTEX_SHADER);
			break;
		case shader_type_t::fragment:
			index = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		case shader_type_t::geometry:
			index = glCreateShader(GL_GEOMETRY_SHADER);
			break;
		case shader_type_t::compute:
			index = glCreateShader(GL_COMPUTE_SHADER);
			break;
		case shader_type_t::tess_control:
			index = glCreateShader(GL_TESS_CONTROL_SHADER);
			break;
		case shader_type_t::tess_evalution:
			index = glCreateShader(GL_TESS_EVALUATION_SHADER);
			break;
		default:
			break;
		}
	}

	CFileShader() = default;

	CError BaseCodeCompile(CFile ShaderFile) 	{
		return BaseCodeCompile(index, ShaderFile);
	}

	CError BaseCodeCompile(uint32_t &IndexShader, CFile ShaderFile)
	{
		CError Result;

		if (!ShaderFile)
			return Result.format("Cannot open Shader: %ls", ShaderFile.wfilename.c_str());

		if (ShaderFile.Size == 0)
			return Result.format("File shader is empty Shader: %ls", ShaderFile.wfilename.c_str());

		char *CodeShader = new char[ShaderFile.Size + 1];

		ShaderFile.FullReadFile(CodeShader, sizeof(char));
		CodeShader[ShaderFile.Size] = 0x00;
		glShaderSource(IndexShader, 1, &CodeShader, NULL);
		glCompileShader(IndexShader);

		int32_t StatusCompileShader;

		glGetShaderiv(IndexShader, GL_COMPILE_STATUS, &StatusCompileShader);

		if (StatusCompileShader == GL_FALSE) {
			int32_t InfoLogLength = 0;
			char Buffer[256];

			glGetShaderiv(IndexShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
			glGetShaderInfoLog(IndexShader, InfoLogLength, NULL, Buffer);
			glDeleteShader(IndexShader);

			Result.format("Shader parser: %s", Buffer);		
			delete[] CodeShader;
			isLoad = false;
			return Result;
		}

		delete[] CodeShader;
		isLoad = true;
		return Result;
	}

	void Delete()
	{
		glDeleteShader(index);
		isLoad = false;
	}

	bool      isLoad = false;
	uint32_t  index  = 0;
};

class CBaseShader
{
  public:
	  uint32_t  index = 0;
	  inline operator uint32_t() { return index; }

	  void ClearGroup()
	  {
		  GroupShader.clear();
	  }


	  CError AddGroup(CFile File, shader_type_t shader_type)
	  {
		  CError Result;
		  CFileShader m_Shader;

		  m_Shader.Init(shader_type);
		  Result = m_Shader.BaseCodeCompile(File);

		  if (Result)
			  return Result;

		  GroupShader.push_back(m_Shader);

		  return Result;
	  }

	  CError AttachGroup()
	  {
		  CError Result;

		  if (GroupShader.size() == 0)
			  return Result.format("Shader empty!");

		  index = glCreateProgram();

		  for (auto it : GroupShader)
		  {
			  if (it.isLoad)
			   glAttachShader(index, it.index);
		  }

		  glLinkProgram(index);

		  int32_t Check, info;
		  glGetProgramiv(index, GL_LINK_STATUS, &Check);
		  glGetProgramiv(index, GL_INFO_LOG_LENGTH, &info);

		  if (info > 0) {
			  std::vector<char> ProgramErrorMessage(info + 1);
			  glGetProgramInfoLog(index, info, NULL, &ProgramErrorMessage[0]);

			  return Result = &ProgramErrorMessage[0];
		  }

		  for (auto it : GroupShader)
		  {
			  glDetachShader(index, it);
			  it.Delete();
		  }

		  return Result;
	  }

	  CError Open(CFile File, shader_type_t shader_type)
	  {
		  CError Result;

		  GroupShader.clear();
		 
		  CFileShader m_Shader;

		  m_Shader.Init(shader_type);
		  Result = m_Shader.BaseCodeCompile(File);

		  if (Result)	  
			  return Result;

		  GroupShader.push_back(m_Shader);

		  return AttachShader();
	  }

	  CError AttachShader()
	  {
		  CError Result;

		  if (GroupShader.size() == 0)
			  return Result.format("Shader empty!");

		  index = glCreateProgram();

		  glAttachShader(index, GroupShader[0]);
		  glLinkProgram(index);


		  int32_t Check, info;
		  glGetProgramiv(index, GL_LINK_STATUS, &Check);
		  glGetProgramiv(index, GL_INFO_LOG_LENGTH, &info);

		  if (info > 0) {
			  std::vector<char> ProgramErrorMessage(info + 1);
			  glGetProgramInfoLog(index, info, NULL, &ProgramErrorMessage[0]);
	
			  return Result = &ProgramErrorMessage[0];
		  }

		  glDetachShader(index, GroupShader[0]);

		  GroupShader[0].Delete();

		  return Result;
	  }

	  std::vector<CFileShader> GroupShader;

	 // CFileShader FileShader;
};

class CShader
{
public:

	void Bind() {
		glUseProgram(BaseShader.index);
	}

	void Unbind() {
		glUseProgram(0);
	}


	inline CError   OpenGroup(CFile File, shader_type_t shader_type) { return BaseShader.AddGroup(File, shader_type); }
	inline CError   AttachGroup()                                    { return BaseShader.AttachGroup();  }
	inline CError   Open(CFile File, shader_type_t shader_type)	{	return BaseShader.Open(File, shader_type);              }
	inline int32_t  GetAttribute(std::string Name)	            {
		int32_t index = glGetAttribLocation(BaseShader, Name.c_str());
		CError  Result;
		if (index == -1) Result.format("Get in %s error for \"%s\" , because \"%s\" don`t create in shader", __FUNCTION__, Name.c_str(), Name.c_str());
		return index;
	}
	inline int32_t  GetUniform(std::string Name)                {
		int32_t index = glGetUniformLocation(BaseShader, Name.c_str());
		CError  Result;
		 if (index == -1) Result.format("Get in %s error for \"%s\" , because \"%s\" don`t create in shader", __FUNCTION__, Name.c_str(), Name.c_str());
		return index;
	}
	
	CError   SetInteger(std::string Name, uint32_t val)  {  
		
		CError  Result;
		int32_t index = GetIndex(Name);

		if (index == -1)
			return Result.format("Set %s error edit \"%s\" for value, because \"%s\" don`t create in shader", __FUNCTION__, Name.c_str(), Name.c_str());
		
		glUniform1i(index, val);

	    GLenum g_OpenGLError; 
		if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) 	
			Result.format("OpenGL expression %s error 0x%X for %s. Text Error: %s\n", __FUNCTION__, (unsigned)g_OpenGLError, Name.c_str(), glewGetErrorString(g_OpenGLError));

		return Result;
	}

	CError   SetFloat(std::string Name, float val) {

		CError  Result;
		int32_t index = GetIndex(Name);

		if (index == -1)
			return Result.format("Set %s error edit \"%s\" for value, because \"%s\" don`t create in shader", __FUNCTION__, Name.c_str(), Name.c_str());

		glUniform1fv(index, 1, &val);

		GLenum g_OpenGLError;
		if ((g_OpenGLError = glGetError()) != GL_NO_ERROR)
			Result.format("OpenGL expression %s error 0x%X for %s. Text Error: %s\n", __FUNCTION__, (unsigned)g_OpenGLError, Name.c_str(), glewGetErrorString(g_OpenGLError));

		return Result;
	}

	CError   SetVec(std::string Name, glm::vec3 & val) {

		CError  Result;
		int32_t index = GetIndex(Name);

		if (index == -1)
			return Result.format("Set %s error edit \"%s\" for value, because \"%s\" don`t create in shader", __FUNCTION__, Name.c_str(), Name.c_str());

		glUniform3fv(index, 1, glm::value_ptr(val));

		GLenum g_OpenGLError;
		if ((g_OpenGLError = glGetError()) != GL_NO_ERROR)
			Result.format("OpenGL expression %s error 0x%X for %s. Text Error: %s\n", __FUNCTION__, (unsigned)g_OpenGLError, Name.c_str(), glewGetErrorString(g_OpenGLError));

		return Result;
	}

	CError   SetVec(std::string Name, glm::vec4 & val) {
		
		CError  Result;
		int32_t index = GetIndex(Name);

		if (index == -1)
			return Result.format("Set %s error edit \"%s\" for value, because \"%s\" don`t create in shader", __FUNCTION__, Name.c_str(), Name.c_str());

		glUniform4fv(index, 1, glm::value_ptr(val));

		GLenum g_OpenGLError;
		if ((g_OpenGLError = glGetError()) != GL_NO_ERROR)
			Result.format("OpenGL expression %s error 0x%X for %s. Text Error: %s\n", __FUNCTION__, (unsigned)g_OpenGLError, Name.c_str(), glewGetErrorString(g_OpenGLError));

		return Result;
	}

    CError   SetMat(std::string Name, glm::mat4 & val) {

		CError  Result;
		int32_t index = this->GetUniform(Name);//GetIndex(Name);

		if (index == -1)	
			return Result.format("Set mat error edit \"%s\" for value, because \"%s\" don`t create in shader", Name.c_str(), Name.c_str());
		
		glUniformMatrix4fv(index, 1, GL_FALSE, glm::value_ptr(val));
	
		GLenum g_OpenGLError;
		if ((g_OpenGLError = glGetError()) != GL_NO_ERROR)			
			Result.format("OpenGL expression %s error 0x%X for %s. Text Error: %s\n", __FUNCTION__, (unsigned)g_OpenGLError, Name.c_str(), glewGetErrorString(g_OpenGLError));
		
		return Result;
	}
	
	struct name_index_t
	{
		name_index_t(std::string n, int32_t i)
		{
			name = n;
			index = i;
		}

		name_index_t() = default;

		std::string name;
		int32_t     index = -1;
	};

	std::vector<name_index_t> list_name_uniform;
	std::vector<name_index_t> list_name_attribute;

	enum class name_type_t
	{
		uni,
		att
	};

	inline int32_t GetIU(std::string name) { return GetIndex(name); }
	inline int32_t GetIA(std::string name) { return GetIndex(name, name_type_t::att); }

	int32_t GetIndex(std::string name, name_type_t t = name_type_t::uni)
	{
		if (t == name_type_t::uni) {
			
			for (auto &it : list_name_uniform)
				if (name == it.name)
					return it.index;
							
			int32_t index = GetUniform(name);

			if (index == -1)
					return -1;

			name_index_t new_obj(name, index);
			list_name_uniform.push_back(new_obj);
			return index;
		}

		if (t == name_type_t::att) {

			for (auto &it : list_name_attribute)
				if (name == it.name)
					return it.index;

			int32_t index = GetAttribute(name);

			if (index == -1)
				return -1;

			list_name_attribute.push_back(name_index_t(name, index));
			return index;
		}

		return -1;
	}

    CBaseShader BaseShader;

	inline operator uint32_t() { return BaseShader.index; }
};

// Vertex Fragment Shader
class CShaderVF
{
   public:
	   inline operator bool()     { return isLoad; }
	   inline operator uint32_t() { return index;  }

	   CError Open(CFile FileVertexShader, CFile FileFragmentShader)
	   {
		   CError Result;

		   VertexShader.Init(shader_type_t::vertex);
		   FragmentShader.Init(shader_type_t::fragment);

		   Result = VertexShader.BaseCodeCompile(FileVertexShader);

		   if (Result)
			   return Result;

		   Result = FragmentShader.BaseCodeCompile(FileFragmentShader);

		   if (Result)
			   return Result;

		   isLoad = true;
		   return AttachShader();
	   }

	   bool     isLoad = false;
	   uint32_t index  = 0;

private:

	CError AttachShader()
	{
		CError Result;

		index = glCreateProgram();

		glAttachShader(index, VertexShader);
		glAttachShader(index, FragmentShader);
		glLinkProgram(index);

		int32_t Check, info;
		glGetProgramiv(index, GL_LINK_STATUS, &Check);
		glGetProgramiv(index, GL_INFO_LOG_LENGTH, &info);

		if (info > 0) {
			std::vector<char> ProgramErrorMessage(info + 1);
			glGetProgramInfoLog(index, info, NULL, &ProgramErrorMessage[0]);
			isLoad = false;
			return Result = &ProgramErrorMessage[0];
		}

		glDetachShader(index, VertexShader);
		glDetachShader(index, FragmentShader);

		Delete();

		return Result;
	}

	   void Delete()
	   {
		   if (VertexShader)
		    VertexShader.Delete();

		   if (FragmentShader)
		    FragmentShader.Delete();

	   }


	   CFileShader VertexShader;
	   CFileShader FragmentShader;
};
