// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <algorithm>
#include <array>
#include <assimp/Importer.hpp>
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ranges>

#include "camera/camera.hpp"
#include "context/context.hpp"
#include "imgui_layer/imgui_layer.hpp"
#include "input/input_system.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "shader/shader.hpp"
#include "utils.hpp"
#include "window/window.hpp"

// HINT: variable prefix for specified kind of objects
// Mesh, model: m
// Shader: s
// Texture: t

static auto InCommand(xac::ControlCommand command) -> bool {
  return static_cast<uint32_t>(command) & global_context.control_commad_;
}

auto main() -> int {
  global_context.Init();
  float delta_time_per_frame;

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GALD!" << std::endl;
    return -1;
  }

  glViewport(
      global_context.imgui_width_, 0, global_context.window_width_ - global_context.imgui_width_,
      global_context.window_height_
  );
  global_context.camera_->SetAspect(
      static_cast<float>(global_context.window_width_ - global_context.imgui_width_) / global_context.window_height_
  );
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#pragma region mesh data
  // clang-format off
  std::vector<unsigned int> light_box_indices {
     0,  1,  2,  2,  3,  0, 
     4,  5,  6,  6,  7,  4,
     8,  9, 10, 10, 11,  8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20,
  };

  std::vector<unsigned int> ground_indices {
    0, 1, 2, 2, 3, 0,
  };

  std::vector<unsigned int> window_indices {
    12, 13, 14, 14, 15, 12,
  };

  std::vector<glm::vec3> window_positions {
    {-15.0f, 0.0f, -4.8f },
    { 15.0f, 0.0f, 5.1f },
    { 0.0f, 0.0f, 7.0f },
    {-3.0f, 0.0f, -23.f },
    { 5.0f, 0.0f, -6.f }
  };

  std::vector<std::vector<unsigned int>> cage_face_indices {
    { 0,  1,  2,  2,  3,  0}, 
    { 4,  5,  6,  6,  7,  4},
    { 8,  9, 10, 10, 11,  8},
    {12, 13, 14, 14, 15, 12},
    {16, 17, 18, 18, 19, 16},
    {20, 21, 22, 22, 23, 20},
  };

  std::vector<xac::Mesh::Vertex> quad_vertices {
    // positions              normals        texture coords
    {{-1.0f,  1.0f, 0.0f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}}, 
    {{ 1.0f,  1.0f, 0.0f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, 
    {{ 1.0f, -1.0f, 0.0f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}}, 
    {{-1.0f, -1.0f, 0.0f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, 
  };

  std::vector<xac::Mesh::Vertex> ground_vertices {
    // positions              normals        texture coords
    {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, 
  };
  // clang-format on
#pragma endregion

#pragma region uniform buffer object
  unsigned int ubo;
  glCreateBuffers(1, &ubo);
  glNamedBufferStorage(ubo, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
#pragma endregion

#pragma region setup objs
  xac::Model m_herta("../resources/models/Herta/heita.obj");
  xac::Model m_sphere("../resources/models/Sphere/sphere.obj");
  xac::Model m_cube("../resources/models/Cube/cube.obj");
  xac::Mesh m_ground{ground_vertices, ground_indices, {glm::vec3{0}, glm::vec3{0.5}, glm::vec3{0.5}, {}}, "ground"};
  xac::Mesh m_quad{quad_vertices, {0, 2, 1, 0, 3, 2}, {}, "quad"};
  xac::Model m_box = m_cube;
  // xac::Model m_skybox = m_cube;
  xac::Model m_light = m_sphere;

  auto s_unlit =
      std::make_shared<xac::Shader>("../22-shadow/shader/common.vert.glsl", "../22-shadow/shader/unlit.frag.glsl");
  auto s_lit =
      std::make_shared<xac::Shader>("../22-shadow/shader/common.vert.glsl", "../22-shadow/shader/lit.frag.glsl");
  auto s_skybox =
      std::make_shared<xac::Shader>("../22-shadow/shader/skybox.vert.glsl", "../22-shadow/shader/skybox.frag.glsl");
  auto t_box_diffuse = xac::LoadTextureFromFile("../resources/textures/container2.png");
  auto t_box_specular = xac::LoadTextureFromFile("../resources/textures/container2_specular.png");
  auto t_ground_diffuse = xac::LoadTextureFromFile("../resources/textures/wood.png");
  auto t_white = xac::LoadTextureFromFile("../resources/textures/white.png");
  auto t_skybox = xac::LoadCubemapFromFile({
      "../resources/textures/skybox/right.jpg",
      "../resources/textures/skybox/left.jpg",
      "../resources/textures/skybox/top.jpg",
      "../resources/textures/skybox/bottom.jpg",
      "../resources/textures/skybox/front.jpg",
      "../resources/textures/skybox/back.jpg",
  });

  m_light.SetShader(s_unlit);
  m_herta.SetShader(s_lit);
  m_ground.SetShader(s_lit);
  m_box.SetShader(s_lit);
  m_quad.SetShader(s_unlit);
#pragma endregion

#pragma region depth framebuffer
  float depth_map_h_w = 1024;
  unsigned int fbo_depth_map;
  glGenFramebuffers(1, &fbo_depth_map);
  unsigned int t_depth_map;
  glGenTextures(1, &t_depth_map);
  glBindTexture(GL_TEXTURE_2D, t_depth_map);
  // glTexImage2D(
  //     GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depth_map_h_w, depth_map_h_w, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
  // );
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, depth_map_h_w, depth_map_h_w, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // for uv out of texture(not exceeding far plane)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glm::vec4 border_color{1.0, 1.0, 1.0, 1.0};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(border_color));
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_depth_map);
  // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, t_depth_map, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t_depth_map, 0);
  // glDrawBuffer(GL_NONE);
  // glReadBuffer(GL_NONE);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

#pragma region Draw Box Helper
  auto DrawBox = [](std::array<glm::vec3, 8> vertex) {
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), vertex.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    std::array<GLuint, 24> indices = {// 底面边
                                      0, 1, 1, 2, 2, 3, 3, 0,
                                      // 顶面边
                                      4, 5, 5, 6, 6, 7, 7, 4,
                                      // 立方体的四个“侧面”边
                                      0, 4, 1, 5, 2, 6, 3, 7};
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // 绘制线条
    glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);

    // 解绑VAO
    glBindVertexArray(0);

    // 删除资源
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
  };
#pragma endregion

#pragma region imgui variables
  struct PointLight {
    glm::vec4 color = glm::vec4{1};
    float intensity = 5;
  };
  struct DirectLight {
    glm::vec4 color = glm::vec4{1};
    float intensity = 1;
    glm::vec3 direction = glm::vec3{1, 1, 1};
  };

  glm::vec4 background_color{};
  PointLight p_lights[3];
  DirectLight d_lights[1];
  glm::vec3 rotation_axis{1, 1, 1};
  float rotation_degree{60};

  global_context.imgui_layer_->RegisterWatchVar<int>("gl_depth_func", GL_LEQUAL - GL_NEVER, [](int new_val) {
    std::cout << "init depth func" << std::endl;
    glDepthFunc(GL_NEVER + new_val);
  });
  global_context.imgui_layer_->RegisterWatchVar<int>("shader_debug_mode", 0, [&](int new_val) {
    switch (new_val) {
      case 0:  // NO_DEBUG
        s_lit->ClearDefineGroup("DEBUG");
        s_lit->CompileShaders();
        break;
      case 1:  // DEBUG_NORMAL
        s_lit->ClearDefineGroup("DEBUG");
        s_lit->AddDefine("DEBUG", "DEBUG_NORMAL");
        s_lit->CompileShaders();
        break;
      case 2:  // DEBUG_DEPTH
        s_lit->ClearDefineGroup("DEBUG");
        s_lit->AddDefine("DEBUG", "DEBUG_DEPTH");
        s_lit->CompileShaders();
        break;
    }
  });
  global_context.imgui_layer_->RegisterWatchVar<bool>("cull_face_enable", true, [](bool new_val) {
    if (new_val) {
      glEnable(GL_CULL_FACE);
    } else {
      glDisable(GL_CULL_FACE);
    }
  });
  global_context.imgui_layer_->RegisterWatchVar<int>("cull_face_mode", 5, [](int new_val) {
    glCullFace(GL_FRONT_LEFT + new_val);
  });
  global_context.imgui_layer_->RegisterWatchVar<int>("lighting_mode", 0, [&](int new_val) {
    switch (new_val) {
      case 0:  // PHONG
        s_lit->ClearDefineGroup("LIGHTING_MODEL");
        s_lit->CompileShaders();
        break;
      case 1:  // BLINN_PHONG
        s_lit->ClearDefineGroup("LIGHTING_MODEL");
        s_lit->AddDefine("LIGHTING_MODEL", "BLINN_PHONG");
        s_lit->CompileShaders();
        break;
    }
  });
  global_context.imgui_layer_->RegisterWatchVar<int>("shadow_mode", 0, [&](int new_val) {
    switch (new_val) {
      case 0:  // no shadow
        s_lit->ClearDefineGroup("SHADOW_MODEL");
        s_lit->CompileShaders();
        break;
      case 1:  // PCF
        s_lit->ClearDefineGroup("SHADOW_MODEL");
        s_lit->AddDefine("SHADOW_MODEL", "PCF_SHADOW");
        s_lit->CompileShaders();
        break;
      case 2:  // PCSS
        s_lit->ClearDefineGroup("SHADOW_MODEL");
        s_lit->AddDefine("SHADOW_MODEL", "PCSS_SHADOW");
        s_lit->CompileShaders();
        break;
    }
  });

#pragma endregion

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glm::vec3 d_light0_pos = d_lights[0].direction * 60.0f;

  auto DrawScene = [&](float delta_time, xac::Camera &camera) {
    glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 proj = camera.GetProjectionMatrix();
    glNamedBufferSubData(ubo, 0, sizeof(glm::mat4), reinterpret_cast<void *>(&view));
    glNamedBufferSubData(ubo, sizeof(glm::mat4), sizeof(glm::mat4), reinterpret_cast<void *>(&proj));

#pragma region render light
    glm::mat4 light0_model(1.0f);
    float rotate_speed = 0.5;
    // make light rotate
    static float decimal = 0;
    decimal += rotate_speed * delta_time - std::floor(rotate_speed * delta_time);
    float phi = glm::radians(360 * decimal);
    glm::vec3 p_light0_pos(5 * std::cos(phi), 0, 5 * std::sin(phi));
    p_light0_pos =
        glm::rotate(glm::mat4(1.0f), glm::radians(rotation_degree), rotation_axis) * glm::vec4(p_light0_pos, 1);
    // Because you do the transformation by the order scale->rotate->translate,
    // glm functions are doing right multiply, so
    // the model matrix should reverse it, that is translate->rotate->scale
    light0_model = glm::translate(light0_model, p_light0_pos);
    light0_model = glm::scale(light0_model, glm::vec3(0.2f));

    s_unlit->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_white);
    s_unlit->SetInt("tex", 0);

    s_unlit->Use();
    s_unlit->SetMat4("Model", light0_model);
    s_unlit->SetVec4("color", p_lights[0].color);
    m_light.Draw();

    s_unlit->Use();
    glm::vec3 p_light1_pos{3, 2, 0};
    auto light1_model = glm::scale(glm::translate(glm::mat4{1}, p_light1_pos), glm::vec3{0.2});
    s_unlit->SetMat4("Model", light1_model);
    s_unlit->SetVec4("color", p_lights[1].color);
    m_light.Draw();

    // s_unlit->Use();
    // glm::vec3 p_light2_pos{-30, 20, 0};
    // auto light2_model = glm::scale(glm::translate(glm::mat4{1}, p_light2_pos), glm::vec3{2});
    // s_unlit->SetMat4("Model", light2_model);
    // s_unlit->SetVec4("color", p_lights[2].color);
    // m_light.Draw();

    s_unlit->Use();
    auto d_light_model = glm::scale(glm::translate(glm::mat4{1}, d_light0_pos), glm::vec3{3.0f});
    s_unlit->SetMat4("Model", d_light_model);
    s_unlit->SetVec4("color", d_lights[0].color);
    m_light.Draw();
#pragma endregion

#pragma region common settings for obj shader
    s_lit->Use();
    s_lit->SetVec3("p_lights[0].ws_position", p_light0_pos);
    s_lit->SetVec3("p_lights[0].color", p_lights[0].color);
    s_lit->SetFloat("p_lights[0].intensity", p_lights[0].intensity);
    s_lit->SetVec3("p_lights[1].ws_position", p_light1_pos);
    s_lit->SetVec3("p_lights[1].color", p_lights[1].color);
    s_lit->SetFloat("p_lights[1].intensity", p_lights[1].intensity);
    // s_lit->SetVec3("p_lights[2].ws_position", light2_pos);
    // s_lit->SetVec3("p_lights[2].color", p_lights[2].color);
    // s_lit->SetFloat("p_lights[2].intensity", p_lights[2].intensity * 300);

    s_lit->SetVec3("d_lights[0].direction", d_lights[0].direction);
    s_lit->SetVec3("d_lights[0].color", d_lights[0].color);
    s_lit->SetFloat("d_lights[0].intensity", d_lights[0].intensity);
    s_lit->SetVec3("ws_cam_pos", camera.GetPosition());
#pragma endregion

#pragma region render ground
    s_lit->Use();
    auto ground_model = glm::mat4(1);
    // you should do scale and rotation at origin!
    // ground_model = glm::translate(ground_model, {0, 0, 0});
    // ground_model = glm::rotate(ground_model, glm::radians(30.0f), {0, 1, 0});
    ground_model = glm::scale(ground_model, {150, 1, 150});
    ground_model = glm::translate(ground_model, {0, -0.5, 0});  // move to origin then scale
    s_lit->SetMat4("Model", ground_model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_ground_diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, t_white);
    s_lit->SetInt("texture_diffuse0", 0);
    s_lit->SetInt("texture_specular0", 1);
    m_ground.Draw();
#pragma endregion

#pragma region render cubes
    s_lit->Use();
    s_lit->SetVec4("base_color", glm::vec4{1});
    // you should do scale and rotation at origin!
    auto cube_model = glm::mat4(1);
    cube_model = glm::translate(cube_model, {20.0, 0, -30.0});
    cube_model = glm::rotate(cube_model, glm::radians(30.0f), {0, 1, 0});
    cube_model = glm::scale(cube_model, {5, 5, 5});
    s_lit->SetMat4("Model", cube_model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_box_diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, t_box_specular);
    s_lit->SetInt("texture_diffuse0", 0);
    s_lit->SetInt("texture_specular0", 1);
    m_box.Draw();

    cube_model = glm::mat4(1);
    cube_model = glm::translate(cube_model, {0, 25.0, 10});
    cube_model = glm::rotate(cube_model, glm::radians(20.0f), {0, 1, 0});
    cube_model = glm::scale(cube_model, glm::vec3{2});
    s_lit->SetMat4("Model", cube_model);

    m_box.Draw();
#pragma endregion

#pragma region render herta
    s_lit->Use();
    s_lit->SetMat4("Model", glm::scale(glm::translate(glm::mat4(1), glm::vec3(-3, 0, 0)), glm::vec3(0.4)));
    m_herta.Draw();
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glDepthFunc(GL_NEVER + global_context.imgui_layer_->GetWatchVar<int>("gl_depth_func").Value());
    // HINT: or else can't clear stencil buffer bit
    glad_glStencilMask(0xFF);
#pragma endregion
  };

  // SECTION: Render loop start
  while (!global_context.window_->ShouldClose()) {
#pragma region Time and Frame
    static float last_frame_time = 0.0f;
    auto cur_frame_time = static_cast<float>(glfwGetTime());
    delta_time_per_frame = cur_frame_time - last_frame_time;
    static float time_counter = 0;
    static int frame_count;
    static int count = 0;
    count += 1;
    time_counter += delta_time_per_frame;
    if (time_counter > 1) {  // every 1s
      frame_count = count;
      count = 0;
      time_counter = 0;
    }
    last_frame_time = cur_frame_time;
#pragma endregion

    global_context.camera_->Tick(delta_time_per_frame);
    xac::InputSystem::Tick();

#pragma region Render Depth Map
    auto light_cam = xac::Camera(d_light0_pos, glm::vec3{0}, xac::Camera::ProjectionMethod::ORTHO);
    auto frustum = global_context.camera_->GetFrustumInWorld();
    auto light_cam_view = light_cam.GetViewMatrix();
    std::ranges::for_each(frustum, [&light_cam_view](auto &&p) {
      auto p_ls = light_cam_view * glm::vec4{p, 1.0};
      p = glm::vec3{p_ls / p_ls.w};
    });
    // HINT: light space
    auto min_max_x = std::ranges::minmax(frustum, {}, &glm::vec3::x);
    auto min_max_y = std::ranges::minmax(frustum, {}, &glm::vec3::y);
    auto min_max_z = std::ranges::minmax(frustum, {}, &glm::vec3::z);
    auto left = min_max_x.min.x;
    auto right = min_max_x.max.x;
    auto bottom = min_max_y.min.y;
    auto top = min_max_y.max.y;
    auto near = min_max_z.min.z;
    auto far = min_max_z.max.z;
    // std::cout << left << " " << right << " " << bottom << " " << top << " " << near << " " << far << std::endl;
    light_cam.SetOrtho(left, right, bottom, top);
    light_cam.SetNear(near);
    light_cam.SetFar(far);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_depth_map);
    glViewport(0, 0, depth_map_h_w, depth_map_h_w);
    DrawScene(delta_time_per_frame, light_cam);
#pragma endregion
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(
        global_context.imgui_width_, 0, global_context.window_width_ - global_context.imgui_width_,
        global_context.window_height_
    );
    s_lit->Use();
    s_lit->SetMat4("world_to_light_space_matrix", light_cam.GetProjectionMatrix() * light_cam.GetViewMatrix());
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, t_depth_map);
    s_lit->SetInt("depth_map", 12);
    DrawScene(delta_time_per_frame, *global_context.camera_);

    // s_unlit->Use();
    // auto identity = glm::mat4{1};
    // s_unlit->SetMat4("Model", identity);
    // glm::mat4 view = global_context.camera_->GetViewMatrix();
    // glm::mat4 proj = global_context.camera_->GetProjectionMatrix();
    // glNamedBufferSubData(ubo, 0, sizeof(glm::mat4), reinterpret_cast<void *>(&view));
    // glNamedBufferSubData(ubo, sizeof(glm::mat4), sizeof(glm::mat4), reinterpret_cast<void *>(&proj));
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, t_white);
    // s_unlit->SetInt("tex", 0);
    // DrawBox({
    //     glm::vec3{-0.8f, -0.8f, -0.8f},
    //     glm::vec3{-0.8f, -0.8f, 0.8f},
    //     glm::vec3{-0.8f, 0.8f, -0.8f},
    //     glm::vec3{-0.8f, 0.8f, 0.8f},
    //     glm::vec3{0.8f, 0.8f, -0.8f},
    //     glm::vec3{0.8f, 0.8f, 0.8f},
    //     glm::vec3{0.8f, -0.8f, -0.8f},
    //     glm::vec3{0.8f, -0.8f, 0.8f},
    // });
#pragma region render to quad
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glClearColor(0.5f, 1.0f, 1.0f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // glViewport(
    //     global_context.imgui_width_, 0, global_context.window_width_ - global_context.imgui_width_,
    //     global_context.window_height_
    // );
    // glDisable(GL_DEPTH_TEST);
    // s_unlit->Use();
    // auto identity = glm::mat4{1.0};
    // s_unlit->SetMat4("Model", identity);
    // glNamedBufferSubData(ubo, 0, sizeof(glm::mat4), reinterpret_cast<void *>(&identity));
    // glNamedBufferSubData(ubo, sizeof(glm::mat4), sizeof(glm::mat4), reinterpret_cast<void *>(&identity));
    // s_unlit->SetVec4("color", glm::vec4{1.0f});
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, t_ground_diffuse);
    // s_unlit->SetInt("tex", 0);
    // m_quad.Draw();
#pragma endregion
    global_context.imgui_layer_->Render();

#pragma region render imgui
    {}
#pragma endregion

    global_context.window_->SwapBuffers();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  return 0;
}
