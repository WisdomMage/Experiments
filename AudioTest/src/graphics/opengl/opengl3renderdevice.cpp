#include "opengl3renderdevice.h"
#include "opengl3vertexarray.h"
#include "opengl3shaderprogram.h"
#include "opengl3texture.h"
#include <GL/glew.h>
#include <sstream>

OpenGL3RenderDevice::OpenGL3RenderDevice()
{
	int majorVersion;
	int minorVersion;
		
	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion); 
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
		
	m_version = (unsigned int)(majorVersion * 100 + minorVersion * 10);

	if(m_version >= 330)
	{
		std::ostringstream convert;
		convert << m_version;
	
		m_shaderVersion = convert.str();
	}
	else if(m_version >= 320)
	{
		m_shaderVersion = "150";
	}
	else if(m_version >= 310)
	{
		m_shaderVersion = "140";
	}
	else if(m_version >= 300)
	{
		m_shaderVersion = "130";
	}
	else if(m_version >= 210)
	{
		m_shaderVersion = "120";
	}
	else if(m_version >= 200)
	{
		m_shaderVersion = "110";
	}
	else
	{
		std::ostringstream out;
		out << "Error: OpenGL Version " << majorVersion << "." << minorVersion << " does not support shaders.";
		throw IRenderDevice::Exception(out.str());
	}
}

IVertexArray* OpenGL3RenderDevice::CreateVertexArray(
			float** vertexData, unsigned int* vertexElementSizes,
			unsigned int numVertexComponents, unsigned int numVertices,
			unsigned int* indices, unsigned int numIndices)
{
	unsigned int numBuffers = numVertexComponents + 1;

	GLuint VAO;
	GLuint* buffers = new GLuint[numBuffers];

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(numBuffers, buffers);
	for(unsigned int i = 0; i < numVertexComponents; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
		glBufferData(GL_ARRAY_BUFFER, vertexElementSizes[i] * sizeof(float) * numVertices, vertexData[i], GL_STATIC_DRAW);

		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, vertexElementSizes[i], GL_FLOAT, GL_FALSE, 0, 0);
	}
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[numVertexComponents]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	return new OpenGL3VertexArray(VAO, buffers, numBuffers, numIndices);
}

void OpenGL3RenderDevice::ReleaseVertexArray(IVertexArray* vertexArray)
{
	if(vertexArray) { delete vertexArray; }
}

IShaderProgram* OpenGL3RenderDevice::CreateShaderProgram(const std::string& shaderText)
{
	return new OpenGL3ShaderProgram(shaderText, m_shaderVersion, false);
}

IShaderProgram* OpenGL3RenderDevice::CreateShaderProgramFromFile(
			const std::string& fileName)
{
	return new OpenGL3ShaderProgram(fileName, m_shaderVersion, true);
}

void OpenGL3RenderDevice::ReleaseShaderProgram(IShaderProgram* shaderProgram)
{
	if(shaderProgram) { delete shaderProgram; }
}

ITexture* OpenGL3RenderDevice::CreateTexture(int width, int height, 
		unsigned char* data, int filter, float anisotropy, int internalFormat,
		int format, bool clamp)
{
	return new OpenGL3Texture(width, height, data, filter, anisotropy, 
			internalFormat, format, clamp);
}

void OpenGL3RenderDevice::ReleaseTexture(ITexture* texture)
{
	if(texture) { delete texture; }
}
