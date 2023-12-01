#pragma once
#include <functional>

namespace xac {

class WatchVarBase {
 public:
  virtual void operator()() = 0;
  virtual ~WatchVarBase() = default;
};

template <typename T>
class WatchVar : public WatchVarBase {
 public:
  using WatchFunc = std::function<void(T)>;
  explicit WatchVar(T &&init_val) : val_(init_val), old_val_(val_) {}

  WatchVar(T &&init_val, WatchFunc &&f) : val_(init_val), old_val_(val_), f_(f) {
    std::invoke(f_, val_);
  }

  void SetWatchFunc(WatchFunc &&f) {
    f_ = std::move(f);
    std::invoke(f_, val_);
  }

  void operator()() override {
    if (old_val_ != val_) {
      std::invoke(f_, val_);
    }
    old_val_ = val_;
  }

  auto Data() -> T * {
    return &val_;
  }

  auto Value() -> T {
    return val_;
  }

 private:
  T val_;
  T old_val_;
  WatchFunc f_;
};
}  // namespace xac
