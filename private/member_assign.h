#pragma once

#include <string>

#include "member.h"

#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<typename> struct member_name;

template<typename Type, typename ReflectedMemberType>
std::enable_if_t<is_const_member_v<ReflectedMemberType>> assign_to_member(
   Type&&,
   ReflectedMemberType&)
{
   throw invalid_assignment_to_const{
      std::string{ "Cannot assign to const member: " }
      + member_key<ReflectedMemberType>()
   };
}

template<typename Type, typename ReflectedMemberType>
std::enable_if_t<is_function_member_v<ReflectedMemberType>> assign_to_member(
   Type&&,
   ReflectedMemberType&)
{
   throw invalid_assignment_to_const{
      std::string{ "Cannot assign to function member: " }
      + member_key<ReflectedMemberType>()
   };
}

template<typename Type, typename ReflectedMemberType>
std::enable_if_t<
   !is_const_member_v<ReflectedMemberType>
   && !is_function_member_v<ReflectedMemberType>
   && is_static_member_v<ReflectedMemberType>
> assign_to_member(Type&& value, ReflectedMemberType&) {
   *member_ptr_v<ReflectedMemberType> = std::forward<Type>(value);
}

template<typename Type, typename ReflectedMemberType>
std::enable_if_t<
   !is_const_member_v<ReflectedMemberType>
   && !is_function_member_v<ReflectedMemberType>
   && !is_static_member_v<ReflectedMemberType>
> assign_to_member(Type&& value, ReflectedMemberType& reflected) {

   auto* instance = class_instance_for(reflected);
   if (instance) {
      auto member_ptr = member_ptr_v<ReflectedMemberType>;
      instance->*member_ptr = std::forward<Type>(value);

      return;
   }

   throw invalid_requested_member_type{
      std::string{ "Cannot access non-static member: " }
      + member_key<ReflectedMemberType>()
   };
}

}
}
