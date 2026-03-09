#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    // "layout (location = 1) in vec3 aColor;\n"
    // "out ourColor;"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    // "	  ourColor = aColor;"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   " uniform vec4 OurColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "	  FragColor=OurColor;"
                                   "}\n\0";

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

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  unsigned int vertexshader;
  vertexshader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexshader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexshader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertexshader, 512, NULL, infoLog);
    std::cerr << "ERROR: SHADER: VERTEX: Compilation Failed\n"
              << infoLog << std::endl;
  }

  unsigned int fragmentshader;
  fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentshader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentshader);
  glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(fragmentshader, 512, NULL, infoLog);
    std::cerr << "ERROR: SHADER: FRAGMENT: Compilation Failed\n"
              << infoLog << std::endl;
  }

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexshader);
  glAttachShader(shaderProgram, fragmentshader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cerr << "ERROR: SHADER: PROGRAM: Linking Failed\n"
              << infoLog << std::endl;
  }
  glDeleteShader(fragmentshader);
  glDeleteShader(vertexshader);

  // float vertices[] = {-0.5, -0.5, 0.0, 0.5, -0.5, 0.0, 0.0, 0.5, 0.0};
  float vertices[] = {0.5f,  0.5f,  0.0f, 0.5f,  -0.5f, 0.0f,
                      -0.5f, -0.5f, 0.0f, -0.5f, 0.5f,  0.0f};

  unsigned int indices[] = {0, 1, 3, 1, 2, 3};

  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  while (!glfwWindowShouldClose(window)) {
    process_input(window);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    float timeValue = glfwGetTime();
    float greenValue = (std::sin(timeValue) / 2.0f) + 0.5f;
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "OurColor");
    glUseProgram(shaderProgram);
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
    glBindVertexArray(VAO);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
