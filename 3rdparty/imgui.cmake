MESSAGE(STATUS "Cannot find imgui, try compile from source.")

file(GLOB imgui_src CONFIGURE_DEPENDS ${imgui_src}
${imgui_SOURCE_DIR}/*.cpp 
${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp 
${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3*.cpp)

# get_filename_component(full_path_demo ${imgui_SOURCE_DIR}/imgui_demo.cpp ABSOLUTE)
# list(REMOVE_ITEM imgui_src ${full_path_demo})
add_library(imgui OBJECT ${imgui_src})
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
MESSAGE(STATUS "Imgui compile done.")