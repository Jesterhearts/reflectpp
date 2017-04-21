#pragma once

#include <type_traits>

#include "find_duplicate_type.h"
#include "implicitly_equal_types.h"
#include "typelist.h"

namespace reflect {
namespace detail {
template<typename candidate, typename... filtered>
constexpr auto filter_types(typelist<candidate>, typelist<filtered...>, bool) {
   return typelist<filtered..., candidate>();
}

template<typename candidate, typename... list, typename... filtered>
constexpr auto filter_types(
   typelist<candidate, list...>,
   typelist<filtered...>,
   std::enable_if_t<
   !find_duplicate_type<candidate, typelist<list...>
   >::value, bool>)
{
   return filter_types(
      typelist<list...>(),
      typelist<filtered..., candidate>(),
      true
   );
}

template<typename candidate, typename... list, typename... filtered>
constexpr auto filter_types(
   typelist<candidate, list...>,
   typelist<filtered...>,
   std::enable_if_t<
   find_duplicate_type<candidate, typelist<list...>
   >::value, bool>)
{
   return filter_types(typelist<list...>(), typelist<filtered...>(), true);
}

template<typename... list>
constexpr auto filter_types(typelist<list...>) {
   return filter_types(typelist<list...>(), typelist<>(), true);
}

template<typename... _list> using typeset = decltype(
   filter_types(typelist<_list...>())
);

///////////////////////////////////////////////////////////////////////////////
template<template<typename> class filter, typename... filtered>
constexpr auto filter_types(typelist<>, typelist<filtered...>, char) {
   return typelist<filtered...>();
}

template<
   template<typename> class filter,
   typename candidate,
   typename... candidates,
   typename... filtered>
   constexpr auto filter_types(
      typelist<candidate, candidates...>,
      typelist<filtered...>,
      std::enable_if_t<filter<candidate>::value, char>)
{
   return filter_types<filter>(
      typelist<candidates...>(),
      typelist<filtered..., candidate>(),
      '\0'
   );
}

template<
   template<typename> class filter,
   typename candidate,
   typename... candidates,
   typename... filtered>
   constexpr auto filter_types(
      typelist<candidate, candidates...>,
      typelist<filtered...>,
      std::enable_if_t<!filter<candidate>::value, char>)
{
   return filter_types<filter>(
      typelist<candidates...>(),
      typelist<filtered...>(),
      '\0'
   );
}

///////////////////////////////////////////////////////////////////////////////
template<typename... list>
constexpr auto filter_fn_types(typelist<list...>) {
   return filter_types<std::is_function>(
      typelist<list...>(),
      typelist<>(),
      '\0'
   );
}

///////////////////////////////////////////////////////////////////////////////
template<typename Target, typename... filtered>
constexpr auto filter_compatible_types(
   typelist<>,
   typelist<filtered...>,
   bool)
{
   return typelist<filtered...>();
}

template<
   typename Target,
   typename Candidate,
   typename... Candidates,
   typename... filtered>
   constexpr auto filter_compatible_types(
      typelist<Candidate, Candidates...>,
      typelist<filtered...>,
      std::enable_if_t<implicitly_equal<Candidate, Target>::value, bool>)
{
   return filter_compatible_types<Target>(
      typelist<Candidates...>(),
      typelist<filtered..., Candidate>(),
      true
   );
}

template<
   typename Target,
   typename Candidate,
   typename... Candidates,
   typename... filtered>
   constexpr auto filter_compatible_types(
      typelist<Candidate, Candidates...>,
      typelist<filtered...>,
      std::enable_if_t<!implicitly_equal<Candidate, Target>::value, bool>)
{
   return filter_compatible_types<Target>(
      typelist<Candidates...>(),
      typelist<filtered...>(),
      true
   );
}

template<typename Target, typename... list>
constexpr auto filter_compatible_types(typelist<list...>) {
   return filter_compatible_types<Target>(
      typelist<list...>(),
      typelist<>(),
      true
   );
}

///////////////////////////////////////////////////////////////////////////////
template<typename... list>
constexpr auto filter_obj_types(typelist<list...>) {
   return filter_types<std::is_object>(typelist<list...>(), typelist<>(), '\0');
}

}
}
