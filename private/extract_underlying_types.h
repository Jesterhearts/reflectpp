#pragma once

#include "filter_types.h"
#include "member_info.h"
#include "typelist.h"

namespace reflect {
namespace detail {
template<typename> struct extract_underlying_type;

template<
    typename MemberType,
    typename Class,
    MemberType Class::* Member,
    typename... Members>
struct extract_underlying_type<
    typelist<member_info<MemberType Class::*, Member>, Members... >>
{
    using remain = typelist<Members...>;
    using head = MemberType;
};

template<
    typename ReturnType,
    typename Class,
    typename... Args,
    ReturnType(Class::* Member)(Args...),
    typename... Members>
struct extract_underlying_type<
    typelist<member_info<ReturnType(Class::*)(Args...), Member>, Members... >>
{
    using remain = typelist<Members...>;
    using head = ReturnType(*) (Args...);
};

template<typename... extracted>
constexpr const auto extract_underlying_types(
    typelist<>,
    typelist<extracted...>)
{
    return typelist<extracted...>();
}

template<typename... members, typename... extracted>
constexpr const auto extract_underlying_types(
    typelist<members...>,
    typelist<extracted...>)
{
    using Extracted1 = extract_underlying_type<typelist<members...>>;
    return extract_underlying_types(
        typename Extracted1::remain(),
        typelist<extracted..., typename Extracted1::head>()
    );
}

template<typename... members>
constexpr const auto extract_underlying_types(typelist<members...>) {
    using Extracted1 = extract_underlying_type<typelist<members...>>;
    return extract_underlying_types(
        typename Extracted1::remain(),
        typelist<typename Extracted1::head>()
    );
}

template<typename... list>
using member_underlyingtype_list = decltype(
    extract_underlying_types(typelist<list...>())
);

template<typename... list>
using member_underlyingtype_set = decltype(
    filter_types(member_underlyingtype_list<list...>())
);

}
}
