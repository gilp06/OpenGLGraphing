//
// Created by GPowers on 10/13/2022.
//

#include "Shader.hpp"

void Shader::Compile(const char *vertexSource, const char *fragmentSource) {
    glDeleteProgram(this->ID);

    unsigned int sVertex, sFragment;
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, nullptr);
    glCompileShader(sVertex);

    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, nullptr);
    glCompileShader(sFragment);
    this->ID = glCreateProgram();

    glAttachShader(this->ID, sVertex);
    glAttachShader(this->ID, sFragment);
    glLinkProgram(this->ID);

    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
}

Shader::~Shader()
{
glDeleteProgram(this->ID);
}

Shader &Shader::Use() {
    glUseProgram(this->ID);
    return *this;
}

void Shader::SetUniformMatrix4fv(const char *name, const glm::mat4 &mat4) const {
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, glm::value_ptr(mat4));
}

void Shader::SetUniform4f(const char *name, const glm::vec4 &vec) const {
    glUniform4fv(glGetUniformLocation(this->ID, name), 1, glm::value_ptr(vec));
}
