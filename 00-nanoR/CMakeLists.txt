include_directories(./src)
file(
  GLOB_RECURSE 
  src 
  COFIGURE_DEPENDS 
  src/*.cpp
  sandbox.cpp
)

MESSAGE(STATUS ${CMAKE_C_FLAGS_DEBUG})
add_executable(nanoR ${src})
target_include_directories(nanoR PRIVATE ${shaderc_SOURCE_DIR})
target_link_libraries(nanoR PRIVATE glfw glad assimp::assimp imgui fmt::fmt pico_libs::ecs shaderc shaderc_util glslc spirv-cross-glsl)
target_precompile_headers(nanoR PRIVATE src/nanorpch.h)