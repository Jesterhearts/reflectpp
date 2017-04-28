#pragma once

namespace reflect {
namespace detail {

template<typename MemberType, MemberType> struct member_info;
template<typename, typename> struct member;

template<typename> struct member_pointer;

template<
   typename MemberType,
   MemberType* Member>
struct member_pointer<member_info<MemberType*, Member>> {
   constexpr static auto get() {
      return Member;
   }
};

template<
   typename MemberType,
   size_t MemberSize,
   MemberType (*Member) [MemberSize]>
struct member_pointer<member_info<MemberType(*)[MemberSize], Member>> {
   constexpr static auto get() {
      return Member;
   }
};

template<
   typename Class,
   typename MemberType,
   MemberType Class::* Member>
struct member_pointer<member_info<MemberType Class::*, Member>> {
   constexpr static auto get() {
      return Member;
   }
};

template<
   typename Class,
   typename ReturnType,
   typename... Args,
   ReturnType(Class::* MemberFn)(Args...)>
struct member_pointer<member_info<ReturnType(Class::*)(Args...), MemberFn>> {
   constexpr static auto get() {
      return MemberFn;
   }
};

template<
   typename ReturnType,
   typename... Args,
   ReturnType(*MemberFn)(Args...)>
struct member_pointer<member_info<ReturnType(*)(Args...), MemberFn>> {
   constexpr static auto get() {
      return MemberFn;
   }
};

template<typename> struct get_member_info;

template<
   typename Class,
   typename MemberInfo>
struct get_member_info<member<Class, MemberInfo>> {
   using type = typename MemberInfo;
};

template<typename ReflectedMemberType>
using get_member_info_t = typename get_member_info<ReflectedMemberType>::type;

template<typename> struct is_function_member : std::false_type{};

template<
   typename Class,
   typename ReturnType,
   typename... Args,
   ReturnType(Class::* MemberFn)(Args...)>
struct is_function_member<
   member_info<ReturnType(Class::*)(Args...), MemberFn>
> : std::true_type {
};

template<
   typename ReturnType,
   typename... Args,
   ReturnType(*MemberFn)(Args...)>
struct is_function_member<
   member_info<ReturnType(*)(Args...), MemberFn>
> : std::true_type {
};

template<typename MemberInfo>
constexpr bool is_function_member_v = is_function_member<MemberInfo>::value;

template<typename> struct is_static_member : std::false_type{};

template<
   typename MemberType,
   MemberType* Member>
struct is_static_member<
   member_info<MemberType*, Member>
> : std::true_type {
};

template<typename MemberInfo>
constexpr bool is_static_member_v = is_static_member<MemberInfo>::value;


template<typename MemberInfo>
constexpr bool is_const_member_v = std::is_const_v<
   extract_underlying_type_t<MemberInfo>
>;

}
}
