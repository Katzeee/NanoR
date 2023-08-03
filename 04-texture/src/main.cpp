// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "../../common/stb_image.h"
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
      glfwCreateWindow(640, 640, "Hello OpenGL", nullptr, nullptr);
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
  //   ---- pos ----      ---- color ----  - texcoord -
     0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f    // bottom right
  };
  unsigned int indices[] = {
    0, 1, 2, 
    0, 2, 3
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
      0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
      static_cast<void *>(nullptr)); // set data for layout = 0
  glEnableVertexAttribArray(1);      // enable layout = 1
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(6 * sizeof(float)));

  // setup EBO
  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // setup shaders
  xac::Shader shader("../04-texture/shader/vert.vs",
                     "../04-texture/shader/frag.fs");

  // setup texture
  int width, height, nr_channels;
  unsigned char *tex_data = stbi_load("../resources/textures/container.jpg",
                                      &width, &height, &nr_channels, 0);
  unsigned int box_tex;
  glGenTextures(1, &box_tex);
  glActiveTexture(GL_TEXTURE0); // active texture0 before bind
  glBindTexture(GL_TEXTURE_2D, box_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, tex_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(tex_data);

  stbi_set_flip_vertically_on_load(true);
  tex_data = stbi_load("../resources/textures/awesomeface.png", &width,
                       &height, &nr_channels, 0);
  unsigned int face_tex;
  glGenTextures(1, &face_tex);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, face_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, tex_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(tex_data);

  shader.Use();
  shader.SetInt("Texture1", 0);
  shader.SetInt("Texture2", 1);

  glEnable(GL_BLEND); // enable blending function
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // shader.Use();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, EBO);
    // draw current EBO
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);

  glfwTerminate();
  return 0;
}
