#pragma once

namespace reflect {
namespace detail {
template<typename MemberType, MemberType> struct member_info;

template<typename Type>
struct extract_underlying_type {
   using type = Type;
};

template<
   typename MemberType,
   typename Class,
   MemberType Class::* Member>
struct extract_underlying_type<
   member_info<MemberType Class::*, Member>>
{
   using type = MemberType;
};

template<
   typename ReturnType,
   typename Class,
   typename... Args,
   ReturnType(Class::* Member)(Args...)>
struct extract_underlying_type<
   member_info<ReturnType(Class::*)(Args...), Member>>
{
   using type = ReturnType(Args...);
};

template<
   typename ReturnType,
   typename... Args,
   ReturnType(*Member)(Args...)>
struct extract_underlying_type<
   member_info<ReturnType(*)(Args...), Member>>
{
   using type = ReturnType(Args...);
};

template<
   typename MemberType,
   MemberType* Member>
struct extract_underlying_type<
   member_info<MemberType*, Member>>
{
   using type = MemberType;
};

template<typename MemberType>
using extract_underlying_type_t = typename extract_underlying_type<MemberType>::type;

}
}
