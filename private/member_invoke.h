#pragma once

#include <string>

#include "member.h"
#include "utility/implicitly_equal_types.h"
#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<
   typename ReturnType,
   typename ReflectedMemberType,
   typename Class,
   typename... Args>
std::enable_if_t<
   !is_function_member_v<ReflectedMemberType>,
   ReturnType
> invoke_member(Class*, Args&&...) {
   throw invalid_function_call{
      std::string{ "Cannot call non-function member" }
      + member_key<ReflectedMemberType>()
   };
}

template<
   typename ReturnType,
   typename ReflectedMemberType,
   typename Class,
   typename... Args>
std::enable_if_t<
   is_function_member_v<ReflectedMemberType>
   && !implicitly_equal_v<
      ReflectedMemberType,
      ReturnType(Args&&...)
   >,
   ReturnType
> invoke_member(Class*, Args&&...) {
   throw invalid_function_call{
      std::string{ "No matching function for argument list" }
      + member_key<ReflectedMemberType>()
   };
}

template<
   typename ReturnType,
   typename ReflectedMemberType,
   typename Class,
   typename... Args>
std::enable_if_t<
   is_function_member_v<ReflectedMemberType>
   && is_static_member_v<ReflectedMemberType>
   && implicitly_equal_v<
      ReflectedMemberType,
      ReturnType(Args&&...)
   >,
   ReturnType
> invoke_member(Class*, Args&&... args) {
   return static_cast<ReturnType>(
      (*get_member_ptr<ReflectedMemberType>())(std::forward<Args>(args)...)
   );
}

template<
   typename ReturnType,
   typename ReflectedMemberType,
   typename Class,
   typename... Args>
std::enable_if_t<
   is_function_member_v<ReflectedMemberType>
   && !is_static_member_v<ReflectedMemberType>
   && implicitly_equal_v<
      ReflectedMemberType,
      ReturnType(Args&&...)
   >,
   ReturnType
> invoke_member(Class* instance, Args&&... args) {
   if (instance) {
      constexpr decltype(auto) member_ptr = get_member_ptr<
         ReflectedMemberType
      >();

      return static_cast<ReturnType>(
         (instance->*member_ptr)(std::forward<Args>(args)...)
      );
   }

   throw invalid_function_call{
      std::string{ "Cannot call non-static function: " }
      + member_key<ReflectedMemberType>()
   };
}

template<typename, typename> struct invoke_member_generator;

template<typename Class, typename ReturnType, typename... Args>
struct invoke_member_generator<Class, ReturnType(Args...)> {
   using function_type = ReturnType(Class*, Args&&...);

   template<typename MemberType>
   constexpr static function_type* create() noexcept {
      return &invoke_member<ReturnType, MemberType, Class, Args...>;
   }
};

}
}