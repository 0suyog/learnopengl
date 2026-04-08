#pragma once

#include "glad/glad.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
  unsigned int ID;
  Shader(const char *vertexPath, const char *fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);

      std::stringstream vShaderStream, fShaderStream;

      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();

      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();

      vShaderFile.close();
      fShaderFile.close();

    } catch (std::ifstream::failure e) {
      std::cerr << "ERROR: SHADER:: FAILED LOADING FILE" << e.what()
                << std::endl;
    }
    auto vShaderCode = vertexCode.c_str();
    auto fShaderCode = fragmentCode.c_str();
    unsigned int vertexShader, fragmentShader;
    int success;
    char infoLog[512];

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);

    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      std::cerr << "ERROR: SHADER: VERTEX: shader compilation failed" << infoLog
                << std::endl;
    }

    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      std::cerr << "ERROR: SHADER: FRAGMENT: shader compilation failed"
                << infoLog << std::endl;
    }
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success) {
      glGetProgramInfoLog(ID, 512, NULL, infoLog);
      std::cerr << "ERROR: SHADER: LINK: program linking falide" << infoLog
                << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
  }
  void use() const { glUseProgram(ID); }
  void setBool(const std::string &name, bool value) {
    auto uniformLocation = glGetUniformLocation(ID, name.c_str());
    glUniform1i(uniformLocation, value);
  }
  void setInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
  };
  void setFloat(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  };
  void setMat4f(const std::string &name, const glm::mat4 &value) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(value));
  }
  void setVec3(const std::string &name, const glm::vec3 &value) {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1,
                 glm::value_ptr(value));
  }
};
