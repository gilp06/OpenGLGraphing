//
// Created by GPowers on 10/13/2022.
//

#ifndef OPENGL_SHADER_HPP
#define OPENGL_SHADER_HPP

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"


class Shader {
public:
    unsigned int ID{};
public:
    Shader() = default;
    ~Shader();
    Shader &Use();
    void Compile(const char* vertexSource, const char* fragmentSource);
    void SetUniformMatrix4fv(const char* name, const glm::mat4& mat4) const;
    void SetUniform4f(const char* name, const glm::vec4& vec) const;
};


#endif //OPENGL_SHADER_HPP
