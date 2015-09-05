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
template<typename... filtered>
constexpr auto filter_fn_types(typelist<>, typelist<filtered...>, bool) {
    return typelist<filtered...>();
}

template<typename candidate, typename... candidates, typename... filtered>
constexpr auto filter_fn_types(
    typelist<candidate, candidates...>,
    typelist<filtered...>,
    std::enable_if_t<std::is_function<candidate>::value, bool>)
{
    return filter_fn_types(
        typelist<candidates...>(),
        typelist<filtered..., candidate>(),
        true
    );
}

template<typename candidate, typename... candidates, typename... filtered>
constexpr auto filter_fn_types(
    typelist<candidate, candidates...>,
    typelist<filtered...>,
    std::enable_if_t<!std::is_function<candidate>::value, bool>)
{
    return filter_fn_types(
        typelist<candidates...>(),
        typelist<filtered...>(),
        true
    );
}

template<typename... list>
constexpr auto filter_fn_types(typelist<list...>) {
    return filter_fn_types(typelist<list...>(), typelist<>(), true);
}

///////////////////////////////////////////////////////////////////////////////
template<typename FnType, typename... filtered>
constexpr auto filter_compatible_fns(
    typelist<>,
    typelist<filtered...>,
    bool)
{
    return typelist<filtered...>();
}

template<
    typename FnType,
    typename candidate,
    typename... candidates,
    typename... filtered>
constexpr auto filter_compatible_fns(
    typelist<candidate, candidates...>,
    typelist<filtered...>,
    std::enable_if_t<implicitly_equal<FnType, candidate>::value, bool>)
{
    return filter_compatible_fns<FnType>(
        typelist<candidates...>(),
        typelist<filtered..., candidate>(),
        true
    );
}

template<
    typename FnType,
    typename candidate,
    typename... candidates,
    typename... filtered>
constexpr auto filter_compatible_fns(
    typelist<candidate, candidates...>,
    typelist<filtered...>,
    std::enable_if_t<!implicitly_equal<FnType, candidate>::value, bool>)
{
    return filter_compatible_fns<FnType>(
        typelist<candidates...>(),
        typelist<filtered...>(),
        true
    );
}

template<typename FnType, typename... list>
constexpr auto filter_compatible_fns(typelist<list...>) {
    return filter_compatible_fns<FnType>(
        typelist<list...>(),
        typelist<>(),
        true
    );
}

}
}
