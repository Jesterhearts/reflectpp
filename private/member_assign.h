#pragma once

#include <string>

#include "member.h"
#include "utility/implicitly_equal_types.h"
#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<typename MemberInfo, typename Type, typename Class>
std::enable_if_t<is_const_member_v<MemberInfo>>
assign_to_member(Class*, Type&&) {
   throw invalid_assignment_to_const{
      std::string{ "Cannot assign to const member: " }
      + member_key<MemberInfo>()
   };
}

template<typename MemberInfo, typename Type, typename Class>
std::enable_if_t<is_function_member_v<MemberInfo>>
assign_to_member(Class*, Type&&) {
   throw invalid_assignment_type{
      std::string{ "Cannot assign to function member: " }
      + member_key<MemberInfo>()
   };
}

template<typename MemberInfo, typename Type, typename Class>
std::enable_if_t<
   !is_really_assignable_v<member_type_t<MemberInfo>, Type>
   && !is_const_member_v<MemberInfo>
   && !is_function_member_v<MemberInfo>
>
assign_to_member(Class*, Type&&) {
   throw invalid_assignment_type{
      std::string{ "Attempting to assign wrong type to member" }
      + member_key<MemberInfo>()
   };
}

template<typename MemberInfo, typename Type, typename Class>
std::enable_if_t<
   is_static_member_v<MemberInfo>
   && is_really_assignable_v<member_type_t<MemberInfo>, Type>
   && !is_const_member_v<MemberInfo>
   && !is_function_member_v<MemberInfo>
> assign_to_member(Class*, Type&& value) {
   *get_member_ptr<MemberInfo>() = std::forward<Type>(value);
}

template<typename MemberInfo, typename Type, typename Class>
std::enable_if_t<
   !is_static_member_v<MemberInfo>
   && is_really_assignable_v<member_type_t<MemberInfo>, Type>
   && !is_const_member_v<MemberInfo>
   && !is_function_member_v<MemberInfo>
> assign_to_member(Class* instance, Type&& value) {
   if (instance) {
      auto member_ptr = get_member_ptr<MemberInfo>();
      instance->*member_ptr = std::forward<Type>(value);
      return;
   }

   throw invalid_assignment_type{
      std::string{ "Cannot access non-static member: " }
      + member_key<MemberInfo>()
   };
}

template<typename Class, typename Type>
struct assign_member_generator {
   using function_type = void(Class*, Type&&);

   template<typename MemberInfo>
   constexpr static function_type* create() noexcept {
      return &assign_to_member<MemberInfo, Type, Class>;
   }
};

}
}
