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

unsigned int indexCount;
static auto GenSphereMesh() -> unsigned int {
  unsigned int sphereVAO = 0;
  glGenVertexArrays(1, &sphereVAO);

  unsigned int vbo, ebo;
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> uv;
  std::vector<unsigned int> indices;

  const unsigned int X_SEGMENTS = 64;
  const unsigned int Y_SEGMENTS = 64;
  const float PI = 3.14159265359f;
  for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
      float xSegment = (float)x / (float)X_SEGMENTS;
      float ySegment = (float)y / (float)Y_SEGMENTS;
      float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
      float yPos = std::cos(ySegment * PI);
      float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

      positions.push_back(glm::vec3(xPos, yPos, zPos));
      normals.push_back(glm::vec3(xPos, yPos, zPos));
      uv.push_back(glm::vec2(xSegment, ySegment));
    }
  }

  bool oddRow = false;
  for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
    if (!oddRow)  // even rows: y == 0, y == 2; and so on
    {
      for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
        indices.push_back(y * (X_SEGMENTS + 1) + x);
        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
      }
    } else {
      for (int x = X_SEGMENTS; x >= 0; --x) {
        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        indices.push_back(y * (X_SEGMENTS + 1) + x);
      }
    }
    oddRow = !oddRow;
  }
  indexCount = static_cast<unsigned int>(indices.size());

  std::vector<float> data;
  for (unsigned int i = 0; i < positions.size(); ++i) {
    data.push_back(positions[i].x);
    data.push_back(positions[i].y);
    data.push_back(positions[i].z);
    if (normals.size() > 0) {
      data.push_back(normals[i].x);
      data.push_back(normals[i].y);
      data.push_back(normals[i].z);
    }
    if (uv.size() > 0) {
      data.push_back(uv[i].x);
      data.push_back(uv[i].y);
    }
  }
  glBindVertexArray(sphereVAO);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
  unsigned int stride = (3 + 3 + 2) * sizeof(float);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));

  return sphereVAO;
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
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#pragma region mesh data
  // clang-format off
  std::vector<unsigned int> ground_indices {
    0, 1, 2, 2, 3, 0,
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
    {{-1.0f,  1.0f, 1.0f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}}, 
    {{ 1.0f,  1.0f, 1.0f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, 
    {{ 1.0f, -1.0f, 1.0f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}}, 
    {{-1.0f, -1.0f, 1.0f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, 
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
  // xac::Model m_herta("../resources/models/Herta/heita.obj");
  xac::Model m_sphere("../resources/models/Sphere/sphere.obj");
  xac::Model m_cube("../resources/models/Cube/cube.obj");
  xac::Mesh m_ground{ground_vertices, ground_indices, {glm::vec3{0}, glm::vec3{0.5}, glm::vec3{0.5}, {}}, "ground"};
  xac::Mesh m_quad{quad_vertices, {0, 2, 1, 0, 3, 2}, {}, "quad"};
  xac::Model m_box = m_cube;
  xac::Model m_skybox = m_cube;
  xac::Model m_light = m_sphere;

  auto s_unlit = std::make_shared<xac::Shader>("../24-ibl/shader/common.vert.glsl", "../24-ibl/shader/unlit.frag.glsl");
  auto s_lit = std::make_shared<xac::Shader>("../24-ibl/shader/common.vert.glsl", "../24-ibl/shader/lit.frag.glsl");
  auto s_pbr = std::make_shared<xac::Shader>("../24-ibl/shader/common.vert.glsl", "../24-ibl/shader/pbr.frag.glsl");
  s_pbr->AddDefine("vert", "MODEL_NORMAL");
  s_pbr->CompileShaders();
  auto s_equirectangle_to_cube = std::make_shared<xac::Shader>(
      "../24-ibl/shader/common.vert.glsl", "../24-ibl/shader/equirectangle_to_cubemap.frag.glsl"
  );
  s_equirectangle_to_cube->AddDefine("vert", "LOCAL_POS");
  s_equirectangle_to_cube->CompileShaders();
  auto s_skybox =
      std::make_shared<xac::Shader>("../24-ibl/shader/skybox.vert.glsl", "../24-ibl/shader/skybox.frag.glsl");
  auto s_ibl_convolution =
      std::make_shared<xac::Shader>("../24-ibl/shader/skybox.vert.glsl", "../24-ibl/shader/ibl_diffuse.frag.glsl");
  auto s_ibl_specular_prefilter = std::make_shared<xac::Shader>(
      "../24-ibl/shader/skybox.vert.glsl", "../24-ibl/shader/ibl_specular_prefilter.frag.glsl"
  );
  auto s_ibl_specular_lut =
      std::make_shared<xac::Shader>("../24-ibl/shader/common.vert.glsl", "../24-ibl/shader/ibl_specular_lut.frag.glsl");
  auto t_box_diffuse = xac::LoadTextureFromFile("../resources/textures/container2.png");
  auto t_box_specular = xac::LoadTextureFromFile("../resources/textures/container2_specular.png");
  auto t_ground_diffuse = xac::LoadTextureFromFile("../resources/textures/wood.png");
  auto t_white = xac::LoadTextureFromFile("../resources/textures/white.png");
  auto t_ibl_hdr = xac::LoadHdrTextureFromFile("../resources/textures/kart_club_4k.hdr");
  auto t_skybox = xac::LoadCubemapFromFile({
      "../resources/textures/skybox/right.jpg",
      "../resources/textures/skybox/left.jpg",
      "../resources/textures/skybox/top.jpg",
      "../resources/textures/skybox/bottom.jpg",
      "../resources/textures/skybox/front.jpg",
      "../resources/textures/skybox/back.jpg",
  });

  m_light.SetShader(s_unlit);
  m_ground.SetShader(s_lit);
  m_box.SetShader(s_equirectangle_to_cube);
  m_skybox.SetShader(s_skybox);
  m_quad.SetShader(s_unlit);

  auto vao_sphere = GenSphereMesh();
#pragma endregion
  glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  glm::mat4 captureViews[] = {
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

#pragma region Render HDR to cubemap
  unsigned int fbo_capture, rbo_captuer;
  glCreateFramebuffers(1, &fbo_capture);
  glCreateRenderbuffers(1, &rbo_captuer);
  glNamedRenderbufferStorage(rbo_captuer, GL_DEPTH_COMPONENT24, 1024, 1024);
  glNamedFramebufferRenderbuffer(fbo_capture, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_captuer);
  unsigned int t_ibl_cubemap;
  glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &t_ibl_cubemap);
  glTextureStorage2D(t_ibl_cubemap, 1, GL_RGB16F, 1024, 1024);
  glTextureParameteri(t_ibl_cubemap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(t_ibl_cubemap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureParameteri(t_ibl_cubemap, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTextureParameteri(t_ibl_cubemap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(t_ibl_cubemap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  auto convert_cubemap = [&]() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_capture);
    s_equirectangle_to_cube->Use();
    glNamedBufferSubData(ubo, sizeof(glm::mat4), sizeof(glm::mat4), &captureProjection);
    glViewport(0, 0, 1024, 1024);
    auto model = glm::mat4{1};
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_ibl_hdr);
    s_equirectangle_to_cube->SetMat4("Model", model);
    s_equirectangle_to_cube->SetInt("tex", 0);
    assert(glGetError() == GL_NO_ERROR);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < 6; i++) {
      glNamedBufferSubData(ubo, 0, sizeof(glm::mat4), &captureViews[i]);
      glNamedFramebufferTextureLayer(fbo_capture, GL_COLOR_ATTACHMENT0, t_ibl_cubemap, 0, i);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      m_box.Draw();
      assert(glGetError() == GL_NO_ERROR);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(
        global_context.imgui_width_, 0, global_context.window_width_ - global_context.imgui_width_,
        global_context.window_height_
    );
  };
  convert_cubemap();
#pragma endregion

#pragma region ibl diffuse cubemap convolution
  unsigned int fbo_ibl_diffuse;
  glCreateFramebuffers(1, &fbo_ibl_diffuse);
  unsigned int t_ibl_diffuse;
  glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &t_ibl_diffuse);
  glTextureStorage2D(t_ibl_diffuse, 1, GL_RGB16F, 32, 32);
  glTextureParameteri(t_ibl_diffuse, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(t_ibl_diffuse, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureParameteri(t_ibl_diffuse, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTextureParameteri(t_ibl_diffuse, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(t_ibl_diffuse, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  auto convolution_cubemap = [&]() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_ibl_diffuse);
    glViewport(0, 0, 32, 32);
    s_ibl_convolution->Use();
    // auto model = glm::mat4{1};
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, t_ibl_cubemap);
    s_ibl_convolution->SetInt("tex", 0);
    s_ibl_convolution->SetMat4("Proj", captureProjection);
    for (int i = 0; i < 6; i++) {
      s_ibl_convolution->SetMat4("View", captureViews[i]);
      glNamedFramebufferTextureLayer(fbo_ibl_diffuse, GL_COLOR_ATTACHMENT0, t_ibl_diffuse, 0, i);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      m_box.SetShader(s_ibl_convolution);
      m_box.Draw();
      assert(glGetError() == GL_NO_ERROR);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(
        global_context.imgui_width_, 0, global_context.window_width_ - global_context.imgui_width_,
        global_context.window_height_
    );
  };
  convolution_cubemap();
#pragma endregion

#pragma region ibl specular prefilter
  unsigned int fbo_ibl_specular_prefilter, t_ibl_specular_prefilter;
  glCreateFramebuffers(1, &fbo_ibl_specular_prefilter);
  glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &t_ibl_specular_prefilter);
  glTextureStorage2D(t_ibl_specular_prefilter, 5, GL_RGB16F, 128, 128);
  glTextureParameteri(t_ibl_specular_prefilter, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(t_ibl_specular_prefilter, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureParameteri(t_ibl_specular_prefilter, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTextureParameteri(t_ibl_specular_prefilter, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTextureParameteri(t_ibl_specular_prefilter, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glGenerateTextureMipmap(t_ibl_specular_prefilter);
  assert(glGetError() == GL_NO_ERROR);
  auto prefilter_map = [&]() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_ibl_specular_prefilter);
    // glViewport(0, 0, 128, 128);
    s_ibl_specular_prefilter->Use();
    // auto model = glm::mat4{1};
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, t_ibl_cubemap);
    s_ibl_specular_prefilter->SetInt("tex", 0);
    s_ibl_specular_prefilter->SetMat4("Proj", captureProjection);
    assert(glGetError() == GL_NO_ERROR);
    for (int mip = 0; mip < 5; mip++) {
      unsigned int mip_width = 128 * std::pow(0.5, mip), mip_height = 128 * std::pow(0.5, mip);
      glViewport(0, 0, mip_width, mip_height);
      float roughness = (float)mip / (float)5;
      roughness += 0.1;
      s_ibl_specular_prefilter->SetFloat("roughness", roughness);
      assert(glGetError() == GL_NO_ERROR);
      for (int i = 0; i < 6; i++) {
        s_ibl_specular_prefilter->SetMat4("View", captureViews[i]);
        glNamedFramebufferTextureLayer(
            fbo_ibl_specular_prefilter, GL_COLOR_ATTACHMENT0, t_ibl_specular_prefilter, mip, i
        );
        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        assert(fboStatus == GL_FRAMEBUFFER_COMPLETE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_box.SetShader(s_ibl_specular_prefilter);
        m_box.Draw();
        assert(glGetError() == GL_NO_ERROR);
      }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(
        global_context.imgui_width_, 0, global_context.window_width_ - global_context.imgui_width_,
        global_context.window_height_
    );
  };
  prefilter_map();
#pragma endregion

#pragma region ibl specular lut
  unsigned int fbo_ibl_specular_lut, t_ibl_specular_lut;
  glCreateFramebuffers(1, &fbo_ibl_specular_lut);
  glCreateTextures(GL_TEXTURE_2D, 1, &t_ibl_specular_lut);
  glTextureStorage2D(t_ibl_specular_lut, 1, GL_RGB16F, 512, 512);
  glTextureParameteri(t_ibl_specular_lut, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(t_ibl_specular_lut, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureParameteri(t_ibl_specular_lut, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(t_ibl_specular_lut, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glNamedFramebufferTexture(fbo_ibl_specular_lut, GL_COLOR_ATTACHMENT0, t_ibl_specular_lut, 0);
  auto ibl_specular_map = [&]() {
    glNamedBufferSubData(ubo, 0, sizeof(glm::mat4), &captureViews[4]);
    glNamedBufferSubData(ubo, sizeof(glm::mat4), sizeof(glm::mat4), &captureProjection);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_ibl_specular_lut);
    glViewport(0, 0, 512, 512);
    s_ibl_specular_lut->Use();
    s_ibl_specular_lut->SetMat4("Model", glm::mat4{1});
    m_quad.SetShader(s_ibl_specular_lut);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_quad.Draw();
    assert(glGetError() == GL_NO_ERROR);
    glViewport(
        global_context.imgui_width_, 0, global_context.window_width_ - global_context.imgui_width_,
        global_context.window_height_
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  };
  ibl_specular_map();
#pragma endregion

#pragma region imgui variables
  struct PointLight {
    glm::vec3 color = glm::vec3{1};
    float intensity = 300;
  };
  struct DirectLight {
    glm::vec4 color = glm::vec4{1};
    float intensity = 1;
    glm::vec3 direction = glm::vec3{1, 1, 1};
  };
  glm::vec3 p_light_positions[] = {
      glm::vec3(-10.0f, 10.0f, 10.0f),
      glm::vec3(10.0f, 10.0f, 10.0f),
      glm::vec3(-10.0f, -10.0f, 10.0f),
      glm::vec3(10.0f, -10.0f, 10.0f),
  };

  glm::vec4 background_color{};
  PointLight p_lights[4];
  DirectLight d_lights[1];
  glm::vec3 rotation_axis{1, 1, 1};
  float rotation_degree{60};

  global_context.imgui_layer_->RegisterWatchVar<int>("gl_depth_func", GL_LEQUAL - GL_NEVER, [](int new_val) {
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
  global_context.imgui_layer_->RegisterWatchVar<bool>("cull_face_enable", false, [](bool new_val) {
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

#pragma region convert hdr to cubemap

#pragma endregion

  auto DrawScene = [&](float delta_time, xac::Camera &camera) {
    glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 proj = camera.GetProjectionMatrix();
    glNamedBufferSubData(ubo, 0, sizeof(glm::mat4), &view);
    glNamedBufferSubData(ubo, sizeof(glm::mat4), sizeof(glm::mat4), &proj);
#pragma region render cubemap
    s_skybox->Use();
    s_skybox->SetMat4("View", glm::mat4(glm::mat3(camera.GetViewMatrix())));
    s_skybox->SetMat4("Proj", camera.GetProjectionMatrix());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, t_ibl_cubemap);
    s_skybox->SetInt("skybox", 0);
    glDisable(GL_CULL_FACE);
    m_skybox.Draw();
    glEnable(GL_CULL_FACE);
#pragma endregion

#pragma region render light
    s_unlit->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_white);
    s_unlit->SetInt("tex", 0);

    for (int i = 0; i < 4; i++) {
      glm::mat4 model{1.0f};
      glm::vec3 position = p_light_positions[i];
      // Because you do the transformation by the order scale->rotate->translate,
      // glm functions are doing right multiply, so
      // the model matrix should reverse it, that is translate->rotate->scale
      model = glm::translate(model, position);
      model = glm::scale(model, glm::vec3(0.2f));
      s_unlit->SetMat4("Model", model);
      s_unlit->SetVec3("color", p_lights[i].color);
      m_light.Draw();
    }

    s_unlit->Use();
    auto d_light_model = glm::scale(glm::translate(glm::mat4{1}, d_light0_pos), glm::vec3{3.0f});
    s_unlit->SetMat4("Model", d_light_model);
    s_unlit->SetVec3("color", d_lights[0].color);
    m_light.Draw();
#pragma endregion

#pragma region common settings for obj shader
    s_lit->Use();
    for (int i = 0; i < 4; i++) {
      s_lit->SetVec3("p_lights[" + std::to_string(i) + "].ws_position", p_light_positions[i]);
      s_lit->SetVec3("p_lights[" + std::to_string(i) + "].color", p_lights[i].color);
      s_lit->SetFloat("p_lights[" + std::to_string(i) + "].intensity", p_lights[i].intensity);
    }

    s_lit->SetVec3("d_lights[0].direction", d_lights[0].direction);
    s_lit->SetVec3("d_lights[0].color", d_lights[0].color);
    s_lit->SetFloat("d_lights[0].intensity", d_lights[0].intensity);
    s_lit->SetVec3("ws_cam_pos", camera.GetPosition());

    s_pbr->Use();
    for (int i = 0; i < 4; i++) {
      s_pbr->SetVec3("p_lights[" + std::to_string(i) + "].ws_position", p_light_positions[i]);
      s_pbr->SetVec3("p_lights[" + std::to_string(i) + "].color", p_lights[i].color);
      s_pbr->SetFloat("p_lights[" + std::to_string(i) + "].intensity", p_lights[i].intensity);
    }
    s_pbr->SetVec3("ws_cam_pos", camera.GetPosition());

#pragma endregion

#pragma region render spheres
    s_pbr->Use();
    s_pbr->SetVec3("albedo", glm::vec3{1});

    glBindVertexArray(vao_sphere);
    int nrRows = 7, nrColumns = 7;
    float spacing = 2.5;
    glm::mat4 model = glm::mat4(1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, t_ibl_diffuse);
    s_pbr->SetInt("ibl_diffuse", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, t_ibl_specular_prefilter);
    s_pbr->SetInt("ibl_specular_prefilter", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, t_ibl_specular_lut);
    s_pbr->SetInt("ibl_specular_lut", 2);
    for (int row = 0; row < nrRows; ++row) {
      s_pbr->SetFloat("metallic", (float)row / (float)nrRows);
      s_pbr->SetFloat("ao", 1.0);
      for (int col = 0; col < nrColumns; ++col) {
        // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
        // on direct lighting.
        s_pbr->SetFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

        model = glm::mat4(1.0f);
        model =
            glm::translate(model, glm::vec3((col - (nrColumns / 2)) * spacing, (row - (nrRows / 2)) * spacing, 0.0f));
        s_pbr->SetMat4("Model", model);
        s_pbr->SetMat3("normal_model_to_world", glm::transpose(glm::inverse(glm::mat3(model))));
        glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
      }
    }
#pragma endregion
  };

  global_context.camera_->SetFar(50);
  auto debug_frustum = global_context.camera_->GetFrustumInWorld();
  global_context.camera_->SetFar(150);

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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(
        global_context.imgui_width_, 0, global_context.window_width_ - global_context.imgui_width_,
        global_context.window_height_
    );

    DrawScene(delta_time_per_frame, *global_context.camera_);
    // convert_cubemap();

    global_context.imgui_layer_->Render();

    global_context.window_->SwapBuffers();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  return 0;
}
