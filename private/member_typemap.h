#pragma once

#include <cstring>

#include "exceptions.h"
#include "extract_underlying_types.h"
#include "find_duplicate_type.h"
#include "member.h"
#include "reflected_instance.h"
#include "type_repr.h"
#include "typelist.h"
#include "typelist_to_numberlist.h"

namespace reflect {
namespace detail {
template<typename, typename, typename, typename> struct member_typemap_impl;

template<typename Class, typename... types>
using member_typemap = member_typemap_impl<
    Class,
    typelist<types...>,
    member_underlyingtype_set<types...>,
    typelist_to_numberlist<member_underlyingtype_set<types...>>
>;

template<
    typename Class,
    template<typename...> class member_typelist,
    template<typename...> class member_underlyingtype_set,
    template<size_t...> class index_list,
    typename... members,
    typename... underlying_members,
    size_t... indexes>
struct member_typemap_impl<
    Class,
    member_typelist<members...>,
    member_underlyingtype_set<underlying_members...>,
    index_list<indexes...>
> : reflected_instance<Class>,
    member<Class, members, type_repr_t<underlying_members...>>...
{
    using Members       = member_typelist<members...>;
    using Types         = member_underlyingtype_set<underlying_members...>;
    using FnTypes       = decltype(filter_fn_types(Types()));
    using ObjTypes      = decltype(filter_obj_types(Types()));
    using Indexes       = index_list<indexes...>;
    using type_repr     = type_repr_t<underlying_members...>;
    using ReflectedType = reflected_member<Class, type_repr>;

    using reflected_instance<Class>::reflected_instance;

    ReflectedType& get_member(
        const char* name,
        size_t len,
        member_typelist<>) const
    {
        throw member_access_error{ std::string{"No member named: "} + name };
    }

    template<typename Option, typename... Options>
    ReflectedType& get_member(
        const char* name,
        size_t len,
        member_typelist<Option, Options...>)
    {
        constexpr auto key = member_name<Option>::key();
        constexpr auto keylen = (sizeof(member_name<Option>::key()) - 1);
        if (len == keylen && std::memcmp(name, key, len) == 0)
        {
            return static_cast<ReflectedType&>(
                static_cast<member<Class, Option, type_repr>&>(*this)
            );
        }

        return get_member(name, len, member_typelist<Options...>());
    }

    ReflectedType& operator[](const char* name) {
        return get_member(name, std::strlen(name), Members());
    }

    ReflectedType& operator[](const std::string& name) {
        return get_member(name.c_str(), name.length(), Members());
    }

    template<typename type>
    constexpr static auto get_type_id() {
        return static_cast<type_repr>(
            get_type_id_impl<type>()
        );
    }

private:
    template<typename type>
    constexpr static auto get_type_id_impl() {
        using TypeInfo = find_duplicate_type<type, Types>;
        static_assert(
            TypeInfo::value,
            "Requested type is not a member type of this class"
        );
        return TypeInfo::index;
    }
};

}
}
