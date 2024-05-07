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

set (CMAKE_BUILD_TYPE Debug)
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
else()
  set (CMAKE_BUILD_TYPE Debug)
endif()

include(FetchContent)
set(EXECUTABLE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
if (MSVC)
  # set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MDd /fsanitize=address")
  # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /fsanitize=address")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # fix LNK2038: mismatch detected for 'RuntimeLibrary' bug
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

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
                     GIT_TAG        10.1.1
)

FetchContent_Declare(imgui
                     GIT_REPOSITORY https://github.com/ocornut/imgui
                     GIT_TAG        v1.89.9-docking
)

option(SHADERC_SKIP_TESTS "" ON)
option(SHADERC_SKIP_EXAMPLES "" ON)
option(SHADERC_ENABLE_TESTS "" OFF)

FetchContent_Declare(shaderc
                     GIT_REPOSITORY https://github.com/google/shaderc.git
                     GIT_TAG        v2023.8
)

FetchContent_Declare(spirv-cross
                     GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
                     GIT_TAG        sdk-1.3.261.1
)

FetchContent_Declare(pico_libs
                     GIT_REPOSITORY https://github.com/Katzeee/pico_libs
                     GIT_TAG        v0.0.1
)

FetchContent_MakeAvailable(glfw assimp fmt glad pico_libs spirv-cross)
find_package(Vulkan)

include(3rdparty/imgui.cmake)
include(3rdparty/glm.cmake)
include(3rdparty/shaderc.cmake)

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
add_subdirectory(21-blinn-phong)
add_subdirectory(22-shadow)
add_subdirectory(23-pbr)
add_subdirectory(24-ibl)
if (${Vulkan_LIBRARIES})
  add_subdirectory(25-vulkan-start)
endif()