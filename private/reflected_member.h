#pragma once

#include <utility>

#include "exceptions.h"
#include "member_assign.h"
#include "member_get.h"
#include "member_invoke.h"
#include "reflected_member_call.h"

namespace reflect {
namespace detail {

template<typename> struct class_reflection_info;

template<typename Class>
struct reflected_member {
   using ThisType = reflected_member<Class>;

   template<typename Type, typename = std::enable_if_t<!std::is_array_v<Type>>>
   operator Type() {
      return get<Type>(filter_convertible_types_t<Class, Type>());
   }

   template<typename Type, typename = std::enable_if_t<std::is_array_v<Type>>>
   operator Type&() {
      return get<Type&>(filter_convertible_types_t<Class, Type&>());
   }

   template<typename Type>
   void operator=(Type&& arg) {
      return assign(
         std::forward<Type>(arg),
         filter_assignable_types_t<Class, Type>()
      );
   }

   template<typename ReturnType, typename... Args>
   ReturnType invoke(Args&&... args) {
      return invoke_impl<ReturnType>(
         filter_convertible_types_t<Class, ReturnType(Args&&...)>(),
         std::forward<Args>(args)...
      );
   }

   template<typename... Args>
   reflected_member_call<Class, ThisType, Args&&...> operator()(Args&&... args) {
      return{ *this, std::forward<Args>(args)... };
   }

   virtual std::size_t get_type() const noexcept = 0;

protected:
   constexpr reflected_member() noexcept = default;

   reflected_member(const reflected_member&) = default;
   reflected_member(reflected_member&&) = default;

   reflected_member& operator=(reflected_member&) = default;
   reflected_member& operator=(reflected_member&&) = default;

private:
   template<typename Type>
   Type get(typelist<>) {
      throw invalid_requested_member_type{
         "Member type does not match requested type"
      };
   }

   template<
      typename Type,
      typename Option,
      typename... Options>
   Type get(typelist<Option, Options...>) {
      using TypeInfo = type_and_index_t<Class, Option>;
      static_assert(TypeInfo::value, "");

      if (TypeInfo::index == get_type()) {
         return static_cast<Type>(get_member_ref(
            static_cast<member<Class, TypeInfo::type>&>(*this)
         ));
      }

      return get<Type>(typelist<Options...>());
   }

   template<typename Type>
   void assign(Type&&, typelist<>) {
      throw invalid_assignment_type{
         "Attempting to assign wrong type to member"
      };
   }

   template<typename Type, typename Option, typename... Options>
   void assign(Type&& arg, typelist<Option, Options...>) {
      using TypeInfo = type_and_index_t<Class, Option>;
      static_assert(TypeInfo::value, "");

      if (TypeInfo::index == get_type()) {
         return assign_to_member(
            std::forward<Type>(arg),
            static_cast<member<Class, TypeInfo::type>&>(*this)
         );
      }

      return assign(std::forward<Type>(arg), typelist<Options...>());
   }

   template<typename ReturnType, typename... Args>
   ReturnType invoke_impl(typelist<>, Args&&...) {
      throw invalid_function_call{
         "No matching function for argument list"
      };
   }

   template<
      typename ReturnType,
      typename Option,
      typename... Options,
      typename... Args>
   ReturnType invoke_impl(
      typelist<Option, Options...>,
      Args&&... args)
   {
      using TypeInfo = type_and_index_t<Class, Option>;
      static_assert(TypeInfo::value, "");

      if (TypeInfo::index == get_type()) {
         return invoke_member<ReturnType>(
            static_cast<member<Class, TypeInfo::type>&>(*this),
            std::forward<Args>(args)...
         );
      }

      return invoke_impl<ReturnType>(
         typelist<Options...>(),
         std::forward<Args>(args)...
      );
   }
};

}
}
