#pragma once

#include <string>

#include "utility/class_instance_data.h"
#include "utility/implicitly_equal_types.h"
#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<typename MemberInfo, typename Type, typename ClassInstanceData>
std::enable_if_t<is_const_member_v<MemberInfo>>
assign_to_member(Type&&, ClassInstanceData) {
   throw invalid_assignment_to_const{
      std::string{ "Cannot assign to const member: " }
      + member_key<MemberInfo>()
   };
}

template<typename MemberInfo, typename Type, typename ClassInstanceData>
std::enable_if_t<is_function_member_v<MemberInfo>>
assign_to_member(Type&&, ClassInstanceData) {
   throw invalid_assignment_to_function{
      std::string{ "Cannot assign to function member: " }
      + member_key<MemberInfo>()
   };
}

template<typename MemberInfo, typename Type, typename ClassInstanceData>
std::enable_if_t<
   !is_really_assignable_v<member_type_t<MemberInfo>, Type>
   && !is_const_member_v<MemberInfo>
   && !is_function_member_v<MemberInfo>
>
assign_to_member(Type&&, ClassInstanceData) {
   throw invalid_assignment_type{
      std::string{ "Attempting to assign wrong type to member" }
      + member_key<MemberInfo>()
   };
}

template<typename MemberInfo, typename Type, typename Class, bool is_null>
std::enable_if_t<
   is_null
   && !is_static_member_v<MemberInfo>
   && is_really_assignable_v<member_type_t<MemberInfo>, Type>
   && !is_const_member_v<MemberInfo>
   && !is_function_member_v<MemberInfo>
> assign_to_member(Type&&, class_instance_data<Class, is_null>) {
   throw invalid_non_static_assignment{
      std::string{ "Cannot access non-static member: " }
      + member_key<MemberInfo>()
   };
}

template<typename MemberInfo, typename Type, typename ClassInstanceData>
std::enable_if_t<
   is_static_member_v<MemberInfo>
   && is_really_assignable_v<member_type_t<MemberInfo>, Type>
   && !is_const_member_v<MemberInfo>
   && !is_function_member_v<MemberInfo>
> assign_to_member(Type&& value, ClassInstanceData) {
   *get_member_ptr<MemberInfo>() = std::forward<Type>(value);
}

template<typename MemberInfo, typename Type, typename Class, bool is_null>
std::enable_if_t<
   !is_null
   && !is_static_member_v<MemberInfo>
   && is_really_assignable_v<member_type_t<MemberInfo>, Type>
   && !is_const_member_v<MemberInfo>
   && !is_function_member_v<MemberInfo>
> assign_to_member(Type&& value, class_instance_data<Class, is_null> instance)
{
   auto member_ptr = get_member_ptr<MemberInfo>();
   instance.data.*member_ptr = std::forward<Type>(value);
}

template<typename Class, typename Type, bool is_null>
struct assign_member_generator {
   using function_type = void(Type&&, class_instance_data<Class, is_null>);

   template<typename MemberInfo>
   constexpr static function_type& create() noexcept {
      return assign_to_member<MemberInfo>;
   }
};

}
}
