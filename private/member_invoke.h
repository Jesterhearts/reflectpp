#pragma once

#include <string>

#include "member.h"
#include "utility/implicitly_equal_types.h"
#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<
   typename ReturnType,
   typename MemberInfo,
   typename Class,
   typename... Args>
std::enable_if_t<!is_function_member_v<MemberInfo>, ReturnType>
invoke_member(Class*, Args&&...) {
   throw invalid_function_call{
      std::string{ "Cannot call non-function member" }
      + member_key<MemberInfo>()
   };
}

template<
   typename ReturnType,
   typename MemberInfo,
   typename Class,
   typename... Args>
std::enable_if_t<
   !is_callable_v<MemberInfo, ReturnType(Args&&...)>
   && is_function_member_v<MemberInfo>,
   ReturnType
> invoke_member(Class*, Args&&...) {
   throw invalid_function_call{
      std::string{ "No matching function for argument list" }
      + member_key<MemberInfo>()
   };
}

template<
   typename ReturnType,
   typename MemberInfo,
   typename Class,
   typename... Args>
std::enable_if_t<
   is_static_member_v<MemberInfo>
   && is_callable_v<MemberInfo, ReturnType(Args&&...)>
   && is_function_member_v<MemberInfo>,
   ReturnType
> invoke_member(Class*, Args&&... args) {
   return static_cast<ReturnType>(
      (*get_member_ptr<MemberInfo>())(std::forward<Args>(args)...)
   );
}

template<
   typename ReturnType,
   typename MemberInfo,
   typename Class,
   typename... Args>
std::enable_if_t<
   !is_static_member_v<MemberInfo>
   && is_callable_v<MemberInfo, ReturnType(Args&&...)>
   && is_function_member_v<MemberInfo>,
   ReturnType
> invoke_member(Class* instance, Args&&... args) {
   if (instance) {
      constexpr decltype(auto) member_ptr = get_member_ptr<MemberInfo>();

      return static_cast<ReturnType>(
         (instance->*member_ptr)(std::forward<Args>(args)...)
      );
   }

   throw invalid_function_call{
      std::string{ "Cannot call non-static function: " }
      + member_key<MemberInfo>()
   };
}

template<typename, typename> struct invoke_member_generator;

template<typename Class, typename ReturnType, typename... Args>
struct invoke_member_generator<Class, ReturnType(Args...)> {
   using function_type = ReturnType(Class*, Args&&...);

   template<typename MemberInfo>
   constexpr static function_type* create() noexcept {
      return &invoke_member<ReturnType, MemberInfo, Class, Args...>;
   }
};

}
}