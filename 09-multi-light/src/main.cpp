// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "camera.h"
#include "texture.h"
#include "shader.h"
// #include "../../common/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 positon(0.0f, 0.0f, 5.0f);
glm::vec3 front(0.0f, 0.0f, -2.0f);
xac::Camera main_cam(positon, front, glm::vec3(0.0f, 1.0f, 0.0f));
float delta_time_per_frame;

void FrameBufferSizeCB(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void ScrollCB(GLFWwindow *window, double x_offset, double y_offset) {
  main_cam.UpdateScroll(y_offset);
}

void CursorCB(GLFWwindow *window, double x_pos, double y_pos) {
  static bool first = true;
  static float last_x_pos;
  static float last_y_pos;
  // std::cout << first << " " << x_pos << " " << y_pos << std::endl;
  if (first) {
    first = false;
    last_x_pos = static_cast<float>(x_pos);
    last_y_pos = static_cast<float>(y_pos);
    return;
  }
  float x_offset = x_pos - last_x_pos;
  float y_offset = y_pos - last_y_pos;
  last_x_pos = static_cast<float>(x_pos);
  last_y_pos = static_cast<float>(y_pos);

  float sensitivity = 0.05f;
  x_offset *= sensitivity;
  y_offset *= sensitivity;

  static float yaw = -90.0f; // face to the box(origin point)
  static float pitch = 0.0f;

  yaw += x_offset;
  pitch -= y_offset;

  // std::cout << yaw << " " << pitch << std::endl;

  if (pitch > 89.0f) {
    pitch = 89.0f;
  } else if (pitch < -89.0f) {
    pitch = -89.0f;
  }

  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

  main_cam.SetFront(glm::normalize(front));
}

void ProcessInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  float speed = delta_time_per_frame * 5.0f;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    main_cam.PositionForward(main_cam.GetFornt() * speed);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    main_cam.PositionForward(main_cam.GetFornt() * -speed);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    main_cam.PositionForward(glm::cross(main_cam.GetFornt(), main_cam.GetUp()) *
                             -speed);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    main_cam.PositionForward(glm::cross(main_cam.GetFornt(), main_cam.GetUp()) *
                             speed);
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

  GLFWwindow *window =
      glfwCreateWindow(640, 640, "Simple Cube", nullptr, nullptr);
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
    // positions              normals        texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
  };

  glm::vec3 cube_positions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(6 * sizeof(float)));

  // setup light
  unsigned int light_VAO; // light also borrow the cube as its shape
  glGenVertexArrays(1, &light_VAO);
  glBindVertexArray(light_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);

  // setup shader
  xac::Shader light_shader("../09-multi-light/shader/light-vert.vs",
                           "../09-multi-light/shader/light-frag.fs");
  xac::Shader obj_shader("../09-multi-light/shader/vert.vs",
                         "../09-multi-light/shader/frag.fs");

  // setup texture
  auto box_tex_diffuse = xac::TextureManager::GetInstance().CreateTexture(
      "../resources/textures/container2.png", GL_TEXTURE_2D, GL_REPEAT,
      GL_REPEAT, GL_NEAREST, GL_LINEAR, GL_RGBA, GL_RGBA);

  auto box_tex_specular = xac::TextureManager::GetInstance().CreateTexture(
      "../resources/textures/container2_specular.png", GL_TEXTURE_2D, GL_REPEAT,
      GL_REPEAT, GL_NEAREST, GL_LINEAR, GL_RGBA, GL_RGBA);

  // setup matrices
  glm::mat4 model(1.0f);
  glm::mat4 light_model(1.0f);
  glm::vec3 light_pos(0.0f, 0.0f, -2.0f);
  // Because you do the transformation as the order scale->rotate->translate,
  // the model matrix should reverse it, that is translate->rotate->scale
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
    auto cur_frame_time = static_cast<float>(glfwGetTime());
    delta_time_per_frame = cur_frame_time - last_frame_time;
    last_frame_time = cur_frame_time;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(light_VAO);
    light_shader.Use();
    light_shader.SetMat4("model", light_model);
    light_shader.SetMat4("view", main_cam.GetViewMatrix());
    light_shader.SetMat4("projection", main_cam.GetProjectionMatrix());
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(VAO);
    obj_shader.Use();
    obj_shader.SetVec3("p_light.position", light_pos);
    obj_shader.SetVec3("p_light.color", glm::vec3(1.0f));
    obj_shader.SetFloat("d_light.intensity", 2.0f);
    obj_shader.SetVec3("d_light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    obj_shader.SetVec3("d_light.color", glm::vec3(0.2f, 0.5f, 0.2f));
    obj_shader.SetFloat("p_light.intensity", 2.0f);
    obj_shader.SetVec3("s_light.position", main_cam.GetPosition());
    obj_shader.SetVec3("s_light.direction", main_cam.GetFornt());
    obj_shader.SetVec3("s_light.color", glm::vec3(1.0f));
    obj_shader.SetFloat("s_light.cutoff", glm::cos(glm::radians(12.5f)));
    obj_shader.SetFloat("s_light.outer_cutoff", glm::cos(glm::radians(17.5f)));

    obj_shader.SetMat4("view", main_cam.GetViewMatrix());
    obj_shader.SetInt("mat.diffuse", static_cast<int>(box_tex_diffuse));
    obj_shader.SetInt("mat.specular", static_cast<int>(box_tex_specular));
    obj_shader.SetFloat("mat.shininess", 8);
    obj_shader.SetMat4("projection", main_cam.GetProjectionMatrix());
    obj_shader.SetVec3("view_pos", main_cam.GetPosition());

    for (unsigned int i = 0; i < 10; i++) {
      model = glm::mat4(1.0f);
      model = glm::translate(model, cube_positions[i]);
      float angle = 20.0f * static_cast<float>(i);
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      obj_shader.SetMat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  glfwTerminate();
  return 0;
}
