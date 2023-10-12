#pragma once
#include <functional>

namespace xac {
template <typename T, typename F>
class CheckChangeThen {
 public:
  CheckChangeThen(T *val_addr, F &&f) : val_addr_(val_addr), old_val_(*val_addr), f_(f) { std::invoke(f_, old_val_); }

  void operator()() {
    if (old_val_ != *val_addr_) {
      std::invoke(f_, *val_addr_);
    }
    old_val_ = *val_addr_;
  }

 private:
  T *val_addr_;
  T old_val_;
  F f_;
};
}  // namespace xac
