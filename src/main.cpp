#include "glm/common.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <cstdlib>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ostream>
#include "glm/gtc/type_ptr.hpp"
#include "camera.h"

glm::vec3 movementVector;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

double mousex;
double mousey;

double prevMouseX = 0;
double prevMouseY = 0;

void mouseCallback(GLFWwindow *window, double _mouseX, double _mouseY) {
  prevMouseX = mousex;
  prevMouseY = mousey;
  mousex = _mouseX;
  mousey = _mouseY;
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "LearnOpenglBySuyog", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouseCallback);

  Shader shaderProgram("../shaders/shader.vert", "../shaders/shader.frag");
  Shader wShaderProgram("../shaders/wshader.vert", "../shaders/wshader.frag");
  stbi_set_flip_vertically_on_load(true);
  std::vector<std::string> imagePaths = {"../images/container.jpg",
                                         "../images/awesomeface.png"};
  auto noOfImages = imagePaths.size();
  unsigned int textures[noOfImages];
  glGenTextures(noOfImages, textures);
  int texWidth, texHeight, texNrChannels;
  for (int i = 0; i < noOfImages; i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, textures[i]);
    float borderColor[] = {0.5f, 1.0f, 0.5f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unsigned char *data = stbi_load(imagePaths[i].c_str(), &texWidth,
                                    &texHeight, &texNrChannels, 0);

    if (data) {
      GLenum format;
      if (texNrChannels == 1)
        format = GL_RED;
      else if (texNrChannels == 3)
        format = GL_RGB;
      else if (texNrChannels == 4)
        format = GL_RGBA;
      glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else {
      std::cerr << "Failed loading texture" << std::endl;
    }
    stbi_image_free(data);
  }

  float vertices[] = {
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // smtg
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // smtg
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // smtg
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // smtg
  };

  float wVertices[] = {
      0.0f,  0.4f,  0.0f, // E
      0.0f,  0.03f, 0.0f, // F
      0.3f,  0.2f,  0.0f, // G
      0.5f,  0.0f,  0.0f, // H
      0.7f,  0.6f,  0.0f, // I
      0.6f,  0.6f,  0.0f, // J
      -0.3f, 0.2f,  0.0f, // A
      -0.5f, 0.0f,  0.0f, // D
      -0.7f, 0.6f,  0.0f, // C
      -0.6f, 0.6f,  0.0f, // B
  };

  unsigned int wIndices[] = {6, 9, 8, 6, 7, 8, 6, 7, 1, 6, 0, 1,
                             1, 0, 2, 1, 2, 3, 2, 3, 4, 2, 5, 4};

  unsigned int indices[] = {0, 1, 3, 1, 2, 3};

  float cubeVertices[] = {
      // positions          // UVs
      // back face
      -0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      0.0f, //
      0.5f,
      -0.5f,
      -0.5f,
      1.0f,
      0.0f,
      0.5f,
      0.5f,
      -0.5f,
      1.0f,
      1.0f,
      0.5f,
      0.5f,
      -0.5f,
      1.0f,
      1.0f,
      -0.5f,
      0.5f,
      -0.5f,
      0.0f,
      1.0f,
      -0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      0.0f,

      // front face
      -0.5f,
      -0.5f,
      0.5f,
      0.0f,
      0.0f,
      0.5f,
      -0.5f,
      0.5f,
      1.0f,
      0.0f,
      0.5f,
      0.5f,
      0.5f,
      1.0f,
      1.0f,
      0.5f,
      0.5f,
      0.5f,
      1.0f,
      1.0f,
      -0.5f,
      0.5f,
      0.5f,
      0.0f,
      1.0f,
      -0.5f,
      -0.5f,
      0.5f,
      0.0f,
      0.0f,

      // left face
      -0.5f,
      0.5f,
      0.5f,
      1.0f,
      0.0f,
      -0.5f,
      0.5f,
      -0.5f,
      1.0f,
      1.0f,
      -0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      1.0f,
      -0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      1.0f,
      -0.5f,
      -0.5f,
      0.5f,
      0.0f,
      0.0f,
      -0.5f,
      0.5f,
      0.5f,
      1.0f,
      0.0f,

      // right face
      0.5f,
      0.5f,
      0.5f,
      1.0f,
      0.0f,
      0.5f,
      0.5f,
      -0.5f,
      1.0f,
      1.0f,
      0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      1.0f,
      0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      1.0f,
      0.5f,
      -0.5f,
      0.5f,
      0.0f,
      0.0f,
      0.5f,
      0.5f,
      0.5f,
      1.0f,
      0.0f,

      // bottom face
      -0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      1.0f,
      0.5f,
      -0.5f,
      -0.5f,
      1.0f,
      1.0f,
      0.5f,
      -0.5f,
      0.5f,
      1.0f,
      0.0f,
      0.5f,
      -0.5f,
      0.5f,
      1.0f,
      0.0f,
      -0.5f,
      -0.5f,
      0.5f,
      0.0f,
      0.0f,
      -0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      1.0f,

      // top face
      -0.5f,
      0.5f,
      -0.5f,
      0.0f,
      1.0f,
      0.5f,
      0.5f,
      -0.5f,
      1.0f,
      1.0f,
      0.5f,
      0.5f,
      0.5f,
      1.0f,
      0.0f,
      0.5f,
      0.5f,
      0.5f,
      1.0f,
      0.0f,
      -0.5f,
      0.5f,
      0.5f,
      0.0f,
      0.0f,
      -0.5f,
      0.5f,
      -0.5f,
      0.0f,
      1.0f,
  };
  unsigned int VAO;
  unsigned int wVAO;
  unsigned int VBO;
  unsigned int wVBO;
  unsigned int EBO;
  unsigned int wEBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
  //                       (void *)(3 * sizeof(float)));
  // glEnableVertexAttribArray(1);
  // glVertexAttribPointer(1,)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  glGenVertexArrays(1, &wVAO);
  glGenBuffers(1, &wVBO);
  glGenBuffers(1, &wEBO);

  glBindVertexArray(wVAO);
  glBindBuffer(GL_ARRAY_BUFFER, wVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(wVertices), wVertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wIndices), wIndices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  float alpha = 0.0f;
  // transformations
  float angle = 45.0f;
  // coordinate system
  auto model = glm::mat4(1.0f);
  model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  auto view = glm::mat4(1.0f);
  // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

  auto projection = glm::mat4(1.0f);
  projection =
      glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
  auto rate = 0.01;

  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  // camera
  Camera cam;
  while (!glfwWindowShouldClose(window)) {
    process_input(window);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float timeValue = glfwGetTime();
    alpha = 0.2;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      // rate += 0.01;
      // alpha += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      // rate -= 0.01;
      // alpha -= 0.01;
    }
    // float greenValue = (std::sin(timeValue) / 2.0f) + 0.5f;
    // int vertexColorLocation = glGetUniformLocation(shaderProgram,
    // "OurColor
    shaderProgram.use();
    shaderProgram.setFloat("movingVertex", sin(timeValue));
    shaderProgram.setInt("texture1", 0);
    shaderProgram.setInt("texture2", 1);
    shaderProgram.setFloat("a", alpha);
    shaderProgram.setMat4f("view", view);
    shaderProgram.setMat4f("projection", projection);
    glBindVertexArray(VAO);
    cam.move(window);
    cam.rotate(&prevMouseX, &prevMouseY, mousex, mousey);
    view = cam.lookAtMatrix();

    for (auto pos : cubePositions) {
      auto model = glm::mat4(1.0);
      model = glm::translate(model, pos);
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(0.2f, 0.4f, 0.4f));
      // model =
      //     glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f,
      //     0.0f));
      shaderProgram.setMat4f("model", model);
      angle = 20.0f * pos.x + pos.y;
      // for rotating wall
      //
      // auto trans = glm::mat4(1.0f);
      // trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 1.0f));
      // trans = glm::translate(trans, glm::vec3(0.8f, -0.8f, 1.0f));
      // trans =
      //     glm::rotate(trans, glm::radians(angle), glm::vec3(0.0f,
      //     0.0f, 1.0f));
      // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    // angle += rate;
    // glBindTexture(GL_TEXTURE_2D, texture1);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    wShaderProgram.use();
    auto wtrans = glm::mat4(1.0);
    wtrans =
        glm::scale(wtrans, glm::vec3(cos(glfwGetTime()), cos(glfwGetTime()),
                                     cos(glfwGetTime())));
    // int wshaderScaleLoc = glGetUniformLocation(wShaderProgram.ID, "scale");
    // glUniformMatrix4fv(wshaderScaleLoc, 1, GL_FALSE, glm::value_ptr(wtrans));
    // glBindVertexArray(wVAO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wEBO);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glDrawElements(GL_TRIANGLES, sizeof(wIndices) / sizeof(unsigned int),
    //                GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
