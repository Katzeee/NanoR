#pragma once
#ifndef NDEBUG

#ifdef _WIN32
#include <source_location>  // MSVC
using source_location = std::source_location;
#else
#include <experimental/source_location>
using source_location = std::experimental::fundamentals_v2::source_location;
#endif
#include <iostream>
#include <source_location>
#include <type_traits>
#include <utility>

#include "nanorpch.h"

namespace nanoR {
template <typename T>
class SourceLocationHelper {
 public:
  template <typename U>
  requires std::constructible_from<T, U>
  consteval SourceLocationHelper(U &&fmt, source_location location = source_location::current())
      : fmt_(std::forward<U>(fmt)), location_(location) {}
  T fmt_;
  source_location location_;
};

enum class LogLevel { kTrace = 0, kDebug, kInfo, kWarning, kError, kFatal };

class Logger {
 public:
  template <typename... T>
  // HINT: fmt::format_string requires fmt to be a constexpr
  void Log(LogLevel level, SourceLocationHelper<fmt::format_string<T...>> fmt, T &&...args) {
    if (level >= level_) {
      std::cout << fmt::format(
                       "[{:^9}] {}:{} || {:<30} ", LevelToString(level) + 1, fmt.location_.file_name(), 
                       fmt.location_.line(), fmt.location_.function_name() 
                   ) + fmt::vformat(fmt.fmt_, fmt::make_format_args(args...));
    }
  }
  void SetLogLevel(LogLevel level) {
    level_ = level;
  }

 private:
  LogLevel level_ = LogLevel::kTrace;
  const char *LevelToString(LogLevel level) {
    switch (level) {
#define X(level)        \
  case LogLevel::level: \
    return #level;
      X(kTrace)
      X(kDebug)
      X(kInfo)
      X(kWarning)
      X(kError)
      X(kFatal)
#undef X
      default:
        return "kUnkonwn";
    }
  }
};

}  // namespace nanoR

extern nanoR::Logger *logger;

#define LOG(level, ...) logger->Log(level, __VA_ARGS__)
#define LOG_TRACE(...) LOG(nanoR::LogLevel::kTrace, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(nanoR::LogLevel::kDebug, __VA_ARGS__)
#define LOG_INFO(...) LOG(nanoR::LogLevel::kInfo, __VA_ARGS__)
#define LOG_WARNING(...) LOG(nanoR::LogLevel::kWarning, __VA_ARGS__)
#define LOG_ERROR(...) LOG(nanoR::LogLevel::kError, __VA_ARGS__)
#define LOG_FATAL(...) LOG(nanoR::LogLevel::kFatal, __VA_ARGS__)
#else
namespace nanoR {
class Logger {};
}  // namespace nanoR
#define LOG_TRACE(...)
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARNING(...)
#define LOG_ERROR(...)
#define LOG_FATAL(...)
#endif