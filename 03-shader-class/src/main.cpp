// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <iostream>
#include "shader.h"

auto main() -> int {
  /* Initialize the library */
  if (!glfwInit()) {
    return -1;
  }

#ifdef __linux__
  glfwWindowHintString(GLFW_X11_CLASS_NAME, "opengl test");
  glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "opengl test");
#endif

  /* Create a windowed mode window and its OpenGL context */
  GLFWwindow *window =
      glfwCreateWindow(640, 480, "Hello OpenGL", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // clang-format off
  float vertices[] = {
    0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // top middle
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
  };
  unsigned int indices[] = {
    0, 1, 2, 
  };
  // clang-format on

  // setup VAO
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO); // stores the resolved data(knows the real meaning of
                          // each vertex data layout)
  // setup VBO
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBO stores all vertex data unresolved
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // resolve vertex data to current VAO
  glEnableVertexAttribArray(0); // enable layout = 0
  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
      static_cast<void *>(nullptr)); // set data for layout = 0
  glEnableVertexAttribArray(1);      // enable layout = 1
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));

  // setup EBO
  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // setup shaders
  xac::Shader shader("../03-shader-class/shader/vert.vs",
                     "../03-shader-class/shader/frag.fs");

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.Use();
    shader.SetFloat("bias", 0.3f);

    glBindBuffer(GL_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT,
                   nullptr); // draw current EBO
    // another way to draw
    // glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3); // draw current VAO

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);

  glfwTerminate();
  return 0;
}
