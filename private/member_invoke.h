#pragma once

#include <string>

#include "member.h"

#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<typename> struct member_name;

template<
   typename ReturnType,
   typename ReflectedMemberType,
   typename... Args>
std::enable_if_t<
   !is_function_member_v<member_info_t<ReflectedMemberType>>, ReturnType
> invoke_member(
   ReflectedMemberType&,
   Args&&...)
{
   throw invalid_function_call{
      std::string{ "Cannot call non-function member" }
     + member_name<member_info_t<ReflectedMemberType>>::key()
   };
}

template<
   typename ReturnType,
   typename ReflectedMemberType,
   typename... Args>
std::enable_if_t<
   is_function_member_v<member_info_t<ReflectedMemberType>>
   && is_static_member_v<member_info_t<ReflectedMemberType>>,
   ReturnType
> invoke_member(
   ReflectedMemberType&,
   Args&&... args)
{
   return static_cast<ReturnType>(
      (*member_attributes<
         member_info_t<ReflectedMemberType>
      >::ptr())(std::forward<Args>(args)...)
   );
}

template<
   typename ReturnType,
   typename ReflectedMemberType,
   typename... Args>
std::enable_if_t<
   is_function_member_v<member_info_t<ReflectedMemberType>>
   && !is_static_member_v<member_info_t<ReflectedMemberType>>,
   ReturnType
> invoke_member(
   ReflectedMemberType& reflected,
   Args&&... args)
{
   auto* instance = class_instance_for(reflected);
   if (instance) {
      auto member_ptr = member_attributes<member_info_t<ReflectedMemberType>>::ptr();
      return static_cast<ReturnType>(
         (instance->*member_ptr)(std::forward<Args>(args)...)
      );
   }

   throw invalid_function_call{
      std::string{ "Cannot call non-static function: " }
      + member_name<member_info_t<ReflectedMemberType>>::key()
   };
}

}
}