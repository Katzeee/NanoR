#include <fstream>
#include <iostream>
#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>
#include <sstream>

inline auto ReadTextFromFile(std::string_view file_path) -> std::string {
  std::ifstream fs;
  std::stringstream ss;
  fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    fs.open(file_path.data());
    ss << fs.rdbuf();
    fs.close();
  } catch (std::exception &e) {
    std::cerr << "ERROR::SHADER::READ_FAILED:\n\t" << e.what() << file_path << std::endl;
    exit(3);
  }
  return ss.str();
}

inline auto CompileShader(std::string_view path, int type) {
  // SECTION: load src
  auto src = ReadTextFromFile(path);

  // SECTION: compile to spirv
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;
  // shaderc_util::FileFinder fileFinder;
  // options.SetIncluder(std::make_unique<glslc::FileIncluder>(&fileFinder));
  options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
  const bool optimize = false;  // for debug info
  if (optimize) {
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
  }
  // TODO: support other kind of shaders
  shaderc_shader_kind kind;
  switch (type) {
    case 1:
      kind = shaderc_shader_kind::shaderc_glsl_vertex_shader;
      break;
    case 2:
      kind = shaderc_shader_kind::shaderc_glsl_fragment_shader;
      break;
    default:
      break;
  }
  auto result = compiler.CompileGlslToSpv(src, kind, path.data(), options);
  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
    std::cerr << result.GetErrorMessage() << std::endl;
    exit(3);
  }
  std::vector<uint32_t> spirv_code = {result.cbegin(), result.cend()};

  return spirv_code;
}