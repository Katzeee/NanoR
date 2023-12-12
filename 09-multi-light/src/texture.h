#pragma once
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <array>
#include <memory>
#include <stdexcept>

#include "stb_image.h"

#define STRCAT(X, Y) X##Y

namespace xac {
class TextureManager {
 public:
  ~TextureManager() = default;
  TextureManager(const TextureManager &) = delete;
  auto operator=(const TextureManager &) -> TextureManager & = delete;
  TextureManager(const TextureManager &&) = delete;
  auto operator=(const TextureManager &&) -> TextureManager & = delete;

  static auto GetInstance() -> TextureManager & {
    // make unique doesn't work, Why?
    static std::unique_ptr<TextureManager> instance_(new TextureManager());
    return *instance_;
  }

  auto ActiveTexture(unsigned int number) {
    glActiveTexture(GL_TEXTURE0 + number);
  }

  auto CreateTexture(
      const char *filename, unsigned int tex_type, int wrap_s, int wrap_t, int min_filter, int mag_filter,
      int internal_format, int format, int levels = 1, int border = 0
  ) -> unsigned int;

 private:
  TextureManager() = default;
  std::array<unsigned int, 32> textures_;
  unsigned int counter_ = 0;  // count the number of textures
};

auto TextureManager::CreateTexture(
    const char *filename, unsigned int tex_type, int wrap_s, int wrap_t, int min_filter, int mag_filter,
    int internal_format, int format, int levels, int border
) -> unsigned int {
  if (counter_ > 31) {
    throw std::logic_error("More than 32 textures");
  }
  int width, height, nr_channels;
  unsigned char *tex_data = stbi_load(filename, &width, &height, &nr_channels, 0);
  glGenTextures(1, &textures_[counter_]);
  ActiveTexture(counter_);
  glBindTexture(tex_type, textures_[counter_]);
  glTexParameteri(tex_type, GL_TEXTURE_WRAP_S, wrap_s);
  glTexParameteri(tex_type, GL_TEXTURE_WRAP_T, wrap_t);
  glTexParameteri(tex_type, GL_TEXTURE_MIN_FILTER, min_filter);
  glTexParameteri(tex_type, GL_TEXTURE_MAG_FILTER, mag_filter);
  glTextureStorage2D(textures_[counter_], levels, internal_format, width, height);
  glTextureSubImage2D(textures_[counter_], 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, tex_data);
  // glTexImage2D(tex_type, 0, internal_format, width, height, border, format, GL_UNSIGNED_BYTE, tex_data);
  glGenerateMipmap(tex_type);
  stbi_image_free(tex_data);

  return counter_++;
}

}  // end namespace xac