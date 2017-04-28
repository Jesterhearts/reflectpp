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
   !is_function_member_v<get_member_info_t<ReflectedMemberType>>, ReturnType
> invoke_member(
   ReflectedMemberType&,
   Args&&...)
{
   throw invalid_function_call{
      std::string{ "Cannot call non-function member" }
     + member_name<get_member_info_t<ReflectedMemberType>>::key()
   };
}

template<
   typename ReturnType,
   typename ReflectedMemberType,
   typename... Args>
std::enable_if_t<
   is_function_member_v<get_member_info_t<ReflectedMemberType>>
   && is_static_member_v<get_member_info_t<ReflectedMemberType>>,
   ReturnType
> invoke_member(
   ReflectedMemberType&,
   Args&&... args)
{
   return static_cast<ReturnType>(
      (*member_pointer<
         get_member_info_t<ReflectedMemberType>
      >::get())(std::forward<Args>(args)...)
   );
}

template<
   typename ReturnType,
   typename ReflectedMemberType,
   typename... Args>
std::enable_if_t<
   is_function_member_v<get_member_info_t<ReflectedMemberType>>
   && !is_static_member_v<get_member_info_t<ReflectedMemberType>>,
   ReturnType
> invoke_member(
   ReflectedMemberType& reflected,
   Args&&... args)
{
   auto* instance = reflected.get_instance();
   if (!instance) {
      throw invalid_function_call{
         std::string{ "Cannot call non-static function: " }
         + member_name<get_member_info_t<ReflectedMemberType>>::key()
      };
   }

   auto member_ptr = member_pointer<get_member_info_t<ReflectedMemberType>>::get();
   return static_cast<ReturnType>(
      (instance->*member_ptr)(std::forward<Args>(args)...)
   );
}

}
}