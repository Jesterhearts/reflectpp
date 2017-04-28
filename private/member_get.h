#pragma once

#include <string>

#include "member.h"

#include "utility/query_member_info.h"

namespace reflect {
namespace detail {
template<typename> struct member_name;

template<
   typename ReflectedMemberType,
   typename MemberInfo = get_member_info_t<ReflectedMemberType>
>
void get_member_ref(
   ReflectedMemberType&,
   std::enable_if_t<is_function_member_v<MemberInfo>, bool> = true)
{
   throw invalid_requested_member_type{
      std::string{ "Cannot access function member: " }
      + member_name<MemberInfo>::key()
   };
}

template<
   typename ReflectedMemberType,
   typename MemberInfo = get_member_info_t<ReflectedMemberType>
>
auto& get_member_ref(
   ReflectedMemberType&,
   std::enable_if_t<is_static_member_v<MemberInfo>, bool> = true,
   std::enable_if_t<!is_function_member_v<MemberInfo>, bool> = true)
{
   auto member_ptr = member_pointer<MemberInfo>::get();
   return *member_ptr;
}

template<
   typename ReflectedMemberType,
   typename MemberInfo = get_member_info_t<ReflectedMemberType>
>
auto& get_member_ref(
   ReflectedMemberType& reflected,
   std::enable_if_t<!is_static_member_v<MemberInfo>, bool> = true,
   std::enable_if_t<!is_function_member_v<MemberInfo>, bool> = true)
{
   auto* instance = reflected.get_instance();
   if (!instance) {
      throw invalid_requested_member_type{
         std::string{ "Cannot access non-static member: " }
         + member_name<MemberInfo>::key()
      };
   }

   auto member_ptr = member_pointer<MemberInfo>::get();
   return instance->*member_ptr;
}




}
}
