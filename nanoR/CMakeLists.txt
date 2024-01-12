include_directories(./src)
file(
  GLOB_RECURSE 
  src 
  COFIGURE_DEPENDS 
  src/*.cpp
  sandbox.cpp
)

add_executable(nanoR ${src})
target_include_directories(nanoR PRIVATE ${shaderc_SOURCE_DIR})
target_link_libraries(nanoR PRIVATE glfw glad assimp::assimp imgui fmt::fmt pico_libs::ecs shaderc shaderc_util glslc)
target_precompile_headers(nanoR PRIVATE src/nanorpch.h)