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

template<typename Candidate, typename... Set1, typename From, typename... Set2>
struct implicitly_equal<typelist<Candidate, Set1...>, typelist<From, Set2...>, true>
   : implicitly_equal<
   typelist<Set1...>,
   typelist<Set2...>,
   (std::is_convertible_v<From, Candidate>
      || std::is_constructible_v<Candidate, From>)
   && !(std::is_array_v<Candidate> && std::is_pointer_v<From>)
   >
{};

template<
   typename CandidateReturnType,
   typename... Args1,
   typename TargetReturnType,
   typename... Args2>
   struct implicitly_equal<CandidateReturnType(Args1...), TargetReturnType(Args2...), true>
   : implicitly_equal<
   typelist<Args1...>,
   typelist<Args2...>,
   std::is_convertible<CandidateReturnType, TargetReturnType>::value>
{};

template<
   typename CandidateReturnType,
   typename... Args1,
   typename... Args2>
   struct implicitly_equal<CandidateReturnType(Args1...), void(Args2...), true>
   : implicitly_equal<
   typelist<Args1...>,
   typelist<Args2...>,
   true>
{};


template<typename Candidate, typename From>
struct implicitly_equal<Candidate, From> : implicitly_equal<
   typelist<>,
   typelist<>,
   (std::is_convertible_v<From, Candidate>
      || std::is_constructible_v<Candidate, From>)
   && !(std::is_array_v<Candidate> && std::is_pointer_v<From>) >
{};

}
}



