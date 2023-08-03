// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../common/stb_image.h"
#include "camera.h"
#include "shader.h"

glm::vec3 positon(0.0f, 0.0f, 5.0f);
glm::vec3 target(0.0f, 0.0f, -2.0f);
xac::Camera main_cam(positon, target);
float delta_time_per_frame;

void FrameBufferSizeCB(GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); }

void ScrollCB(GLFWwindow *window, double x_offset, double y_offset) { main_cam.UpdateScroll(y_offset); }

void CursorCB(GLFWwindow *window, double x_pos, double y_pos) {
  static float last_x_pos = static_cast<float>(x_pos);
  static float last_y_pos = static_cast<float>(y_pos);
  if (x_pos == last_x_pos && y_pos == last_y_pos) {
    return;
  }
  float x_offset = x_pos - last_x_pos;
  float y_offset = y_pos - last_y_pos;
  last_x_pos = static_cast<float>(x_pos);
  last_y_pos = static_cast<float>(y_pos);

  main_cam.UpdateCursorMove(x_offset, y_offset);
}

void ProcessInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  float speed = delta_time_per_frame * 10.0f;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    main_cam.PositionForward(main_cam.GetFornt() * speed);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    main_cam.PositionForward(main_cam.GetFornt() * -speed);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    main_cam.PositionForward(glm::cross(main_cam.GetFornt(), main_cam.GetUp()) * -speed);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    main_cam.PositionForward(glm::cross(main_cam.GetFornt(), main_cam.GetUp()) * speed);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    main_cam.PositionForward(main_cam.GetUp() * speed);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    main_cam.PositionForward(main_cam.GetUp() * -speed);
  }
}

auto main() -> int {
  if (!glfwInit()) {
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __linux__
  glfwWindowHintString(GLFW_X11_CLASS_NAME, "opengl test");
  glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "opengl test");
#endif

  GLFWwindow *window = glfwCreateWindow(640, 640, "Hello OpenGL", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, FrameBufferSizeCB);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GALD" << std::endl;
    return -1;
  }

  // clang-format off
  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
  };
  // clang-format on

  // setup objs
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));

  // setup light
  unsigned int light_VAO;
  glGenVertexArrays(1, &light_VAO);
  glBindVertexArray(light_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);

  // setup shader
  xac::Shader light_shader("../07-lighting/shader/light-vert.vs", "../07-lighting/shader/light-frag.fs");
  xac::Shader obj_shader("../07-lighting/shader/vert.vs", "../07-lighting/shader/frag.fs");

  // setup matrices
  glm::mat4 model(1.0f);
  glm::mat4 light_model(1.0f);
  glm::vec3 light_pos(1.2f, 1.0f, 2.2f);
  light_model = glm::translate(light_model, light_pos);
  light_model = glm::scale(light_model, glm::vec3(0.2f));

  // other settings
  glEnable(GL_DEPTH_TEST);
  glfwSetScrollCallback(window, ScrollCB);
  glfwSetCursorPosCallback(window, CursorCB);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  while (!glfwWindowShouldClose(window)) {
    ProcessInput(window);
    static float last_frame_time = 0.0f;
    float cur_frame_time = static_cast<float>(glfwGetTime());
    delta_time_per_frame = cur_frame_time - last_frame_time;
    last_frame_time = cur_frame_time;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(light_VAO);
    light_shader.Use();
    light_shader.SetMat4("model", light_model);
    obj_shader.SetMat4("view", main_cam.GetViewMatrix());
    obj_shader.SetMat4("projection", main_cam.GetProjectionMatrix());
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(VAO);
    obj_shader.Use();
    model = glm::rotate(model, std::sin(cur_frame_time) * 0.001f * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    model = glm::rotate(model, std::sin(cur_frame_time) * 0.001f * glm::radians(50.0f), glm::vec3(0.5f, 0.0f, 0.5f));
    obj_shader.SetMat4("model", model);
    obj_shader.SetMat4("view", main_cam.GetViewMatrix());
    obj_shader.SetMat4("projection", main_cam.GetProjectionMatrix());
    obj_shader.SetVec3("light_color", glm::vec3(1.0f));
    obj_shader.SetVec3("obj_color", glm::vec3(1.0f, 0.5f, 0.31f));
    obj_shader.SetVec3("light_pos", glm::vec3(light_model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
    obj_shader.SetVec3("view_pos", main_cam.GetPosition());

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  glfwTerminate();
  return 0;
}
