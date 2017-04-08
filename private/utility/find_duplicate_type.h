#pragma once

#include <type_traits>

#include "typelist.h"

namespace reflect {
namespace detail {

template<typename MemberType, MemberType> struct member_info;

template<std::intptr_t, typename, typename>
struct find_duplicate_type_impl;

template<std::intptr_t _index, typename candidate>
struct find_duplicate_type_impl<_index, candidate, typelist<>>
   : std::false_type
{
   constexpr static const std::intptr_t id = _index;
};

template<std::intptr_t _index, typename candidate, typename... list>
struct find_duplicate_type_impl<
   _index,
   candidate,
   typelist<candidate, list...>
> : std::true_type
{
   constexpr static const std::intptr_t id = _index;
};

template<std::intptr_t _index, typename candidate, typename test, typename... list>
struct find_duplicate_type_impl<
   _index,
   candidate,
   typelist<test, list...>
> : find_duplicate_type_impl<_index + 1, candidate, typelist<list...>>
{};

template<typename type, typename _typelist>
struct find_duplicate_type : find_duplicate_type_impl<0, type, _typelist> {};

template<std::intptr_t _index, typename candidate, candidate value, typename... list>
struct find_duplicate_type_impl<
   _index,
   candidate,
   typelist<member_info<candidate, value>, list...>
> : std::true_type
{
   constexpr static const std::intptr_t id = _index;
};

template<
   std::intptr_t _index,
   typename candidate,
   typename test,
   test value,
   typename... list>
   struct find_duplicate_type_impl<
   _index,
   candidate,
   typelist<member_info<test, value>, list...>
> : find_duplicate_type_impl<_index + 1, candidate, typelist<list...>>
{};

template<typename type, type value, typename _typelist>
struct find_duplicate_type<member_info<type, value>, _typelist>
   : find_duplicate_type_impl<0, type, _typelist>
{};

}
}
