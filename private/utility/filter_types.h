#pragma once

#include <type_traits>

#include "implicitly_equal_types.h"
#include "typelist.h"

namespace reflect {
namespace detail {

template<typename> struct class_reflection_info;

template<typename, typename, typename, typename = void> struct filter_convertible_types;

template<typename Target, typename... Results>
struct filter_convertible_types<
   Target,
   typelist<>,
   typelist<Results...>,
   void>
{
   using types = typelist<Results...>;
};

template<typename Target, typename Candidate, typename... Rest, typename... Results>
struct filter_convertible_types<
   Target,
   typelist<Candidate, Rest...>,
   typelist<Results...>,
   std::enable_if_t<implicitly_equal_v<Candidate, Target>>
> : filter_convertible_types<Target, typelist<Rest...>, typelist<Results..., Candidate>>
{
};

template<typename Target, typename Candidate, typename... Rest, typename... Results>
struct filter_convertible_types<
   Target,
   typelist<Candidate, Rest...>,
   typelist<Results...>,
   std::enable_if_t<!implicitly_equal_v<Candidate, Target>>
> : filter_convertible_types<Target, typelist<Rest...>, typelist<Results...>>
{
};

template<typename Class, typename Target>
using filter_convertible_types_t = typename filter_convertible_types<
   Target,
   typename class_reflection_info<Class>::member_map::Members,
   typelist<>
>::types;

template<typename, typename, typename, typename = void> struct filter_assignable_types;

template<typename From, typename... Results>
struct filter_assignable_types<
   From,
   typelist<>,
   typelist<Results...>,
   void>
{
   using types = typelist<Results...>;
};

template<typename From, typename Candidate, typename... Rest, typename... Results>
struct filter_assignable_types<
   From,
   typelist<Candidate, Rest...>,
   typelist<Results...>,
   std::enable_if_t<implicitly_equal_v<From, Candidate>>
> : filter_assignable_types<From, typelist<Rest...>, typelist<Results..., Candidate>>
{
};

template<typename From, typename Candidate, typename... Rest, typename... Results>
struct filter_assignable_types<
   From,
   typelist<Candidate, Rest...>,
   typelist<Results...>,
   std::enable_if_t<!implicitly_equal_v<From, Candidate>>
> : filter_assignable_types<From, typelist<Rest...>, typelist<Results...>>
{
};

template<typename Class, typename From>
using filter_assignable_types_t = typename filter_assignable_types<
   From,
   typename class_reflection_info<Class>::member_map::Members,
   typelist<>
>::types;

}
}
