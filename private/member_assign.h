#pragma once

#include <string>

#include "member.h"

#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<typename> struct member_name;
template<typename> struct reflected_member;

template<typename ReflectedMemberType, typename Type, typename Class>
std::enable_if_t<is_const_member_v<ReflectedMemberType>>
assign_to_member(reflected_member<Class>&, Type&&) {
   throw invalid_assignment_to_const{
      std::string{ "Cannot assign to const member: " }
      + member_key<ReflectedMemberType>()
   };
}

template<typename ReflectedMemberType, typename Type, typename Class>
std::enable_if_t<is_function_member_v<ReflectedMemberType>>
assign_to_member(reflected_member<Class>&, Type&&) {
   throw invalid_assignment_to_const{
      std::string{ "Cannot assign to function member: " }
      + member_key<ReflectedMemberType>()
   };
}

template<typename ReflectedMemberType, typename Type, typename Class>
std::enable_if_t<
   !is_const_member_v<ReflectedMemberType>
   && !is_function_member_v<ReflectedMemberType>
   && is_static_member_v<ReflectedMemberType>
> assign_to_member(reflected_member<Class>&, Type&& value) {
   *get_member_ptr<ReflectedMemberType>() = std::forward<Type>(value);
}

template<typename ReflectedMemberType, typename Type, typename Class>
std::enable_if_t<
   !is_const_member_v<ReflectedMemberType>
   && !is_function_member_v<ReflectedMemberType>
   && !is_static_member_v<ReflectedMemberType>
> assign_to_member(reflected_member<Class>& reflected, Type&& value) {
   auto* instance = class_instance_for<ReflectedMemberType>(reflected);
   if (instance) {
      auto member_ptr = get_member_ptr<ReflectedMemberType>();
      instance->*member_ptr = std::forward<Type>(value);

      return;
   }

   throw invalid_requested_member_type{
      std::string{ "Cannot access non-static member: " }
      + member_key<ReflectedMemberType>()
   };
}

template<typename Class, typename Type>
struct assign_member_generator {
   using function_type = void(reflected_member<Class>&, Type&&);

   template<typename MemberType>
   constexpr static function_type* create() noexcept {
      return &assign_to_member<MemberType, Type, Class>;
   }
};

}
}
