#pragma once

#include <type_traits>

#include "member_info.h"
#include "typelist.h"


namespace reflect {
namespace detail {

template<size_t, typename, typename>
struct find_duplicate_type_impl;

template<size_t _index, typename candidate>
struct find_duplicate_type_impl<_index, candidate, typelist<>>
    : std::false_type
{
    constexpr static const size_t index = _index;
};

template<size_t _index, typename candidate, typename... list>
struct find_duplicate_type_impl<
    _index,
    candidate,
    typelist<candidate, list...>
> : std::true_type
{
    constexpr static const size_t index = _index;
};

template<size_t _index, typename candidate, typename test, typename... list>
struct find_duplicate_type_impl<
    _index,
    candidate,
    typelist<test, list...>
> : find_duplicate_type_impl<_index + 1, candidate, typelist<list...>>
{};

template<typename type, typename _typelist>
struct find_duplicate_type : find_duplicate_type_impl<0, type, _typelist> {};

template<size_t _index, typename candidate, candidate value, typename... list>
struct find_duplicate_type_impl<
    _index,
    candidate,
    typelist<member_info<candidate, value>, list...>
> : std::true_type
{
    constexpr static const size_t index = _index;
};

template<
    size_t _index,
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
