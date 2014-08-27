#ifndef GLSHADERPROGRAM_H
#define GLSHADERPROGRAM_H

#define GLEW_STATIC
#include <GL/glew.h>
#include "GlShader.h"
#include <vector>
#include <map>

class GlShaderProgram
{
	void setNVectorAttribute(std::string &attributeName, GLsizei stride, const GLvoid* data, int dimension);
	std::map<std::string, GLint> uniformLocations;
	std::map<std::string, GLint> attributeIds;
	bool m_isValid;
	std::string lastError;
	GLuint program;
	std::vector<GlShader> shaders;
	void _setLastError(std::string errorPrefix);
public:
	GlShaderProgram(std::vector<GlShader> &shaders);
	void setUniformInt(std::string uniformName, GLint uniform);
	bool link();
	bool use();
	void unset();
	void setUniformMatrix4x4(std::string uniformName, GLfloat buffer[16]);
	void setUniformMatrix3x3(std::string uniformName, GLfloat buffer[9]);
	void set4dVectorAttribute(std::string attributeName, GLsizei stride, const GLvoid* data);
	void set3dVectorAttribute(std::string attributeName, GLsizei stride, const GLvoid* data);
	void set2dVectorAttribute(std::string attributeName, GLsizei stride, const GLvoid* data);
	std::string getLastError() const;
	GLuint getProgram() const;
};

#endif // GLSHADERPROGRAM_H
