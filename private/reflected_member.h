#pragma once

#include <tuple>
#include <type_traits>

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
   constexpr reflected_member(std::size_t type, Class* instance)
      : this_type{ type },
      class_instance{ instance }
   {}

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
      return reflected_member_call<Class, Args&&...>{
         this_type,
         std::forward_as_tuple(std::forward<Args>(args)...),
         class_instance
      };
   }

   template<typename ReturnType = void, typename... Args>
   ReturnType invoke(Args&&... args) {
      return do_invoke<ReturnType>(
         this_type,
         std::forward_as_tuple(std::forward<Args>(args)...),
         class_instance
      );
   }

   template<typename Type>
   Type get() {
      using function_generator = get_member_ref_generator<Class, Type>;
      using function_table = function_table_t<function_generator, Class>;

      return function_table::functions[this_type](class_instance);
   }

   template<typename Type>
   void assign(Type&& arg) {
      using function_generator = assign_member_generator<Class, Type>;
      using function_table = function_table_t<function_generator, Class>;

      const auto type = this_type;
      return function_table::functions[this_type](
         std::forward<Type>(arg),
         class_instance
      );
   }

   constexpr std::size_t get_type() const noexcept {
      return this_type;
   }

private:
   Class *const class_instance;
   const std::size_t this_type;
};

}
}
