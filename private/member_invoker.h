#pragma once

#include <string>
#include <type_traits>

#include "exceptions.h"
#include "reflected_instance.h"
#include "reflected_member.h"
#include "member_invoker_base.h"

#include "macros/member_info.h"
#include "macros/member_names.h"

#include "utility/type_info.h"

namespace reflect {
namespace detail {

template<
   typename Class,
   typename TypeRepr,
   typename ReturnType,
   typename... Args,
   ReturnType(Class::* MemberFn)(Args...)>
struct member<Class, member_info<ReturnType(Class::*)(Args...), MemberFn>, TypeRepr>
   : member_invoker<Class, reflected_member<Class, TypeRepr>, ReturnType, Args...>
{
   using Type = ReturnType(Args...);
   using Info = member_info<ReturnType(Class::*)(Args...), MemberFn>;

   ReturnType operator()(Args... args) override {
      Class* instance = static_cast<reflected_instance<Class>*>(
         static_cast<typename class_reflection_info<Class>::TypeInfo*>(this)
      )->instance;

      if (instance) {
         return (instance->*MemberFn)(std::forward<Args>(args)...);
      }
      else {
         throw invalid_function_call{
             std::string{ "Cannot call non-static member function without an instance: " }
             + member_name<Info>::key()
         };
      }
   }
};

}
}
