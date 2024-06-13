#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include <assimp/Importer.hpp>
#include <chrono>
#include <cstdint>
#include <deque>
#include <fstream>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>


#include "../3rdparty/stb_image.h"
#include "logger/logger.h"