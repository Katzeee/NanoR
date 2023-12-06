#pragma once
#include <cstdint>
#include <utility>

namespace xac::mpu {

template <typename... Args>
struct type_list {
  constexpr static uint32_t size = sizeof...(Args);
};

template <uint32_t N, typename T>  // T can be any type, include a template type
struct nth_type;

// specialization for template class T
// here Head and Rest are prepared for T
// T must be a template type itself rather than a specialization of template type
// ...Args here are just for deducing the T and what are Args
// so we must use specialization to implement this
// without the raw template, we cannot do type deducing
template <template <typename...> class T, typename Head, typename... Rest>
struct nth_type<0, T<Head, Rest...>> : std::common_type<Head> {};

template <uint32_t N, template <typename...> class T, typename Head, typename... Rest>
struct nth_type<N, T<Head, Rest...>> : nth_type<N - 1, T<Rest...>> {
  static_assert(N < sizeof...(Rest) + 1, "123");
};

template <template <typename...> class U, typename T>
struct rename {};
template <template <typename...> class U, template <typename...> class T, typename... Args>
struct rename<U, T<Args...>> {
  using type = U<Args...>;
};

}  // namespace xac::mpu