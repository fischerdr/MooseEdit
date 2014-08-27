#include "GlShader.h"

bool GlShader::isValid() const
{
	return m_isValid;
}

void GlShader::setIsValid(bool value)
{
	m_isValid = value;
}


int GlShader::getShaderId() const
{
	return shaderId;
}

void GlShader::setShaderId(int value)
{
	shaderId = value;
}

bool GlShader::compile() {
	shaderId = glCreateShader(shaderType);
	GLint length = shaderSource.length();
	const char *str = shaderSource.c_str();
	glShaderSource(shaderId, 1, (const GLchar**)&str, &length);
	glCompileShader(shaderId);
	
	GLint shader_ok;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &shader_ok);
	if (!shader_ok) {
		_setLastError("Compile failed: ");
		return false;
	}
	return true;
}

std::string GlShader::getLastError() const
{
	return lastError;
}

GLuint GlShader::getProgram() const
{
	return program;
}

void GlShader::setProgram(const GLuint &value)
{
	program = value;
}

void GlShader::_setLastError(std::string errorPrefix) {
	m_isValid = false;
	
	GLint log_length;
	char *log;
	
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &log_length);
	log = new char[log_length];
	glGetShaderInfoLog(shaderId, log_length, NULL, log);
	lastError = errorPrefix + log;
	delete[] log;
	
	glDeleteShader(shaderId);
}

GlShader::GlShader(GLenum shaderType, std::istream &stream) {
	this->shaderType = shaderType;
	stream.seekg(0, std::ios_base::end);
	int size = stream.tellg();
	stream.seekg(0, std::ios_base::beg);
	char *data = new char[size + 1];
	data[size] = 0;
	stream.read(data, size);
	shaderSource = data;
	delete[] data;
}
