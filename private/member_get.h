#pragma once

#include <string>

#include "member.h"
#include "utility/implicitly_equal_types.h"
#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<typename ReturnType, typename MemberInfo, typename Class>
std::enable_if_t<is_function_member_v<MemberInfo>, ReturnType>
get_member_ref(Class*) {
   throw invalid_requested_member_type{
      std::string{ "Cannot access function member: " }
      + member_key<MemberInfo>()
   };
}

template<typename ReturnType, typename MemberInfo, typename Class>
std::enable_if_t<
   !std::is_convertible_v<member_type_t<MemberInfo>, ReturnType>
   && !is_function_member_v<MemberInfo>,
   ReturnType>
get_member_ref(Class*) {
   throw invalid_requested_member_type{
      std::string{ "Member type does not match requested type" }
      + member_key<MemberInfo>()
   };
}

template<typename ReturnType, typename MemberInfo, typename Class>
std::enable_if_t<
   is_static_member_v<MemberInfo>
   && std::is_convertible_v<member_type_t<MemberInfo>, ReturnType>
   && !is_function_member_v<MemberInfo>,
   ReturnType
> get_member_ref(Class*) {
   constexpr decltype(auto) member_ptr = get_member_ptr<MemberInfo>();
   return *member_ptr;
}

template<typename ReturnType, typename MemberInfo, typename Class>
std::enable_if_t<
   !is_static_member_v<MemberInfo>
   && std::is_convertible_v<member_type_t<MemberInfo>, ReturnType>
   && !is_function_member_v<MemberInfo>,
   ReturnType
> get_member_ref(Class* instance) {
   if (instance) {
      constexpr decltype(auto) member_ptr = get_member_ptr<MemberInfo>();
      return instance->*member_ptr;
   }

   throw invalid_requested_member_type{
      std::string{ "Cannot access non-static member: " }
      + member_key<MemberInfo>()
   };
}

template<typename Class, typename ReturnType>
struct get_member_ref_generator {
   using function_type = ReturnType(Class*);

   template<typename MemberInfo>
   constexpr static function_type* create() noexcept {
      return &get_member_ref<ReturnType, MemberInfo, Class>;
   }
};

}
}
