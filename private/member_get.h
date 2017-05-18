#pragma once

#include <string>

#include "utility/class_instance_data.h"
#include "utility/implicitly_equal_types.h"
#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<typename ReturnType, typename MemberInfo, typename ClassInstanceData>
std::enable_if_t<is_function_member_v<MemberInfo>, ReturnType>
get_member_ref(ClassInstanceData) {
   throw invalid_requested_member_type{
      std::string{ "Cannot access function member: " }
      + member_key<MemberInfo>()
   };
}

template<typename ReturnType, typename MemberInfo, typename ClassInstanceData>
std::enable_if_t<
   !std::is_convertible_v<member_type_t<MemberInfo>, ReturnType>
   && !is_function_member_v<MemberInfo>,
   ReturnType
> get_member_ref(ClassInstanceData) {
   throw invalid_requested_member_type{
      std::string{ "Member type does not match requested type" }
      + member_key<MemberInfo>()
   };
}

template<
   typename ReturnType,
   typename MemberInfo,
   typename Class,
   bool is_null>
std::enable_if_t<
   is_null
   && !is_static_member_v<MemberInfo>
   && std::is_convertible_v<member_type_t<MemberInfo>, ReturnType>
   && !is_function_member_v<MemberInfo>,
   ReturnType
> get_member_ref(class_instance_data<Class, is_null>) {
   throw invalid_requested_member_type{
      std::string{ "Cannot access non-static member: " }
      + member_key<MemberInfo>()
   };
}

template<typename ReturnType, typename MemberInfo, typename ClassInstanceData>
std::enable_if_t<
   is_static_member_v<MemberInfo>
   && std::is_convertible_v<member_type_t<MemberInfo>, ReturnType>
   && !is_function_member_v<MemberInfo>,
   ReturnType
> get_member_ref(ClassInstanceData) {
   constexpr decltype(auto) member_ptr = get_member_ptr<MemberInfo>();
   return *member_ptr;
}

template<typename ReturnType, typename MemberInfo, typename Class, bool is_null>
std::enable_if_t<
   !is_null
   && !is_static_member_v<MemberInfo>
   && std::is_convertible_v<member_type_t<MemberInfo>, ReturnType>
   && !is_function_member_v<MemberInfo>,
   ReturnType
> get_member_ref(class_instance_data<Class, is_null> instance) {
   constexpr decltype(auto) member_ptr = get_member_ptr<MemberInfo>();
   return instance.data.*member_ptr;
}

template<typename Class, typename ReturnType, bool is_null = false>
struct get_member_ref_generator {
   using function_type = ReturnType(class_instance_data<Class, is_null>);

   template<typename MemberInfo>
   constexpr static function_type& create() noexcept {
      return get_member_ref<ReturnType, MemberInfo>;
   }
};

}
}
