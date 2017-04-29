#pragma once
#include <type_traits>

namespace reflect {
namespace detail {

template<typename> struct member_name;
template<typename MemberType, MemberType> struct member_info;
template<typename, typename> struct member;

template<typename Type>
struct get_member_info {
   using type = Type;

};

template<
   typename MemberType,
   MemberType Member>
struct get_member_info<member_info<MemberType, Member>> {
   using type = member_info<MemberType, Member>;
};

template<
   typename Class,
   typename MemberInfo>
struct get_member_info<member<Class, MemberInfo>> {
   using type = MemberInfo;
};

template<typename ReflectedMemberType>
using member_info_t = typename get_member_info<ReflectedMemberType>::type;

template<typename ReflectedMemberType>
constexpr static decltype(auto) member_key() noexcept {
   return member_name<member_info_t<ReflectedMemberType>>::key();
}

template<typename Type>
struct member_attributes {
   using type = Type;
};

template<
   typename MemberType,
   MemberType* Member>
struct member_attributes<member_info<MemberType*, Member>> {
   using type = MemberType;

   constexpr static bool is_const = std::is_const_v<MemberType>;
   constexpr static bool is_static = true;
   constexpr static bool is_function = false;

   constexpr static auto ptr() {
      return Member;
   }
};

template<
   typename MemberType,
   size_t MemberSize,
   MemberType (*Member) [MemberSize]>
struct member_attributes<member_info<MemberType(*)[MemberSize], Member>> {
   using type = MemberType[MemberSize];

   constexpr static bool is_const = std::is_const_v<MemberType>;
   constexpr static bool is_static = true;
   constexpr static bool is_function = false;

   constexpr static auto ptr() {
      return Member;
   }
};

template<
   typename Class,
   typename MemberType,
   MemberType Class::* Member>
struct member_attributes<member_info<MemberType Class::*, Member>> {
   using type = MemberType;

   constexpr static bool is_const = std::is_const_v<MemberType>;
   constexpr static bool is_static = false;
   constexpr static bool is_function = false;

   constexpr static auto ptr() {
      return Member;
   }
};

template<
   typename Class,
   typename MemberType,
   size_t MemberSize,
   MemberType (Class::* Member)[MemberSize]>
struct member_attributes<member_info<MemberType (Class::*)[MemberSize], Member>> {
   using type = MemberType[MemberSize];

   constexpr static bool is_const = std::is_const_v<MemberType>;
   constexpr static bool is_static = false;
   constexpr static bool is_function = false;

   constexpr static auto ptr() {
      return Member;
   }
};

template<
   typename Class,
   typename ReturnType,
   typename... Args,
   ReturnType(Class::* MemberFn)(Args...)>
struct member_attributes<member_info<ReturnType(Class::*)(Args...), MemberFn>> {
   using type = ReturnType(Args...);

   constexpr static bool is_const = false;
   constexpr static bool is_static = false;
   constexpr static bool is_function = true;

   constexpr static auto ptr() {
      return MemberFn;
   }
};

template<
   typename ReturnType,
   typename... Args,
   ReturnType(*MemberFn)(Args...)>
struct member_attributes<member_info<ReturnType(*)(Args...), MemberFn>> {
   using type = ReturnType(Args...);

   constexpr static bool is_const = false;
   constexpr static bool is_static = true;
   constexpr static bool is_function = true;

   constexpr static auto ptr() {
      return MemberFn;
   }
};

template<typename MemberInfo>
constexpr bool is_function_member_v = member_attributes<member_info_t<MemberInfo>>::is_function;

template<typename MemberInfo>
constexpr bool is_static_member_v = member_attributes<member_info_t<MemberInfo>>::is_static;

template<typename MemberInfo>
constexpr bool is_const_member_v = member_attributes<member_info_t<MemberInfo>>::is_const;

template<typename MemberInfo>
using member_type_t = typename member_attributes<member_info_t<MemberInfo>>::type;

template<typename ReflectedMemberType>
constexpr static decltype(auto) get_member_ptr() noexcept {
   return member_attributes<member_info_t<ReflectedMemberType>>::ptr();
}

}
}
