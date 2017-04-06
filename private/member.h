#pragma once

#include <utility>

#include "member_assigner.h"
#include "reflected_instance.h"
#include "reflected_member.h"

#include "macros/member_info.h"
#include "macros/member_names.h"

namespace reflect {
namespace detail {

template<typename, typename, typename> struct member;

template<
   typename Class,
   typename TypeRepr,
   typename MemberType,
   MemberType Class::* Member>
   struct member<Class, member_info<MemberType Class::*, Member>, TypeRepr>
   : member_assigner<Class, reflected_member<Class, TypeRepr>, MemberType>
{
   using Type = MemberType;
   using Info = member_info<MemberType Class::*, Member>;

   Type& get() override {
      Class* instance = static_cast<reflected_instance<Class>*>(
         static_cast<typename class_reflection_info<Class>::TypeInfo*>(this)
      )->instance;

      if (instance) {
         return instance->*Member;
      }
      else {
         throw invalid_requested_member_type{
             std::string{"Cannot access non-static member: "}
             +member_name<Info>::key()
         };
      }
   }

   void operator=(const Type& type) {
      assign_impl(type, std::is_const<Type>{});
   }

   void operator=(Type&& type) {
      assign_impl(std::move(type), std::is_const<Type>{});
   }

   template<typename _Type>
   void assign_impl(_Type&& type, std::false_type) {
      Class* instance = static_cast<reflected_instance<Class>*>(
         static_cast<typename class_reflection_info<Class>::TypeInfo*>(this)
      )->instance;

      if (instance) {
         instance->*Member = std::forward<_Type>(type);
      }
      else {
         throw member_access_error{
             std::string{"Cannot assign to non-static member: "}
             +member_name<Info>::key()
         };
      }
   }

   template<typename _Type>
   void assign_impl(_Type&&, std::true_type) {
      throw invalid_assignment_to_const{
          std::string{"Cannot assign to const member: "}
          +member_name<Info>::key()
      };
   }
};

template<
   typename Class,
   typename TypeRepr,
   typename MemberType,
   MemberType* Member>
   struct member<Class, member_info<MemberType*, Member>, TypeRepr>
   : member_assigner<Class, reflected_member<Class, TypeRepr>, MemberType>
{
   using Type = MemberType;
   using Info = member_info<MemberType*, Member>;

   Type& get() override {
      return *Member;
   }

   void operator=(const Type& type) {
      assign_impl(type, std::is_const<Type>{});
   }

   void operator=(Type&& type) {
      assign_impl(std::move(type), std::is_const<Type>{});
   }

   template<typename _Type>
   void assign_impl(_Type&& type, std::false_type) {
      *Member = std::forward<_Type>(type);
   }

   template<typename _Type>
   void assign_impl(_Type&&, std::true_type) {
      throw invalid_assignment_to_const{
          std::string{"Cannot assign to const member: "}
          +member_name<Info>::key()
      };
   }
};

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
             std::string{"Cannot call non-static member function without an instance: "}
             +member_name<Info>::key()
         };
      }
   }
};

}
}
