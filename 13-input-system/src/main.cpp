// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <assimp/Importer.hpp>
#include <cmath>
#include <filesystem>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "camera/camera.hpp"
#include "context/context.hpp"
#include "input/input_system.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "shader.hpp"

void FrameBufferSizeCB(GLFWwindow *window, int width, int height) {
  glViewport(global_imgui_width, 0, width - global_imgui_width, height);
  global_width = width;
  global_height = height;
  global_context.camera_->SetAspect(static_cast<float>(width - global_imgui_width) / height);
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

  GLFWwindow *window = glfwCreateWindow(global_width, global_height, "Hello OpenGL", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // other settings
  glfwSetFramebufferSizeCallback(window, FrameBufferSizeCB);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

  global_context.Init();
  xac::InputSystem::Init(window);
  float delta_time_per_frame;

  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 450 core");
  ImGui::StyleColorsDark();

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GALD!" << std::endl;
    return -1;
  }

  glViewport(global_imgui_width, 0, global_width - global_imgui_width, global_height);
  global_context.camera_->SetAspect(static_cast<float>(global_width - global_imgui_width) / global_height);
  glEnable(GL_DEPTH_TEST);

  // clang-format off
  std::vector<unsigned int> indices{
     0,  1,  2,  2,  3,  0, 
     4,  5,  6,  6,  7,  4,
     8,  9, 10, 10, 11,  8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20,
  };

  std::vector<xac::Mesh::Vertex> vertices{
    // positions              normals        texture coords
    {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, 
    {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}}, 
    {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, 
    {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}}, 
                                               
    {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, 
    {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, 
    {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, 
    {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, 
                                               
    {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, 
                                               
    {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, 
                                               
    {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}}, 
                                               
    {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, 
  };
  // clang-format on

  // setup objs
  xac::Model herta("../resources/models/Herta/heita.obj");
  xac::Mesh light_cube{std::move(vertices), std::move(indices), {}};

  // setup shader
  xac::Shader light_shader("../13-input-system/shader/light_vert.glsl", "../13-input-system/shader/light_frag.glsl");
  light_cube.SetShader(light_shader);
  xac::Shader herta_shader("../13-input-system/shader/herta_vert.glsl", "../13-input-system/shader/herta_frag.glsl");
  herta.SetShader(herta_shader);

  // imgui
  glm::vec4 background_color{};
  glm::vec4 light1_color{1};
  glm::vec4 light2_color{1};
  float light1_intensity = 5;
  float light2_intensity = 13;
  glm::vec3 rotation_axis{1, 1, 1};
  float rotation_degree{60};

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  while (!glfwWindowShouldClose(window)) {
    // render imgui
    {
      ImGui::NewFrame();
      ImGui::SetNextWindowPos({0, 0});
      ImGui::SetNextWindowSize({static_cast<float>(global_imgui_width), static_cast<float>(global_height)});
      ImGui::Begin("Hello, World!", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
      ImGui::SetWindowFontScale(1.2);

      ImGui::ColorEdit4("BG_color", reinterpret_cast<float *>(&background_color), ImGuiColorEditFlags_AlphaPreview);
      ImGui::ColorEdit3("light1", reinterpret_cast<float *>(&light1_color));
      ImGui::ColorEdit3("light2", reinterpret_cast<float *>(&light2_color));
      ImGui::DragFloat3("rotation_axis", reinterpret_cast<float *>(&rotation_axis), 0.005f, -1.0f, 1.0f);
      ImGui::SliderFloat("rotation_degree", &rotation_degree, 0, 360);
      ImGui::Text("x: %f, y: %f", xac::InputSystem::cursor_x_offset_, xac::InputSystem::cursor_y_offset_);

      ImVec2 mousePositionAbsolute = ImGui::GetMousePos();
      ImGui::Text("Cursor Position: %f, %f", mousePositionAbsolute.x, mousePositionAbsolute.y);
      if (ImGui::CollapsingHeader("Section 2"), ImGuiTreeNodeFlags_DefaultOpen) {
        ImGui::SetNextItemOpen(true);
        if (ImGui::TreeNode("Command")) {
          ImGui::Text("FORWARD: %d, BACKWARD: %d", xac::InCommand(xac::ControlCommand::FORWARD),
                      xac::InCommand(xac::ControlCommand::BACKWARD));
          ImGui::Text("LEFT: %4d, RIGHT: %4d", xac::InCommand(xac::ControlCommand::LEFT),
                      xac::InCommand(xac::ControlCommand::RIGHT));
          ImGui::Text("DOWN: %4d, UP: %7d", xac::InCommand(xac::ControlCommand::DOWN),
                      xac::InCommand(xac::ControlCommand::UP));
          ImGui::TreePop();
        }
      }
      ImGui::End();
      ImGui::Render();
    }

    global_context.camera_->Tick(delta_time_per_frame);
    xac::InputSystem::Tick();

    static float last_frame_time = 0.0f;
    auto cur_frame_time = static_cast<float>(glfwGetTime());
    delta_time_per_frame = cur_frame_time - last_frame_time;
    last_frame_time = cur_frame_time;

    glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 light_model(1.0f);
    float rotate_speed = 0.5;
    // make light rotate
    float decimal = rotate_speed * cur_frame_time - std::floor(rotate_speed * cur_frame_time);
    float phi = glm::radians(360 * decimal);
    glm::vec3 light_pos(5 * std::cos(phi), 0, 5 * std::sin(phi));
    light_pos = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_degree), rotation_axis) * glm::vec4(light_pos, 1);
    // Because you do the transformation as the order scale->rotate->translate,
    // the model matrix should reverse it, that is translate->rotate->scale
    light_model = glm::translate(light_model, light_pos);
    light_model = glm::scale(light_model, glm::vec3(0.2f));

    light_shader.Use();
    light_shader.SetMat4("M", light_model);
    light_shader.SetMat4("V", global_context.camera_->GetViewMatrix());
    light_shader.SetMat4("P", global_context.camera_->GetProjectionMatrix());
    light_shader.SetVec4("color", light1_color);
    light_cube.Draw();

    light_shader.Use();
    glm::vec3 light2_pos{3, 2, 0};
    auto light2_model = glm::scale(glm::translate(glm::mat4{1}, light2_pos), glm::vec3{0.2});
    light_shader.SetMat4("M", light2_model);
    light_shader.SetMat4("V", global_context.camera_->GetViewMatrix());
    light_shader.SetMat4("P", global_context.camera_->GetProjectionMatrix());
    light_shader.SetVec4("color", light2_color);
    light_cube.Draw();

    herta_shader.Use();
    herta_shader.SetVec3("lights[0].color", light1_color);
    herta_shader.SetFloat("lights[0].intensity", light1_intensity);
    herta_shader.SetVec3("lights[1].color", light2_color);
    herta_shader.SetFloat("lights[1].intensity", light2_intensity);
    herta_shader.SetMat4("Model", glm::scale(glm::translate(glm::mat4(1), glm::vec3(-3, -3, 0)), glm::vec3(0.4)));
    herta_shader.SetMat4("View", global_context.camera_->GetViewMatrix());
    herta_shader.SetMat4("Proj", global_context.camera_->GetProjectionMatrix());
    herta_shader.SetVec3("ws_cam_pos", global_context.camera_->GetPosition());
    herta_shader.SetVec3("lights[0].ws_position", light_pos);
    herta_shader.SetVec3("lights[1].ws_position", light2_pos);
    herta.Draw();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
