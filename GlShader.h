#ifndef GLSHADER_H
#define GLSHADER_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include <string>
#include <istream>

class GlShader
{
	GLuint shaderId;
	GLuint program;
	GLenum shaderType;
	bool m_isValid;
	std::string shaderSource;
	std::string lastError;
	void _setLastError(std::string errorPrefix);
public:
	GlShader(GLenum shaderType, std::istream& stream);
	bool isValid() const;
	void setIsValid(bool value);
	int getShaderId() const;
	void setShaderId(int value);
	bool compile();
	std::string getLastError() const;
	GLuint getProgram() const;
	void setProgram(const GLuint &value);
};

#endif // GLSHADER_H
