#pragma once

#include <type_traits>

#include "extract_underlying_types.h"
#include "typelist.h"

namespace reflect {
namespace detail {

template<typename, typename, bool = true> struct implicitly_equal;

template<typename From, typename To>
constexpr bool implicitly_equal_v = implicitly_equal<
   extract_underlying_type_t<From>,
   extract_underlying_type_t<To>
>::value;

template<typename Set1, typename Set2>
struct implicitly_equal<Set1, Set2, false> : std::false_type {};

template<typename Item, typename... Set1>
struct implicitly_equal<typelist<Item, Set1...>, typelist<>, true> : std::false_type {};

template<typename Item, typename... Set2>
struct implicitly_equal<typelist<>, typelist<Item, Set2...>, true> : std::false_type {};

template<>
struct implicitly_equal<typelist<>, typelist<>, true> : std::true_type {};

template<typename From, typename... Set1, typename To, typename... Set2>
struct implicitly_equal<typelist<From, Set1...>, typelist<To, Set2...>, true>
: implicitly_equal<
   typelist<Set1...>,
   typelist<Set2...>,
   (std::is_convertible_v<From, To>
      || std::is_constructible_v<To, From>)
   && !(std::is_array_v<To> && std::is_pointer_v<From>)
>
{};

template<typename From, typename To>
struct implicitly_equal<From, To> : implicitly_equal<
   typelist<>,
   typelist<>,
   (std::is_convertible_v<From, To>
      || std::is_constructible_v<To, From>)
   && !(std::is_array_v<To> && std::is_pointer_v<From>) >
{};

template<
   typename FromReturnType,
   typename... Args1,
   typename ToReturnType,
   typename... Args2>
struct implicitly_equal<FromReturnType(Args1...), ToReturnType(Args2...), true>
: implicitly_equal<
   typelist<Args1...>,
   typelist<Args2...>,
   implicitly_equal_v<FromReturnType, ToReturnType>>
{
};

template<
   typename FromReturnType,
   typename... Args1,
   typename... Args2>
struct implicitly_equal<FromReturnType(Args1...), void(Args2...), true>
: implicitly_equal<
   typelist<Args1...>,
   typelist<Args2...>,
   true>
{
};



}
}



