include_directories(./src)
file(GLOB_RECURSE src COFIGURE_DEPENDS *.cpp)
# aux_source_directory(./src src)

add_executable(nanoR ${src})
target_link_libraries(nanoR PRIVATE glfw glad assimp::assimp imgui fmt::fmt)
target_precompile_headers(nanoR PRIVATE src/nanorpch.h)
