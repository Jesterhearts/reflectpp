#pragma once

#include <type_traits>

#include "find_duplicate_type.h"
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

}
}
