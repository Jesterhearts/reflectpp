#pragma once

#include <string>

#include "member.h"

#include "utility/query_member_info.h"

namespace reflect {
namespace detail {
template<typename> struct member_name;

template<
   typename ReflectedMemberType,
   typename MemberInfo = member_info_t<ReflectedMemberType>
>
void get_member_ref(
   ReflectedMemberType&,
   std::enable_if_t<is_function_member_v<MemberInfo>, bool> = true)
{
   throw invalid_requested_member_type{
      std::string{ "Cannot access function member: " }
      + member_key<ReflectedMemberType>()
   };
}

template<
   typename ReflectedMemberType,
   typename MemberInfo = member_info_t<ReflectedMemberType>
>
auto& get_member_ref(
   ReflectedMemberType&,
   std::enable_if_t<is_static_member_v<MemberInfo>, bool> = true,
   std::enable_if_t<!is_function_member_v<MemberInfo>, bool> = true)
{
   constexpr decltype(auto) member_ptr = get_member_ptr<ReflectedMemberType>();
   return *member_ptr;
}

template<
   typename ReflectedMemberType,
   typename MemberInfo = member_info_t<ReflectedMemberType>
>
auto& get_member_ref(
   ReflectedMemberType& reflected,
   std::enable_if_t<!is_static_member_v<MemberInfo>, bool> = true,
   std::enable_if_t<!is_function_member_v<MemberInfo>, bool> = true)
{
   auto* instance = class_instance_for(reflected);
   if (instance) {
      constexpr decltype(auto) member_ptr = get_member_ptr<ReflectedMemberType>();
      return instance->*member_ptr;
   }

   throw invalid_requested_member_type{
      std::string{ "Cannot access non-static member: " }
      + member_key<ReflectedMemberType>()
   };
}




}
}
