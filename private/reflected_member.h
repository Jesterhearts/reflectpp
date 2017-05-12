#pragma once

#include <tuple>
#include <type_traits>
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

   template<typename MemberType>
   constexpr reflected_member(MemberType& member)
      : this_type{ type_and_index_t<Class, MemberType>::index },
      class_instance{ class_instance_for(member) }
   {
   }

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
   reflected_member_call<Class, ThisType, Args&&...>
   operator()(Args&&... args) {
      return{ *this, std::forward<Args>(args)... };
   }

   template<typename ReturnType, typename... Args>
   ReturnType invoke(Args&&... args) {
      return invoke<ReturnType>(std::forward_as_tuple(
         std::forward<Args>(args)...
      ));
   }

   template<typename ReturnType, typename... Args>
   ReturnType invoke(std::tuple<Args...>&& args) {
      using function_generator = invoke_member_generator<
         Class, ReturnType(Args...), std::index_sequence_for<Args...>
      >;
      using function_table = function_table_t<function_generator, Class>;

      return function_table::functions[this_type](
         std::move(args),
         class_instance,
         std::index_sequence_for<Args...>{}
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
         class_instance,
         std::forward<Type>(arg)
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
