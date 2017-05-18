#pragma once

#include <tuple>
#include <type_traits>

#include "exceptions.h"
#include "member_assign.h"
#include "member_get.h"
#include "member_invoke.h"
#include "reflected_member_call.h"

#include "utility/class_instance_data.h"
#include "utility/function_table.h"

namespace reflect {
namespace detail {

template<typename Class, bool is_null>
struct reflected_member {
   constexpr reflected_member(
      std::size_t type,
      class_instance_data<Class, is_null> instance) noexcept
      : this_type{ type },
      instance{ instance }
   {}

   template<typename = std::enable_if_t<is_null>>
   constexpr reflected_member(std::size_t type) noexcept : this_type{ type } {}

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
   auto operator()(Args&&... args) {
      return reflected_member_call<Class, is_null, Args&&...>{
         this_type,
         std::forward_as_tuple(std::forward<Args>(args)...),
         instance
      };
   }

   template<typename ReturnType = void, typename... Args>
   ReturnType invoke(Args&&... args) {
      return do_invoke<ReturnType>(
         this_type,
         std::forward_as_tuple(std::forward<Args>(args)...),
         instance
      );
   }

   template<typename Type>
   Type get() {
      using function_generator = get_member_ref_generator<
         Class,
         Type,
         is_null
      >;
      using function_table = function_table_t<
         function_generator,
         Class,
         is_null
      >;

      return function_table::functions[this_type](instance);
   }

   template<typename Type>
   void assign(Type&& arg) {
      using function_generator = assign_member_generator<Class, Type, is_null>;
      using function_table = function_table_t<
         function_generator,
         Class,
         is_null
      >;

      const auto type = this_type;
      return function_table::functions[this_type](
         std::forward<Type>(arg),
         instance
      );
   }

   constexpr std::size_t get_type() const noexcept {
      return this_type;
   }

private:
   const std::size_t                   this_type;
   class_instance_data<Class, is_null> instance;
};

}
}
