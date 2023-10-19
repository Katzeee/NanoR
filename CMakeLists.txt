cmake_minimum_required(VERSION 3.22)
project(learn-opengl C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") # works

IF (WIN32)
# add_definitions(-DWIN32)
	MESSAGE(STATUS "Now is windows")
ELSEIF (APPLE)
	MESSAGE(STATUS "Now is Apple systens.")
ELSEIF (UNIX)
	MESSAGE(STATUS "Now is UNIX-like OS's.")
ENDIF ()

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
else()
  set (CMAKE_BUILD_TYPE Debug)
endif()

include(FetchContent)
# set(EXECUTABLE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)

include_directories(3rdparty)

find_package(glfw3 REQUIRED)

find_package(imgui CONFIG)
if (NOT TARGET imgui)
  include(3rdparty/imgui.cmake)
endif()

find_package(glad CONFIG)
if (NOT TARGET glad::glad)
  MESSAGE(STATUS "Cannot find glad, try compile from source.")
  file(GLOB glad_src CONFIGURE_DEPENDS 3rdparty/glad/glad.c)
  add_library(glad OBJECT ${glad_src})
  target_include_directories(glad PUBLIC 3rdparty/glad)
  add_library(glad::glad ALIAS glad)
endif()

find_package(glm REQUIRED)

FetchContent_Declare(assimp
                     GIT_REPOSITORY https://github.com/assimp/assimp.git
                     GIT_TAG        v5.2.5 
)
FetchContent_MakeAvailable(assimp)

FetchContent_Declare(fmt
                     GIT_REPOSITORY https://github.com/fmtlib/fmt
                     GIT_TAG        10.1.0
)
FetchContent_MakeAvailable(fmt)

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

##################### demo
add_executable(imgui_demo 3rdparty/imgui/imgui_demo.cpp)
target_link_libraries(imgui_demo PRIVATE glfw glad::glad imgui)
