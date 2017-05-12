#pragma once

#include <type_traits>

#include "query_member_info.h"
#include "typelist.h"

namespace reflect {
namespace detail {

template<typename, typename, bool = true> struct is_callable;

template<typename Set1, typename Set2>
struct is_callable<Set1, Set2, false> : std::false_type {};

template<typename Item, typename... Set1>
struct is_callable<typelist<Item, Set1...>, typelist<>, true> : std::false_type {};

template<typename Item, typename... Set2>
struct is_callable<typelist<>, typelist<Item, Set2...>, true> : std::false_type {};

template<>
struct is_callable<typelist<>, typelist<>, true> : std::true_type {};

template<
   typename CalleeArg,
   typename... Set1,
   typename CallAsArg,
   typename... Set2>
struct is_callable<
   typelist<CalleeArg, Set1...>,
   typelist<CallAsArg, Set2...>,
   true
> : is_callable<
   typelist<Set1...>,
   typelist<Set2...>,
   std::is_convertible_v<CallAsArg, CalleeArg>
>
{};

template<typename CalleeArg, typename CallAsArg>
struct is_callable<CalleeArg, CallAsArg> : is_callable<
   typelist<>,
   typelist<>,
   std::is_convertible_v<CallAsArg, CalleeArg>
>
{};

template<
   typename CalleeReturnType,
   typename... CalleeArgs,
   typename CallAsReturnType,
   typename... CallAsArgs>
struct is_callable<
   CalleeReturnType(CalleeArgs...),
   CallAsReturnType(CallAsArgs...),
   true
> : is_callable<
   typelist<CalleeArgs...>,
   typelist<CallAsArgs...>,
   std::is_convertible_v<CalleeReturnType, CallAsReturnType>
>
{};

template<
   typename CalleeReturnType,
   typename... CalleeArgs,
   typename... CallAsArgs>
struct is_callable<CalleeReturnType(CalleeArgs...), void(CallAsArgs...), true>
   : is_callable<
      typelist<CalleeArgs...>,
      typelist<CallAsArgs...>,
      true
   >
{};

template<typename Callee, typename CallAs>
constexpr bool is_callable_v = is_callable<
   member_type_t<Callee>,
   member_type_t<CallAs>
>::value;

template<typename To, typename From>
constexpr bool is_really_assignable_v = std::is_assignable_v<
   std::add_lvalue_reference_t<To>,
   From
>;

}
}



