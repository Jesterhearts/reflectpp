#pragma once

#include <type_traits>

#include "typelist.h"

namespace reflect {
namespace detail {

template<typename> struct class_reflection_info;

template<std::intptr_t, typename, typename>
struct type_and_index;

template<std::size_t _index, typename Candidate>
struct type_and_index<_index, Candidate, typelist<>>
   : std::false_type
{
   using type = Candidate;
   constexpr static const std::size_t index = _index;
};

template<std::size_t _index, typename Candidate, typename... Rest>
struct type_and_index<
   _index,
   Candidate,
   typelist<Candidate, Rest...>
> : std::true_type
{
   using type = Candidate;
   constexpr static const std::size_t index = _index;
};

template<std::size_t _index, typename Candidate, typename Test, typename... Rest>
struct type_and_index<
   _index,
   Candidate,
   typelist<Test, Rest...>
> : type_and_index<_index + 1, Candidate, typelist<Rest...>>
{};

template<typename Class, typename Type>
using type_and_index_t = type_and_index<
   0, Type,
   typename class_reflection_info<Class>::member_map::Members
>;


}
}
