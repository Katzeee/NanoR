#pragma once
#include <cstdint>
#include <utility>

namespace xac::mpu {

template <typename... Args>
struct type_list {
  constexpr static uint32_t size = sizeof...(Args);
};

template <typename T, typename TList>
struct index_of {};

template <typename T, template <typename...> class TList, typename... Tail>
struct index_of<T, TList<T, Tail...>> : std::integral_constant<uint32_t, 0> {};

template <typename T, template <typename...> class TList, typename Head, typename... Tail>
struct index_of<T, TList<Head, Tail...>> : std::integral_constant<uint32_t, index_of<T, TList<Tail...>>::value + 1> {};

template <typename T, typename TList>
inline constexpr uint32_t index_of_v = index_of<T, TList>::value;

template <uint32_t N, typename TList>  // T can be any type, include a template type
struct type_at;

// specialization for template class T
// here Head and Rest are prepared for T
// T must be a template type itself rather than a specialization of template type
// ...Args here are just for deducing the T and what are Args
// so we must use specialization to implement this
// without the raw template, we cannot do type deducing
template <template <typename...> class TList, typename Head, typename... Tail>
struct type_at<0, TList<Head, Tail...>> : std::common_type<Head> {};

template <uint32_t N, template <typename...> class TList, typename Head, typename... Tail>
struct type_at<N, TList<Head, Tail...>> : type_at<N - 1, TList<Tail...>> {
  static_assert(N < sizeof...(Tail) + 1, "Out of bound");
};

template <uint32_t N, typename TList>
using type_at_t = typename type_at<N, TList>::type;

namespace __detail {
template <template <typename...> class U, typename T>
struct rename {};
template <template <typename...> class U, template <typename...> class T, typename... Args>
struct rename<U, T<Args...>> {
  using type = U<Args...>;
};
}  // namespace __detail

template <template <typename...> class U, typename T>
using rename = typename __detail::rename<U, T>::type;

template <typename T, typename TList>
struct contains : std::false_type {};

template <typename T, template <typename...> class TList, typename... Rest>
struct contains<T, TList<T, Rest...>> : std::true_type {};

template <typename T, template <typename...> class TList, typename Head, typename... Rest>
struct contains<T, TList<Head, Rest...>> : contains<T, TList<Rest...>> {};

namespace test {

using TestTypeList = type_list<int, bool, double>;
static_assert(index_of_v<int, TestTypeList> == 0);
static_assert(index_of_v<bool, TestTypeList> == 1);
static_assert(index_of_v<double, TestTypeList> == 2);
using t = type_at<0, TestTypeList>;
static_assert(std::is_same_v<type_at_t<0, TestTypeList>, int>);
static_assert(std::is_same_v<type_at_t<1, TestTypeList>, bool>);
static_assert(std::is_same_v<type_at_t<2, TestTypeList>, double>);

}  // namespace test

}  // namespace xac::mpu