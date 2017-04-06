#pragma once

#include <type_traits>

#include "typelist.h"

namespace reflect {
namespace detail {

template<typename, typename, bool = true> struct implicitly_equal;

template<typename Set1, typename Set2>
struct implicitly_equal<Set1, Set2, false> : std::false_type {};

template<typename... Set1>
struct implicitly_equal<typelist<Set1...>, typelist<>, true> : std::false_type {};

template<typename... Set2>
struct implicitly_equal<typelist<>, typelist<Set2...>, true> : std::false_type {};

template<>
struct implicitly_equal<typelist<>, typelist<>, true> : std::true_type {};

template<typename T1, typename... Set1, typename T2, typename... Set2>
struct implicitly_equal<typelist<T1, Set1...>, typelist<T2, Set2...>, true>
    : implicitly_equal<
        typelist<Set1...>,
        typelist<Set2...>,
        std::is_convertible_v<T1, T2>
        || std::is_constructible_v<T1, T2>
        || (std::is_pointer_v<T1> && std::is_array_v<T2>)
        || (std::is_array_v<T1> && std::is_pointer_v<T2>)
    >
{};

template<
    typename ReturnType1,
    typename... Args1,
    typename ReturnType2,
    typename... Args2>
struct implicitly_equal<ReturnType1(Args1...), ReturnType2(Args2...), true>
    : implicitly_equal<
        typelist<Args1...>,
        typelist<Args2...>,
        std::is_convertible<ReturnType1, ReturnType2>::value>
{};

template<
    typename... Args1,
    typename ReturnType2,
    typename... Args2>
struct implicitly_equal<void(Args1...), ReturnType2(Args2...), true>
    : implicitly_equal<
        typelist<Args1...>,
        typelist<Args2...>,
        true>
{};

template<typename Type1, typename Type2>
struct implicitly_equal<Type1, Type2, true>
    : implicitly_equal<
        typelist<>,
        typelist<>,
        std::is_convertible_v<Type1, Type2>
        || (std::is_pointer_v<Type1> && std::is_array_v<Type2>)
        || (std::is_array_v<Type1> && std::is_pointer_v<Type2>)
   >
{};

}
}



