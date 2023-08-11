// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../common/stb_image.h"
#include "camera.h"
#include "mesh.hpp"
#include "shader.h"

glm::vec3 cam_pos(0.0f, 0.0f, 5.0f);
glm::vec3 target_pos(0.0f, 0.0f, -2.0f);
xac::Camera main_cam(cam_pos, target_pos);
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
  float speed = delta_time_per_frame * 5.0f;
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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
  std::vector<unsigned int> indices{
     0,  1,  2,  2,  3,  0, 
     4,  5,  6,  6,  7,  4,
     8,  9, 10, 10, 11,  8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 13, 20,
  };

  std::vector<xac::Mesh::Vertex> vertices{
    // positions              normals        texture coords
    {.position = glm::vec3{-0.5f, -0.5f, -0.5f}, .normal = glm::vec3{ 0.0f,  0.0f, -1.0f}, .texcoord = glm::vec2{0.0f, 0.0f}},
    {.position = glm::vec3{ 0.5f, -0.5f, -0.5f}, .normal = glm::vec3{ 0.0f,  0.0f, -1.0f}, .texcoord = glm::vec2{1.0f, 0.0f}},
    {.position = glm::vec3{ 0.5f,  0.5f, -0.5f}, .normal = glm::vec3{ 0.0f,  0.0f, -1.0f}, .texcoord = glm::vec2{1.0f, 1.0f}},
    {.position = glm::vec3{-0.5f,  0.5f, -0.5f}, .normal = glm::vec3{ 0.0f,  0.0f, -1.0f}, .texcoord = glm::vec2{0.0f, 1.0f}},
                                                                                         
    {.position = glm::vec3{-0.5f, -0.5f,  0.5f}, .normal = glm::vec3{ 0.0f,  0.0f,  1.0f}, .texcoord = glm::vec2{0.0f, 0.0f}},
    {.position = glm::vec3{ 0.5f, -0.5f,  0.5f}, .normal = glm::vec3{ 0.0f,  0.0f,  1.0f}, .texcoord = glm::vec2{1.0f, 0.0f}},
    {.position = glm::vec3{ 0.5f,  0.5f,  0.5f}, .normal = glm::vec3{ 0.0f,  0.0f,  1.0f}, .texcoord = glm::vec2{1.0f, 1.0f}},
    {.position = glm::vec3{-0.5f,  0.5f,  0.5f}, .normal = glm::vec3{ 0.0f,  0.0f,  1.0f}, .texcoord = glm::vec2{0.0f, 1.0f}},
                                                                                         
    {.position = glm::vec3{-0.5f,  0.5f,  0.5f}, .normal = glm::vec3{-1.0f,  0.0f,  0.0f}, .texcoord = glm::vec2{1.0f, 0.0f}},
    {.position = glm::vec3{-0.5f,  0.5f, -0.5f}, .normal = glm::vec3{-1.0f,  0.0f,  0.0f}, .texcoord = glm::vec2{1.0f, 1.0f}},
    {.position = glm::vec3{-0.5f, -0.5f, -0.5f}, .normal = glm::vec3{-1.0f,  0.0f,  0.0f}, .texcoord = glm::vec2{0.0f, 1.0f}},
    {.position = glm::vec3{-0.5f, -0.5f,  0.5f}, .normal = glm::vec3{-1.0f,  0.0f,  0.0f}, .texcoord = glm::vec2{0.0f, 0.0f}},
                                                                                         
    {.position = glm::vec3{ 0.5f,  0.5f,  0.5f}, .normal = glm::vec3{ 1.0f,  0.0f,  0.0f}, .texcoord = glm::vec2{1.0f, 0.0f}},
    {.position = glm::vec3{ 0.5f,  0.5f, -0.5f}, .normal = glm::vec3{ 1.0f,  0.0f,  0.0f}, .texcoord = glm::vec2{1.0f, 1.0f}},
    {.position = glm::vec3{ 0.5f, -0.5f, -0.5f}, .normal = glm::vec3{ 1.0f,  0.0f,  0.0f}, .texcoord = glm::vec2{0.0f, 1.0f}},
    {.position = glm::vec3{ 0.5f, -0.5f,  0.5f}, .normal = glm::vec3{ 1.0f,  0.0f,  0.0f}, .texcoord = glm::vec2{0.0f, 0.0f}},
                                                                                         
    {.position = glm::vec3{-0.5f, -0.5f, -0.5f}, .normal = glm::vec3{ 0.0f, -1.0f,  0.0f}, .texcoord = glm::vec2{0.0f, 1.0f}},
    {.position = glm::vec3{ 0.5f, -0.5f, -0.5f}, .normal = glm::vec3{ 0.0f, -1.0f,  0.0f}, .texcoord = glm::vec2{1.0f, 1.0f}},
    {.position = glm::vec3{ 0.5f, -0.5f,  0.5f}, .normal = glm::vec3{ 0.0f, -1.0f,  0.0f}, .texcoord = glm::vec2{1.0f, 0.0f}},
    {.position = glm::vec3{-0.5f, -0.5f,  0.5f}, .normal = glm::vec3{ 0.0f, -1.0f,  0.0f}, .texcoord = glm::vec2{0.0f, 0.0f}},
                                                                                         
    {.position = glm::vec3{-0.5f,  0.5f, -0.5f}, .normal = glm::vec3{ 0.0f,  1.0f,  0.0f}, .texcoord = glm::vec2{0.0f, 1.0f}},
    {.position = glm::vec3{ 0.5f,  0.5f, -0.5f}, .normal = glm::vec3{ 0.0f,  1.0f,  0.0f}, .texcoord = glm::vec2{1.0f, 1.0f}},
    {.position = glm::vec3{ 0.5f,  0.5f,  0.5f}, .normal = glm::vec3{ 0.0f,  1.0f,  0.0f}, .texcoord = glm::vec2{1.0f, 0.0f}},
    {.position = glm::vec3{-0.5f,  0.5f,  0.5f}, .normal = glm::vec3{ 0.0f,  1.0f,  0.0f}, .texcoord = glm::vec2{0.0f, 0.0f}},
  };
  // clang-format on

  // setup objs
  xac::Mesh cube{std::move(vertices), std::move(indices)};

  // setup shader
  xac::Shader light_shader("../10-mesh/shader/light_vert.glsl", "../10-mesh/shader/light_frag.glsl");
  xac::Shader obj_shader("../10-mesh/shader/cube_vert.glsl", "../10-mesh/shader/cube_frag.glsl");

  // setup texture
  int width, height, nr_channels;
  unsigned char *tex_data = stbi_load("../resources/textures/container2.png", &width, &height, &nr_channels, 0);
  unsigned int box_tex_diffuse;
  glGenTextures(1, &box_tex_diffuse);
  glActiveTexture(GL_TEXTURE0);  // active texture0 before bind
  glBindTexture(GL_TEXTURE_2D, box_tex_diffuse);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(tex_data);

  tex_data = stbi_load("../resources/textures/container2_specular.png", &width, &height, &nr_channels, 0);
  unsigned int box_tex_specular;
  glGenTextures(1, &box_tex_specular);
  glActiveTexture(GL_TEXTURE1);  // active texture0 before bind
  glBindTexture(GL_TEXTURE_2D, box_tex_specular);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(tex_data);

  // setup matrices
  glm::mat4 model(1.0f);
  glm::mat4 light_model(1.0f);
  glm::vec3 light_pos(1.2f, 1.0f, 2.2f);
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

    light_shader.Use();
    light_shader.SetMat4("M", light_model);
    light_shader.SetMat4("V", main_cam.GetViewMatrix());
    light_shader.SetMat4("P", main_cam.GetProjectionMatrix());
    cube.Draw();

    obj_shader.Use();
    model = glm::rotate(model, std::sin(cur_frame_time) * 0.001f * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    model = glm::rotate(model, std::sin(cur_frame_time) * 0.001f * glm::radians(50.0f), glm::vec3(0.5f, 0.0f, 0.5f));
    obj_shader.SetMat4("Model", model);
    obj_shader.SetMat4("View", main_cam.GetViewMatrix());
    obj_shader.SetMat4("Proj", main_cam.GetProjectionMatrix());
    obj_shader.SetVec3("light.color", glm::vec3(1.0f));
    obj_shader.SetVec3("light.ws_pos", glm::vec3(light_model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
    obj_shader.SetVec3("ws_view_pos", main_cam.GetPosition());
    obj_shader.SetInt("mat.diffuse", 0);
    obj_shader.SetInt("mat.specular", 1);
    obj_shader.SetFloat("mat.shininess", 64);
    cube.Draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
