#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const int viewportWidth = 800;
const int viewportHeight = 600;

const char *vertexShaderCode = R"(#version 330
  layout (location=0) in vec3 aPos;
  void main(){
	 gl_Position = vec4(aPos.x,aPos.y,aPos.z, 1.0);
  }
)";

const char *fragmentShaderCode = R"(#version 330
  out vec4 FragColor;
  void main(){
	 FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
  }
)";

void framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(viewportWidth, viewportWidth,
                                        "Exercise 5.8.1", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to Create GLFW window" << std::endl;
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to init GLAD" << std::endl;
    return -1;
  }
  // params are the bottom left corner of viewport and width and height
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

  // coords for two triangles
  float vertices[] = {-0.1f, 0.0f, 0.0f, -0.3f, 0.3f, 0.0f, -0.5f, 0.0f, 0.0f,
                      0.1f,  0.0f, 0.0f, 0.3f,  0.3f, 0.0f, 0.5f,  0.0f, 0.0f};
  // };
  // an unsigned int to be used as VBO's id and for VAO's id
  unsigned int VBO;
  unsigned int VAO;
  // generating a VBO id and VAO
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  // need to bind buffer first
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // sending defined vertices to gpu
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // creating an unsigned int to be used as vertex shader id.
  unsigned int vertexShader;
  // create a vertex shader
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // specify the source code for vertex shader
  glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
  // compile shader
  glCompileShader(vertexShader);

  // checking for compilation errors
  // var for success or not status
  int success;
  // var for log storage
  char infoLog[512];
  // checking whether compilation succeded if it didt then success will be 0 i
  // think
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  // if its 0 then we get info string and print it
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cerr << "Shader: VERTEX: Compilation failed:\n"
              << infoLog << std::endl;
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cerr << "Shader: FRAGMENT: Compilation failed:\n"
              << infoLog << std::endl;
  }

  // id for shader program
  unsigned int shaderProgram;
  // glCreateProgram returns id of a program object which we store in
  // shaderProgram
  shaderProgram = glCreateProgram();

  // attaching shader and linking the shader program
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cerr << "Shader: Program: linking failed:\n" << infoLog << std::endl;
  }
  // since both shaders been linked we delete the resources
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // telling opengl to use this program all shaders, rendering done afthe this
  // call will use this program
  // glUseProgram(shaderProgram);
  //
  // now time for a render loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    processInput(window);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glfwSwapBuffers(window);
  }
  glfwTerminate();
  return 0;
}
