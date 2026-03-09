#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const int viewportWidth = 800;
const int viewportHeight = 600;

void windoResizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

const char *vertexShaderCode = R"(
  #version 330 core
  layout (location 0)
  in vec4 aPos;
  void main(){
  gl_Pos = aPos;
  }
)";

// const char *fragmentShoderCode = R"(
//   #version 330 core
//
// )"

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(viewportWidth, viewportHeight,
                                        "Exercise 5.8.2", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to Create GLFW window" << std::endl;
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to init GLAD" << std::endl;
    return -1;
  }
  // set viewport width and height params are the bottom left corner of viewport
  // and width and height
  glViewport(0, 0, 800, 600);
  // resize callback
  glfwSetFramebufferSizeCallback(window, windoResizeCallback);
  // coords for two triangles
  float vertices1[] = {-0.1f, 0.0f, 0.0f, -0.3f, 0.3f, 0.0f, -0.5f, 0.0f, 0.0f};
  float vertices2[] = {0.1f, 0.0f, 0.0f, 0.3f, 0.3f, 0.0f, 0.5f, 0.0f, 0.0f};
  // an unsigned int to be used as VBO's id and for VAO's id
  GLuint VBO[2];
  GLuint VAO[2];
  // generating a VBO id and VAO
  glGenBuffers(2, VBO);
  glGenVertexArrays(2, VAO);
  // need to bind vertex array
  glBindVertexArray(VAO[0]);
  // need to bind buffer first
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  // sending defined vertices to gpu
  glBufferData(GL_VERTEX_ARRAY, 0, vertices1, GL_STATIC_DRAW);
  // telling opengl how it should interpret data in vertex buffer
  glVertexAttribPointer(0, sizeof(vertices1), GL_FLOAT, GL_FALSE,
                        3 * sizeof(float), (void *)0);
  // enabling vertex array
  glEnableVertexAttribArray(0);
  // for second triangle
  glBindVertexArray(VAO[1]);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
  glBufferData(GL_VERTEX_ARRAY, 0, vertices2, GL_STATIC_DRAW);
  glVertexAttribPointer(0, sizeof(vertices2), GL_FLOAT, GL_FALSE,
                        3 * sizeof(float), (void *)0);
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
  // checking whether compilation succeded if it didt then success will be 0
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  // i think if its 0 then we get info string and print it same ofr fragment
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cerr << "SHADER: VERTEX: compilation failed" << std::endl;
  }

  unsigned int fragmantShader;
  fragmantShader = glCreateShader(GL_FRAGMENT_SHADER);
  // glShaderSource(fragmantShader,1,)
  // shader

  // id for shader program
  // glCreateProgram returns id of a program object which we store in
  // shaderProgram

  // attaching shader and linking the shader program

  // since both shaders been linked we delete the resources

  // telling opengl to use this program all shaders, rendering done afthe
  // this call will use this program glUseProgram(shaderProgram);
  //
  // now time for a render loop
  while (!glfwWindowShouldClose(window)) {
    // polling events
    glfwPollEvents();
    // processing input

    // clearing screen
    glClearColor(0.5, 0.5, 0.5, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    // specifying program to use
    // binding vertex array
    // dravitg trinagles
    // double buffer so swaap
  }
  // after all is finish terminate glfw
  return 0;
}
