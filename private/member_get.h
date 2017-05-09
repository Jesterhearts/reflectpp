#pragma once

#include <string>

#include "member.h"

#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<typename> struct member_name;
template<typename> struct reflected_member;

template<typename ReturnType, typename ReflectedMemberType, typename Class>
std::enable_if_t<is_function_member_v<ReflectedMemberType>, ReturnType> get_member_ref(
   reflected_member<Class>&)
{
   throw invalid_requested_member_type{
      std::string{ "Cannot access function member: " }
      + member_key<ReflectedMemberType>()
   };
}

template<typename ReturnType, typename ReflectedMemberType, typename Class>
std::enable_if_t<
   is_static_member_v<ReflectedMemberType> && !is_function_member_v<ReflectedMemberType>,
   ReturnType
> get_member_ref(reflected_member<Class>&) {
   constexpr decltype(auto) member_ptr = get_member_ptr<ReflectedMemberType>();
   return static_cast<ReturnType>(*member_ptr);
}

template<typename ReturnType, typename ReflectedMemberType, typename Class>
std::enable_if_t<
   !is_static_member_v<ReflectedMemberType> && !is_function_member_v<ReflectedMemberType>,
   ReturnType
> get_member_ref(reflected_member<Class>& reflected) {
   auto* instance = class_instance_for<ReflectedMemberType>(reflected);
   if (instance) {
      constexpr decltype(auto) member_ptr = get_member_ptr<ReflectedMemberType>();
      return static_cast<ReturnType>(instance->*member_ptr);
   }

   throw invalid_requested_member_type{
      std::string{ "Cannot access non-static member: " }
      + member_key<ReflectedMemberType>()
   };
}

template<typename Class, typename ReturnType>
struct get_member_ref_generator {
   using function_type = ReturnType(reflected_member<Class>&);

   template<typename MemberType>
   constexpr static function_type* create() noexcept {
      return &get_member_ref<ReturnType, MemberType, Class>;
   }
};

}
}
