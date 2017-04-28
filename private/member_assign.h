#pragma once

#include <string>

#include "member.h"

#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<typename> struct member_name;

template<
   typename Type,
   typename ReflectedMemberType,
   typename MemberInfo = get_member_info_t<ReflectedMemberType>
>
void assign_to_member(
   Type&&,
   ReflectedMemberType&,
   std::enable_if_t<is_const_member_v<MemberInfo>, bool> = true)
{
   throw invalid_assignment_to_const{
      std::string{ "Cannot assign to const member: " }
      + member_name<MemberInfo>::key()
   };
}

template<
   typename Type,
   typename ReflectedMemberType,
   typename MemberInfo = get_member_info_t<ReflectedMemberType>
>
void assign_to_member(
   Type&&,
   ReflectedMemberType&,
   std::enable_if_t<is_function_member_v<MemberInfo>, bool> = true)
{
   throw invalid_assignment_to_const{
      std::string{ "Cannot assign to function member: " }
      + member_name<MemberInfo>::key()
   };
}

template<
   typename Type,
   typename ReflectedMemberType,
   typename MemberInfo = get_member_info_t<ReflectedMemberType>
>
void assign_to_member(
   Type&& value,
   ReflectedMemberType&,
   std::enable_if_t<
      !is_const_member_v<MemberInfo> && !is_function_member_v<MemberInfo>,
      bool
   > = true,
   std::enable_if_t<is_static_member_v<MemberInfo>, bool> = true)
{
   *member_pointer<MemberInfo>::get() = std::forward<Type>(value);
}

template<
   typename Type,
   typename ReflectedMemberType,
   typename MemberInfo = get_member_info_t<ReflectedMemberType>
>
void assign_to_member(
   Type&& value,
   ReflectedMemberType& reflected,
   std::enable_if_t<
      !is_const_member_v<MemberInfo> && !is_function_member_v<MemberInfo>,
      bool
   > = true,
   std::enable_if_t<!is_static_member_v<MemberInfo>, bool> = true)
{
   auto* instance = reflected.get_instance();
   if (!instance) {
      throw invalid_requested_member_type{
         std::string{ "Cannot access non-static member: " }
         + member_name<MemberInfo>::key()
      };
   }

   auto member_ptr = member_pointer<MemberInfo>::get();
   instance->*member_ptr = std::forward<Type>(value);
}

}
}
