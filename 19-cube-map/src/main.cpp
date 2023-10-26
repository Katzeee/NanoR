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
#include "shader/shader.hpp"
#include "utils.hpp"


// HINT: variable prefix for specified kind of objects
// Mesh, model: m
// Shader: s
// Texture: t

void FrameBufferSizeCB(GLFWwindow *window, int width, int height) {
  glViewport(global_context.imgui_width_, 0, width - global_context.imgui_width_, height);
  global_context.window_width_ = width;
  global_context.window_height_ = height;
  global_context.camera_->SetAspect(static_cast<float>(width - global_context.imgui_width_) / height);
}

static auto InCommand(xac::ControlCommand command) -> bool {
  return static_cast<uint32_t>(command) & global_context.control_commad_;
}

auto main() -> int {
#pragma region window init
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

  GLFWwindow *window =
      glfwCreateWindow(global_context.window_width_, global_context.window_height_, "Hello OpenGL", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);  // disable vsync

  glfwSetFramebufferSizeCallback(window, FrameBufferSizeCB);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
#pragma endregion

  global_context.Init();
  xac::InputSystem::Init(window);
  float delta_time_per_frame;

#pragma region imgui init
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 450 core");
  ImGui::StyleColorsDark();
#pragma endregion

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GALD!" << std::endl;
    return -1;
  }

  glViewport(global_context.imgui_width_, 0, global_context.window_width_ - global_context.imgui_width_,
             global_context.window_height_);
  global_context.camera_->SetAspect(static_cast<float>(global_context.window_width_ - global_context.imgui_width_) /
                                    global_context.window_height_);
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
    20, 21, 22, 22, 23, 20,
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

  std::vector<xac::Mesh::Vertex> cage_vertices {
    // positions              normals        texture coords
    {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, 
    {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}}, 
    {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, 
    {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}}, 
                                               
    {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, 
    {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, 
    {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, 
    {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, 
                                               
    {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, 
                                               
    {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, 
                                               
    {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}}, 
                                               
    {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, 
  };

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

  // clang-format on
#pragma endregion

#pragma region setup objs
  xac::Model m_herta("../resources/models/Herta/heita.obj");
  xac::Model m_sphere("../resources/models/Sphere/sphere.obj");
  xac::Model m_cube("../resources/models/Cube/cube.obj");
  xac::Mesh m_ground{cage_vertices, ground_indices, {}, "ground"};
  xac::Mesh m_window{cage_vertices, window_indices, {}, "window"};
  xac::Mesh m_quad{quad_vertices, {0, 2, 1, 0, 3, 2}, {}, "quad"};
  xac::Model m_box = m_cube;
  // xac::Model m_skybox = m_cube;
  xac::Model m_light = m_sphere;

  auto s_unlit =
      std::make_shared<xac::Shader>("../19-cube-map/shader/common.vert.glsl", "../19-cube-map/shader/unlit.frag.glsl");
  auto s_lit =
      std::make_shared<xac::Shader>("../19-cube-map/shader/common.vert.glsl", "../19-cube-map/shader/lit.frag.glsl");
  auto s_skybox =
      std::make_shared<xac::Shader>("../19-cube-map/shader/skybox.vert.glsl", "../19-cube-map/shader/skybox.frag.glsl");
  auto s_reflect = std::make_shared<xac::Shader>("../19-cube-map/shader/common.vert.glsl",
                                                 "../19-cube-map/shader/reflect.frag.glsl");

  auto t_ground_diffuse = xac::LoadTextureFromFile("../resources/textures/container2.png");
  auto t_ground_specular = xac::LoadTextureFromFile("../resources/textures/container2_specular.png");
  auto t_transparent_window = xac::LoadTextureFromFile("../resources/textures/blending_transparent_window.png");
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
  m_window.SetShader(s_lit);
  m_quad.SetShader(s_unlit);
  // m_skybox.SetShader(s_skybox);
  // m_sphere.SetShader(s_refract);
  m_cube.SetShader(s_reflect);

  std::array<std::shared_ptr<xac::Mesh>, 6> m_cage_faces;
  for (int i = 0; i < 6; i++) {
    m_cage_faces[i] = std::make_shared<xac::Mesh>(cage_vertices, cage_face_indices[i], xac::Material{});
    m_cage_faces[i]->SetShader(s_lit);
  }

#pragma endregion

#pragma region frame buffer
  unsigned int fbo_screen;
  glGenFramebuffers(1, &fbo_screen);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_screen);
  unsigned int t_fb_screen_color;
  glGenTextures(1, &t_fb_screen_color);
  glBindTexture(GL_TEXTURE_2D, t_fb_screen_color);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, global_context.window_width_ - global_context.imgui_width_,
               global_context.window_height_, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // HINT: bind color buffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t_fb_screen_color, 0);
  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                        global_context.window_width_ - global_context.imgui_width_, global_context.window_height_);
  // HINT: bind depth buffer
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "ERROR::FRAMEBUFFER: Framebuffer is not complete!" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

#pragma region Dynamic Environment Mapping
  unsigned int t_fb_cube_map;
  glGenTextures(1, &t_fb_cube_map);
  glBindTexture(GL_TEXTURE_CUBE_MAP, t_fb_cube_map);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  unsigned int fbo_dem[6];
  glGenFramebuffers(6, fbo_dem);
  for (int i = 0; i < 6; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_dem[i]);
    // HINT: allocate memory
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, t_fb_cube_map, 0);
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cout << "ERROR::FRAMEBUFFER: Framebuffer is not complete!" << std::endl;
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
  PointLight p_lights[2];
  DirectLight d_lights[1];
  glm::vec3 rotation_axis{1, 1, 1};
  float rotation_degree{60};

  int gl_depth_func = GL_LEQUAL - GL_NEVER;
  xac::CheckChangeThen check_gl_depth_func{&gl_depth_func, [](int new_val) { glDepthFunc(GL_NEVER + new_val); }};
  int shader_debug_mode = 0;
  xac::CheckChangeThen check_shader_debug_mode{&shader_debug_mode, [&](int new_val) {
                                                 switch (new_val) {
                                                   case 0:  // NO_DEBUG
                                                     s_lit->ClearDefine();
                                                     s_lit->CompileShaders();
                                                     break;
                                                   case 1:  // DEBUG_NORMAL
                                                     s_lit->ClearDefine();
                                                     s_lit->AddDefine("DEBUG_NORMAL");
                                                     s_lit->CompileShaders();
                                                     break;
                                                   case 2:  // DEBUG_DEPTH
                                                     s_lit->ClearDefine();
                                                     s_lit->AddDefine("DEBUG_DEPTH");
                                                     s_lit->CompileShaders();
                                                     break;
                                                 }
                                               }};
  bool face_culling_enable = true;
  xac::CheckChangeThen check_face_culling_enable{&face_culling_enable, [](bool new_val) {
                                                   if (new_val) {
                                                     glEnable(GL_CULL_FACE);
                                                   } else {
                                                     glDisable(GL_CULL_FACE);
                                                   }
                                                 }};
  int culled_face = 5;
  xac::CheckChangeThen check_culled_face{&culled_face, [](int new_val) { glCullFace(GL_FRONT_LEFT + new_val); }};

  bool debug_sky_map = false;
  xac::CheckChangeThen check_debug_sky_map{&debug_sky_map, [](int new_val) {}};

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
#pragma endregion

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  auto DrawScene = [&](float delta_time, xac::Camera &camera) {
    glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
    if (!debug_sky_map) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
#pragma region render light
    glm::mat4 light_model(1.0f);
    float rotate_speed = 0.5;
    // make light rotate
    static float decimal = 0;
    decimal += rotate_speed * delta_time - std::floor(rotate_speed * delta_time);
    float phi = glm::radians(360 * decimal);
    glm::vec3 light_pos(5 * std::cos(phi), 0, 5 * std::sin(phi));
    light_pos = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_degree), rotation_axis) * glm::vec4(light_pos, 1);
    // Because you do the transformation by the order scale->rotate->translate,
    // glm functions are doing right multiply, so
    // the model matrix should reverse it, that is translate->rotate->scale
    light_model = glm::translate(light_model, light_pos);
    light_model = glm::scale(light_model, glm::vec3(0.2f));

    s_unlit->Use();
    s_unlit->SetMat4("View", camera.GetViewMatrix());
    s_unlit->SetMat4("Proj", camera.GetProjectionMatrix());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_white);
    s_unlit->SetInt("tex", 0);

    s_unlit->Use();
    s_unlit->SetMat4("Model", light_model);
    s_unlit->SetVec4("color", p_lights[0].color);
    m_light.Draw();

    s_unlit->Use();
    glm::vec3 light2_pos{3, 2, 0};
    auto light2_model = glm::scale(glm::translate(glm::mat4{1}, light2_pos), glm::vec3{0.2});
    s_unlit->SetMat4("Model", light2_model);
    s_unlit->SetVec4("color", p_lights[1].color);
    m_light.Draw();

    s_unlit->Use();
    glm::vec3 d_light_pos = d_lights[0].direction * 40.0f;
    auto d_light_model = glm::scale(glm::translate(glm::mat4{1}, d_light_pos), glm::vec3{3.0f});
    s_unlit->SetMat4("Model", d_light_model);
    s_unlit->SetVec4("color", d_lights[0].color);
    m_light.Draw();
#pragma endregion

#pragma region common settings for obj shader
    s_lit->Use();
    s_lit->SetVec3("p_lights[0].ws_position", light_pos);
    s_lit->SetVec3("p_lights[1].ws_position", light2_pos);
    s_lit->SetVec3("p_lights[0].color", p_lights[0].color);
    s_lit->SetFloat("p_lights[0].intensity", p_lights[0].intensity);
    s_lit->SetVec3("p_lights[1].color", p_lights[1].color);
    s_lit->SetFloat("p_lights[1].intensity", p_lights[1].intensity);

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
    s_lit->SetMat4("View", camera.GetViewMatrix());
    s_lit->SetMat4("Proj", camera.GetProjectionMatrix());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_ground_diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, t_ground_specular);
    s_lit->SetInt("texture_diffuse0", 0);
    s_lit->SetInt("texture_specular0", 1);
    m_ground.Draw();
#pragma endregion

#pragma region render cage
    glStencilMask(0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    s_lit->Use();
    auto cage_model = glm::mat4{1};
    cage_model = glm::translate(cage_model, {10, 0, 15});
    cage_model = glm::scale(cage_model, glm::vec3{15});
    cage_model = glm::translate(cage_model, {0, 0.5, 0});
    s_lit->SetMat4("Model", cage_model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_white);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    s_lit->SetInt("texture_diffuse0", 0);
    s_lit->SetInt("texture_specular0", 1);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    s_lit->SetVec4("base_color", {0.5, 0.5, 0.9, 1.0});
    m_cage_faces[3]->Draw();
    glStencilFunc(GL_ALWAYS, 2, 0xFF);
    s_lit->SetVec4("base_color", {0.5, 0.9, 0.5, 1.0});
    m_cage_faces[1]->Draw();
    glStencilFunc(GL_ALWAYS, 3, 0xFF);
    s_lit->SetVec4("base_color", {0.9, 0.9, 0.5, 1.0});
    m_cage_faces[0]->Draw();
    s_lit->SetVec4("base_color", {0.3, 0.3, 0.8, 1.0});
    m_cage_faces[2]->Draw();
    m_cage_faces[4]->Draw();
    m_cage_faces[5]->Draw();
    // HINT: only write stencil buffer when draw cage
    glad_glStencilMask(0x00);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    // glClear(GL_DEPTH_BUFFER_BIT);
#pragma endregion

#pragma region render cubes
    // glStencilFunc(GL_EQUAL, 2, 0xFF);
    s_lit->Use();
    s_lit->SetVec4("base_color", glm::vec4{1});
    // you should do scale and rotation at origin!
    auto cube_model = glm::mat4(1);
    cube_model = glm::translate(cube_model, {-8.0, 0, -10.0});
    cube_model = glm::rotate(cube_model, glm::radians(30.0f), {0, 1, 0});
    cube_model = glm::scale(cube_model, {5, 5, 5});
    // cube_model = glm::translate(cube_model, {0, 0.5, 0});
    s_lit->SetMat4("Model", cube_model);
    s_lit->SetMat4("View", camera.GetViewMatrix());
    s_lit->SetMat4("Proj", camera.GetProjectionMatrix());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_ground_diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, t_ground_specular);
    s_lit->SetInt("texture_diffuse0", 0);
    s_lit->SetInt("texture_specular0", 1);
    m_box.Draw();

    cube_model = glm::mat4(1);
    cube_model = glm::translate(cube_model, {0, 45.0, 0});
    cube_model = glm::rotate(cube_model, glm::radians(45.0f), {0, 1, 0});
    cube_model = glm::scale(cube_model, glm::vec3{2});
    // cube_model = glm::translate(cube_model, {0, 0.5, 0});
    s_lit->SetMat4("Model", cube_model);
    s_lit->SetMat4("View", camera.GetViewMatrix());
    s_lit->SetMat4("Proj", camera.GetProjectionMatrix());

    m_box.Draw();
#pragma endregion

#pragma region render herta
    // s_lit->Use();
    // s_lit->SetMat4("Model", glm::scale(glm::translate(glm::mat4(1), glm::vec3(-3, 0, 0)), glm::vec3(0.4)));
    // s_lit->SetMat4("View", camera.GetViewMatrix());
    // s_lit->SetMat4("Proj", camera.GetProjectionMatrix());
    // m_herta.Draw();
    // glStencilFunc(GL_ALWAYS, 1, 0xFF);
    // glDepthFunc(GL_NEVER + gl_depth_func);
    // // HINT: or else can't clear stencil buffer bit
    // glad_glStencilMask(0xFF);
#pragma endregion

#pragma region render skybox
    glDepthMask(GL_FALSE);
    glBindVertexArray(skyboxVAO);
    glm::mat4 skybox_view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    s_skybox->Use();
    s_skybox->SetMat4("View", skybox_view);
    s_skybox->SetMat4("Proj", camera.GetProjectionMatrix());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, t_skybox);
    s_skybox->SetInt("skybox", 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
#pragma endregion

#pragma region render transparent window
    // s_lit->Use();
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, t_transparent_window);
    // s_lit->SetInt("texture_diffuse0", 0);
    // // HINT: no specular texture
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, 0);
    // s_lit->SetInt("texture_specular0", 1);

    // std::sort(window_positions.begin(), window_positions.end(), [&](auto &&lhs, auto &&rhs) {
    //   auto cam_pos = camera.GetPosition();
    //   return glm::distance(cam_pos, lhs) > glm::distance(cam_pos, rhs);
    // });

    // for (auto &&window_position : window_positions) {
    //   auto window_model = glm::mat4(1);
    //   // you should do scale and rotation at origin!
    //   window_model = glm::translate(window_model, window_position);
    //   window_model = glm::scale(window_model, {1, 10, 10});
    //   // move to origin then scale
    //   window_model = glm::translate(window_model, {0, 0.5, 0});
    //   s_lit->SetMat4("Model", window_model);
    //   s_lit->SetMat4("View", camera.GetViewMatrix());
    //   s_lit->SetMat4("Proj", camera.GetProjectionMatrix());
    //   m_window.Draw();
    // }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion
  };

  // HINT: Render loop start
  while (!glfwWindowShouldClose(window)) {
#pragma region render imgui
    {
      ImGui::NewFrame();
      ImGui::SetNextWindowPos({0, 0});
      ImGui::SetNextWindowSize(
          {static_cast<float>(global_context.imgui_width_), static_cast<float>(global_context.window_height_)});
      ImGui::Begin("Hello, World!", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
      ImGui::SetWindowFontScale(1.2);

      ImGui::ColorEdit4("BG_color", reinterpret_cast<float *>(&background_color), ImGuiColorEditFlags_AlphaPreview);

      if (ImGui::TreeNodeEx("Rotating light", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::ColorEdit3("p_light1", reinterpret_cast<float *>(&p_lights[0].color));
        ImGui::Separator();
        ImGui::ColorEdit3("p_light2", reinterpret_cast<float *>(&p_lights[1].color));
        ImGui::DragFloat3("rotation_axis", reinterpret_cast<float *>(&rotation_axis), 0.005f, -1.0f, 1.0f);
        ImGui::Separator();
        ImGui::ColorEdit3("d_light1", reinterpret_cast<float *>(&d_lights[0].color));
        ImGui::PushItemWidth(80);
        ImGui::SliderFloat("X", &d_lights[0].direction.x, -1.0f, 1.0f);
        ImGui::SameLine();
        ImGui::SliderFloat("Y", &d_lights[0].direction.y, -1.0f, 1.0f, "%.3f", ImGuiSliderFlags_None);
        ImGui::SameLine();
        ImGui::SliderFloat("Z", &d_lights[0].direction.z, -1.0f, 1.0f);
        ImGui::PopItemWidth();
        ImGui::TreePop();
      }
      ImGui::SliderFloat("Camera speed", &global_context.camera_->speed_, 5.0f, 30.0f);
      ImGui::SliderFloat("rotation_degree", &rotation_degree, 0, 360);

      if (ImGui::TreeNodeEx("Depth test", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Combo("Debug Mode", &shader_debug_mode, "NODEBUG\0NORMAL\0DEPTH\0");
        ImGui::Combo("Depth test", &gl_depth_func,
                     "GL_NEVER\0GL_LESS\0GL_EQUAL\0GL_LEQUAL\0GL_GREATER\0GL_NOTEQUAL\0GL_GEQUAL\0GL_ALWAYS\0");
        ImGui::TreePop();
      }

      if (ImGui::TreeNodeEx("Face culling", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Face culling enable", &face_culling_enable);
        ImGui::SameLine();
        ImGui::Checkbox("Debug sky map", &debug_sky_map);
        ImGui::Combo("Face culling", &culled_face,
                     "GL_FRONT_LEFT\0GL_FRONT_RIGHT\0GL_BACK_LEFT\0GL_BACK_RIGHT\0GL_FRONT\0GL_BACK\0GL_LEFT\0GL_"
                     "RIGHT\0GL_FRONT_AND_BACK\0");
        ImGui::TreePop();
      }

      if (ImGui::CollapsingHeader("Section 2", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::TreeNodeEx("Cursor", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Text("yaw: %f\npitch: %f", global_context.camera_->GetYaw(), global_context.camera_->GetPitch());
          ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Cursor", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Text("xoffset: %f\nyoffset: %f", xac::InputSystem::cursor_x_offset_,
                      xac::InputSystem::cursor_y_offset_);
          ImVec2 mouse_position_absolute = ImGui::GetMousePos();
          ImGui::Text("Position: %f, %f", mouse_position_absolute.x, mouse_position_absolute.y);
          ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Command", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Text("FORWARD: %d, BACKWARD: %d", xac::InCommand(xac::ControlCommand::FORWARD),
                      xac::InCommand(xac::ControlCommand::BACKWARD));
          ImGui::Text("LEFT: %4d, RIGHT: %4d", xac::InCommand(xac::ControlCommand::LEFT),
                      xac::InCommand(xac::ControlCommand::RIGHT));
          ImGui::Text("DOWN: %4d, UP: %7d", xac::InCommand(xac::ControlCommand::DOWN),
                      xac::InCommand(xac::ControlCommand::UP));
          ImGui::TreePop();
        }
        ImGui::Text("Frame rate: %f", 1.0 / delta_time_per_frame);
      }
      ImGui::End();
      ImGui::Render();
    }
#pragma endregion

    check_gl_depth_func();
    check_face_culling_enable();
    check_culled_face();
    check_shader_debug_mode();

    static float last_frame_time = 0.0f;
    auto cur_frame_time = static_cast<float>(glfwGetTime());
    delta_time_per_frame = cur_frame_time - last_frame_time;
    last_frame_time = cur_frame_time;

    global_context.camera_->Tick(delta_time_per_frame);
    xac::InputSystem::Tick();

    glViewport(0, 0, 1024, 1024);
    xac::Camera camera;
    camera.SetFov(glm::radians(90.0));
    camera.SetAspect(1);
    camera.SetPosition({0, 31.5, 0});

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (debug_sky_map) {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    // right
    if (debug_sky_map) {
      glViewport(940, 300, 300, 300);
    } else {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo_dem[0]);
    }
    camera.SetYaw(180);
    camera.SetPitch(0);
    DrawScene(delta_time_per_frame, camera);

    // left
    if (debug_sky_map) {
      glViewport(340, 300, 300, 300);
    } else {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo_dem[1]);
    }
    camera.SetYaw(0);
    camera.SetPitch(0);
    DrawScene(delta_time_per_frame, camera);

    // top
    if (debug_sky_map) {
      glViewport(640, 0, 300, 300);  // show at bottom
    } else {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo_dem[2]);
    }
    camera.SetYaw(180);
    camera.SetPitch(-90);
    DrawScene(delta_time_per_frame, camera);

    // bottom
    if (debug_sky_map) {
      glViewport(640, 600, 300, 300);  // show at top
    } else {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo_dem[3]);
    }
    camera.SetYaw(180);
    camera.SetPitch(90);
    DrawScene(delta_time_per_frame, camera);

    // front
    if (debug_sky_map) {
      glViewport(640, 300, 300, 300);
    } else {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo_dem[4]);
    }
    camera.SetYaw(90);
    camera.SetPitch(0);
    DrawScene(delta_time_per_frame, camera);

    // back
    if (debug_sky_map) {
      glViewport(1240, 300, 300, 300);
    } else {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo_dem[5]);
    }
    camera.SetYaw(-90);
    camera.SetPitch(0);
    DrawScene(delta_time_per_frame, camera);

    // glBindFramebuffer(GL_FRAMEBUFFER, fbo_screen);
    // HINT: render starts from the top left of the whole window, not setting this will lead to black at left
    // glViewport(0, 0, global_context.window_width_ - global_context.imgui_width_, global_context.window_height_);

    if (!debug_sky_map) {
      glViewport(global_context.imgui_width_, 0, global_context.window_width_ - global_context.imgui_width_,
                 global_context.window_height_);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      DrawScene(delta_time_per_frame, *global_context.camera_);

#pragma region render cube
      s_reflect->Use();
      s_reflect->SetMat4("Model", glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 30, 0)), glm::vec3{4.0f}));
      s_reflect->SetMat4("View", global_context.camera_->GetViewMatrix());
      s_reflect->SetMat4("Proj", global_context.camera_->GetProjectionMatrix());
      s_reflect->SetVec3("ws_cam_pos", global_context.camera_->GetPosition());
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, t_fb_cube_map);
      s_reflect->SetInt("skybox", 0);
      m_cube.Draw();
#pragma endregion
    }

    // #pragma region render to screen quad
    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //     glClearColor(0.5f, 1.0f, 1.0f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //     glViewport(global_context.imgui_width_, 0, global_context.window_width_ - global_context.imgui_width_,
    //                global_context.window_height_);

    //     glDisable(GL_DEPTH_TEST);
    //     s_unlit->Use();
    //     s_unlit->SetMat4("Model", glm::mat4{1.0f});
    //     s_unlit->SetMat4("View", glm::mat4{1.0f});
    //     s_unlit->SetMat4("Proj", glm::mat4{1.0f});
    //     s_unlit->SetVec4("color", glm::vec4{1.0f});
    //     glActiveTexture(GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, t_fb_screen_color);
    //     s_unlit->SetInt("tex", 0);
    //     m_quad.Draw();
    //     glEnable(GL_DEPTH_TEST);
    // #pragma endregion

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glDeleteFramebuffers(1, &fbo_screen);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
