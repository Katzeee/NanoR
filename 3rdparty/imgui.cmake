MESSAGE(STATUS "Cannot find imgui, try compile from source.")
# include_directories(3rdparty/imgui)
set(imgui_src_dir 3rdparty/imgui)
file(GLOB imgui_src CONFIGURE_DEPENDS ${imgui_src}
${imgui_src_dir}/*.cpp 
${imgui_src_dir}/backends/imgui_impl_glfw.cpp 
${imgui_src_dir}/backends/imgui_impl_opengl3*.cpp)
get_filename_component(full_path_demo ${imgui_src_dir}/imgui_demo.cpp ABSOLUTE)
list(REMOVE_ITEM imgui_src ${full_path_demo})
add_library(imgui OBJECT ${imgui_src})
target_include_directories(imgui PUBLIC 3rdparty/imgui)
message(STATUS ${imgui_src})