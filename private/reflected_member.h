#pragma once

#include <utility>

#include "exceptions.h"
#include "member_assign.h"
#include "member_get.h"
#include "member_invoke.h"
#include "reflected_member_call.h"

#include "utility/function_table.h"

namespace reflect {
namespace detail {

template<typename> struct class_reflection_info;

template<typename Class>
struct reflected_member {
   using ThisType = reflected_member<Class>;

   template<typename Type, typename = std::enable_if_t<!std::is_array_v<Type>>>
   operator Type() {
      return get<Type>();
   }

   template<typename Type, typename = std::enable_if_t<std::is_array_v<Type>>>
   operator Type&() {
      return get<Type&>();
   }

   template<typename Type>
   void operator=(Type&& arg) {
      return assign(std::forward<Type>(arg));
   }

   template<typename... Args>
   reflected_member_call<Class, ThisType, Args&&...> operator()(Args&&... args) {
      return{ *this, std::forward<Args>(args)... };
   }

   template<typename ReturnType, typename... Args>
   ReturnType invoke(Args&&... args) {
      using options = filter_convertible_types_t<Class, ReturnType(Args&&...)>;
      using function_generator = invoke_member_generator<Class, ReturnType(Args&&...)>;
      using function_table = function_table_t<function_generator, Class, options>;

      const auto type = get_type();
      for (std::size_t i = 0; i < function_table::count; ++i) {
         if (type == function_table::id_at(i)) {
            return function_table::function_at(i)(*this, std::forward<Args>(args)...);
         }
      }

      throw invalid_function_call{
         "No matching function for argument list"
      };
   }

   template<typename Type>
   Type get() {
      using options = filter_convertible_types_t<Class, Type>;
      using function_generator = get_member_ref_generator<Class, Type>;
      using function_table = function_table_t<function_generator, Class, options>;

      const auto type = get_type();
      for (std::size_t i = 0; i < function_table::count; ++i) {
         if (type == function_table::id_at(i)) {
            return function_table::function_at(i)(*this);
         }
      }

      throw invalid_requested_member_type{
         "Member type does not match requested type"
      };
   }

   template<typename Type>
   void assign(Type&& arg) {
      using options = filter_assignable_types_t<Class, Type>;
      using function_generator = assign_member_generator<Class, Type>;
      using function_table = function_table_t<function_generator, Class, options>;

      const auto type = get_type();
      for (std::size_t i = 0; i < function_table::count; ++i) {
         if (type == function_table::id_at(i)) {
            return function_table::function_at(i)(*this, std::forward<Type>(arg));
         }
      }

      throw invalid_assignment_type{
         "Attempting to assign wrong type to member"
      };
   }

   virtual std::size_t get_type() const noexcept = 0;

protected:
   constexpr reflected_member() noexcept = default;

   reflected_member(const reflected_member&) = default;
   reflected_member(reflected_member&&) = default;

   reflected_member& operator=(reflected_member&) = default;
   reflected_member& operator=(reflected_member&&) = default;
};

}
}
