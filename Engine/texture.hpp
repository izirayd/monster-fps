#pragma once

#include "base.hpp"
#include "error.hpp"
#include "file.hpp"
#include "graphic_base.hpp"

struct header_bmp_t
{
	int32_t  dataPos;
	int32_t  imageSize;
	int32_t  width, height;
};

enum class type_texture_t
{
	unknow,
	diffuse,
	special
};

class CTexture
{
   public:
	   CTexture() = default;
	   CTexture(CTexture *Texture) { *this = Texture; }

	   inline operator uint32_t() { return textureID; }
	   inline operator bool()     { return isLoad; }

	   type_texture_t type = type_texture_t::unknow;

	   void Bind(int32_t ActiveIndex)
	   {
		   if (isLoad) {
			   glActiveTexture(GL_TEXTURE0 + ActiveIndex);
			   glBindTexture(GL_TEXTURE_2D, textureID);
		   }
	   }

	   void Bind()
	   {
		   if (isLoad) {
			   glActiveTexture(GL_TEXTURE0);
			   glBindTexture(GL_TEXTURE_2D, textureID);
		   }
	   }

	   void Unbind()
	   {
		   if (isLoad)
		   glBindTexture(GL_TEXTURE_2D, 0);
	   }

	   CError Create(unsigned char* Buffer, int32_t width, int32_t height)
	   {
		   CError Result;
		   glGenTextures(1, &textureID);
	
		   glBindTexture(GL_TEXTURE_2D, textureID);
		   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, Buffer);

		   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		   glGenerateMipmap(GL_TEXTURE_2D);

		   isLoad = true;
		   Unbind();

		   type = type_texture_t::diffuse;

		   return Result;
	   }

	   // Удалит текстуру из памяти GPU
	   void delete_allocate_gpu()
	   {
		   glDeleteTextures(1, &textureID);
		   isLoad = false;
	   }

	   uint32_t textureID;
	   bool isLoad = false;
};

class CGroupTexture
{
    public:
		std::vector<CTexture> ListTexture;

		void Bind()
		{
			int32_t i = 0;
			for (auto &it : ListTexture)	{
				it.Bind(i);
				i++;
			}
		}

		void Unbind()
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void AddTexture(CTexture Texture)
		{
			ListTexture.push_back(Texture);
		}

		CGroupTexture operator= (CTexture *Texture)
		{
			AddTexture(Texture);
			return *this;
		}

		CGroupTexture operator= (CTexture &Texture)
		{
			AddTexture(Texture);
			return *this;
		}
};


#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

class COpenTexture
{
  public:

	  COpenTexture() = default;
	  COpenTexture(wchar_t *Path)    { OpenTexture(Path); }
	  COpenTexture(wchar_t *Path, type_texture_t t) { OpenTexture(Path); this->ResultTexture.type = t; }
	  
	  COpenTexture(COpenTexture *ot) { *this = ot; }

	  CTexture OpenTexture(wchar_t *FilePath) {
		  OpenTexture(FilePath, &ResultTexture);
		  return ResultTexture;
	  }
	  
	  CError OpenTexture(wchar_t *FilePath, CTexture *Texture)
	  {
		  CError Result;

		  if (FilePath == NULL)
			  return Result.format("Cannot open file image");

		  if (Texture == nullptr || Texture == NULL)
			  return Result.format("No allocate or point texture %ls", FilePath);
		  
		  ilutRenderer(ILUT_OPENGL);

		  uint32_t Status = ilutGLLoadImage(FilePath);

		  if (Status == 0)
			  return Result.format("No load texture: %ls", FilePath);
			  
			  Texture->textureID = Status;
			  Texture->isLoad    = true;

			  Texture->Unbind();

		  return Result;
	  }

	  void OpenImageBmp(wchar_t *FilePath, CTexture *Texture)
	  {
		  CFile FileImage;
		  FileImage.OpenFile(FilePath);
		  OpenImageBmp(FileImage, Texture);
		  FileImage.CloseFile();
	  }

	  CError OpenImageBmp(CFile FileImage, CTexture *Texture)
	  {
		  CError Result;

		  if (!FileImage)
			  return Result.format("Cannot open bmp image: %ls", FileImage.wfilename.c_str());

		  if (FileImage.Size == 0)
			  return Result.format("Size bmp image = 0: %ls", FileImage.wfilename.c_str());

		  if (Texture == nullptr || Texture == NULL)
			  return Result.format("No allocate or point texture %ls", FileImage.wfilename.c_str());

		  header_bmp_t  header;
		  unsigned char HeaderBuffer[54];

		  fseek(FileImage.GetDesc(), 0, SEEK_SET);
		  if (fread(HeaderBuffer, 1, 54, FileImage.GetDesc()) != 54)
			  return Result.format("Don`t bmp image = 0: %ls", FileImage.wfilename.c_str());

		  if (HeaderBuffer[0] != 'B' || HeaderBuffer[1] != 'M' || *(int*)&(HeaderBuffer[0x1E]) != 0 || *(int*)&(HeaderBuffer[0x1C]) != 24)
			  return Result.format("Not a correct BMP file: %ls", FileImage.wfilename.c_str());

		  header.dataPos    = *(int*)&(HeaderBuffer[0x0A]);
		  header.imageSize  = *(int*)&(HeaderBuffer[0x22]);
		  header.width      = *(int*)&(HeaderBuffer[0x12]);
		  header.height     = *(int*)&(HeaderBuffer[0x16]);

		  if (header.imageSize == 0)      header.imageSize = header.width * header.height * 3;
		  if (header.dataPos   == 0)      header.dataPos   = 54;

		  unsigned char *data = new unsigned char[header.imageSize];

		  fread(data, 1, header.imageSize, FileImage.GetDesc());
		  Texture->Create(data, header.width, header.height);

		  delete[]  data;
		  return Result;
	  }

	  CTexture ResultTexture;
};