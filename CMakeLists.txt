cmake_minimum_required(VERSION 3.22)
project(learn-opengl C CXX)
set(EXPORT_COMPILE_COMMANDS ON) # generate compile database

IF (WIN32)
# add_definitions(-DWIN32)
	MESSAGE(STATUS "Now on windows")
ELSEIF (APPLE)
	MESSAGE(STATUS "Now on Apple systens.")
ELSEIF (UNIX)
	MESSAGE(STATUS "Now on UNIX-like OS's.")
ENDIF ()

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
else()
  set (CMAKE_BUILD_TYPE Debug)
endif()

include(FetchContent)
set(EXECUTABLE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

include_directories(3rdparty)

set(GLAD_PROFILE "core")
FetchContent_Declare(glad
                     GIT_REPOSITORY https://github.com/Dav1dde/glad.git
                     GIT_TAG        v0.1.36
)

FetchContent_Declare(glm
                     GIT_REPOSITORY https://github.com/g-truc/glm
                     GIT_TAG        0.9.9.8
)

FetchContent_Declare(glfw
                     GIT_REPOSITORY https://github.com/glfw/glfw
                     GIT_TAG        3.3.8
)

FetchContent_Declare(assimp
                     GIT_REPOSITORY https://github.com/assimp/assimp.git
                     GIT_TAG        v5.2.5 
)

FetchContent_Declare(fmt
                     GIT_REPOSITORY https://github.com/fmtlib/fmt
                     GIT_TAG        10.1.0
)

FetchContent_Declare(imgui
                     GIT_REPOSITORY https://github.com/ocornut/imgui
                     GIT_TAG        v1.89.9-docking
)

FetchContent_MakeAvailable(glfw assimp fmt glad)
include(3rdparty/imgui.cmake)
include(3rdparty/glm.cmake)

add_subdirectory(nanoR)
add_subdirectory(01-create-window)
add_subdirectory(02-simple-triangle)
add_subdirectory(03-shader-class)
add_subdirectory(04-texture)
add_subdirectory(05-simple-cube)
add_subdirectory(06-camera)
add_subdirectory(07-lighting)
add_subdirectory(08-lighting-map)
add_subdirectory(09-multi-light)
add_subdirectory(10-mesh)
add_subdirectory(11-load-model)
add_subdirectory(12-imgui)
add_subdirectory(13-input-system)
add_subdirectory(14-depth-test)
add_subdirectory(15-stencil-test)
add_subdirectory(16-alpha-blend)
add_subdirectory(17-face-culling)
add_subdirectory(18-frame-buffer)
add_subdirectory(19-cube-map)
add_subdirectory(20-interface-block)

##################### demo
# add_executable(imgui_demo 3rdparty/imgui/imgui_demo.cpp)
# target_link_libraries(imgui_demo PRIVATE glfw glad::glad imgui)
