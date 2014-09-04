#include "GlShaderProgram.h"

void GlShaderProgram::_setLastError(std::string errorPrefix) {
	m_isValid = false;
	
	GLint log_length;
	char *log;
	
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
	log = new char[log_length];
	glGetProgramInfoLog(program, log_length, NULL, log);
	lastError = errorPrefix + log;
	delete[] log;
	
	for (int i=0; i<shaders.size(); ++i) {
		GlShader &shader = shaders[i];
		glDeleteShader(shader.getShaderId());
	}
	
	glDeleteProgram(program);
}

bool GlShaderProgram::link() {
	program = glCreateProgram();
	for (int i=0; i<shaders.size(); ++i) {
		GlShader& shader = shaders[i];
		glAttachShader(program, shader.getShaderId());
		shader.setProgram(program);
	}
	glLinkProgram(program);
	
	GLint program_ok;
	glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
	if (!program_ok) {
		_setLastError("Link failed: ");
		return false;
	}
	return true;
}

bool GlShaderProgram::use()
{
	glUseProgram(program);
}

void GlShaderProgram::unset() {
	glUseProgram(0);
	
	for (std::map<std::string, GLint>::iterator it = attributeIds.begin(); it != attributeIds.end(); ++it) {
		glDisableVertexAttribArray(it->second);
	}
}

GlShaderProgram::GlShaderProgram(std::vector<GlShader> &shaders) {
	this->shaders = shaders;
}

void GlShaderProgram::setNVectorAttribute(std::string &attributeName, GLsizei stride, const GLvoid* data, int dimension, int type) {
	if (attributeIds.find(attributeName) == attributeIds.end()) {
		GLint attribId = glGetAttribLocation( program, attributeName.c_str() );
		if (attribId < 0) {
			return;
		}
		attributeIds[attributeName] = attribId;
	}
	GLint attribId = attributeIds[attributeName];
	glEnableVertexAttribArray(attribId);
    glVertexAttribPointer( attribId, dimension, type, GL_FALSE, stride, data );
}

GLuint GlShaderProgram::getProgram() const
{
	return program;
}

std::string GlShaderProgram::getLastError() const
{
	return lastError;
}

void GlShaderProgram::set4dVectorAttribute(std::string attributeName, GLsizei stride, const GLvoid* data) {
	setNVectorAttribute(attributeName, stride, data, 4, GL_FLOAT);
}

void GlShaderProgram::set4dVectorAttributeUB(std::string attributeName, GLsizei stride, const GLvoid* data) {
	setNVectorAttribute(attributeName, stride, data, 4, GL_UNSIGNED_BYTE);
}

void GlShaderProgram::set3dVectorAttribute(std::string attributeName, GLsizei stride, const GLvoid* data) {
	setNVectorAttribute(attributeName, stride, data, 3, GL_FLOAT);
}

void GlShaderProgram::set2dVectorAttribute(std::string attributeName, GLsizei stride, const GLvoid* data) {
	setNVectorAttribute(attributeName, stride, data, 2, GL_FLOAT);
}

void GlShaderProgram::setUniformMatrixArray4x3(std::string uniformName, GLfloat buffer[], int arraySize) {
	if (uniformLocations.find(uniformName) == uniformLocations.end()) {
		GLint location = glGetUniformLocation(program, uniformName.c_str());
		if (location < 0) {
			return;
		}
		uniformLocations[uniformName] = location;
	}
	GLint location = uniformLocations[uniformName];
	glUniformMatrix4x3fv(	location,
		arraySize, false,
		buffer);
}

void GlShaderProgram::setUniformMatrix4x4(std::string uniformName, GLfloat buffer[16]) {
	if (uniformLocations.find(uniformName) == uniformLocations.end()) {
		GLint location = glGetUniformLocation(program, uniformName.c_str());
		if (location < 0) {
			return;
		}
		uniformLocations[uniformName] = location;
	}
	GLint location = uniformLocations[uniformName];
	glUniformMatrix4fv(	location,
		1, false,
		buffer);
}

void GlShaderProgram::setUniformMatrix4x3(std::string uniformName, GLfloat buffer[12]) {
	if (uniformLocations.find(uniformName) == uniformLocations.end()) {
		GLint location = glGetUniformLocation(program, uniformName.c_str());
		if (location < 0) {
			return;
		}
		uniformLocations[uniformName] = location;
	}
	GLint location = uniformLocations[uniformName];
	glUniformMatrix4x3fv(	location,
		1, false,
		buffer);
}


void GlShaderProgram::setUniformMatrix3x3(std::string uniformName, GLfloat buffer[9]) {
	if (uniformLocations.find(uniformName) == uniformLocations.end()) {
		GLint location = glGetUniformLocation(program, uniformName.c_str());
		if (location < 0) {
			return;
		}
		uniformLocations[uniformName] = location;
	}
	GLint location = uniformLocations[uniformName];
	glUniformMatrix3fv(	location,
		1, false,
		buffer);
}

void GlShaderProgram::setUniformInt(std::string uniformName, GLint uniform) {
	if (uniformLocations.find(uniformName) == uniformLocations.end()) {
		GLint location = glGetUniformLocation(program, uniformName.c_str());
		if (location < 0) {
			return;
		}
		uniformLocations[uniformName] = location;
	}
	GLint location = uniformLocations[uniformName];
	glUniform1i(location, uniform);
}

void GlShaderProgram::setUniformFloat(std::string uniformName, GLfloat uniform) {
	if (uniformLocations.find(uniformName) == uniformLocations.end()) {
		GLint location = glGetUniformLocation(program, uniformName.c_str());
		if (location < 0) {
			return;
		}
		uniformLocations[uniformName] = location;
	}
	GLint location = uniformLocations[uniformName];
	glUniform1f(location, uniform);
}

void GlShaderProgram::setUniformVec4(std::string uniformName, GLfloat components[4]) {
	if (uniformLocations.find(uniformName) == uniformLocations.end()) {
		GLint location = glGetUniformLocation(program, uniformName.c_str());
		if (location < 0) {
			return;
		}
		uniformLocations[uniformName] = location;
	}
	GLint location = uniformLocations[uniformName];
	glUniform4fv(location, 1, components);
}
