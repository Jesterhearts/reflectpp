#pragma once

#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include "member.h"
#include "utility/implicitly_equal_types.h"
#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<
   typename ReturnType,
   typename MemberInfo,
   typename Class,
   typename... Args,
   std::size_t... Indexes>
std::enable_if_t<!is_function_member_v<MemberInfo>, ReturnType>
invoke_member(
   std::tuple<Args...>&&,
   Class*,
   std::index_sequence<Indexes...>)
{
   throw invalid_call_to_non_callable_member{
      std::string{ "Cannot call non-function member" }
      + member_key<MemberInfo>()
   };
}

template<
   typename ReturnType,
   typename MemberInfo,
   typename Class,
   typename... Args,
   std::size_t... Indexes>
std::enable_if_t<
   !is_callable_v<MemberInfo, ReturnType(Args...)>
   && is_function_member_v<MemberInfo>,
   ReturnType
> invoke_member(
   std::tuple<Args...>&&,
   Class*,
   std::index_sequence<Indexes...>)
{
   throw invalid_function_arguments{
      std::string{ "No matching function for argument list" }
      + member_key<MemberInfo>()
   };
}

template<
   typename ReturnType,
   typename MemberInfo,
   typename Class,
   typename... Args,
   std::size_t... Indexes>
std::enable_if_t<
   is_static_member_v<MemberInfo>
   && is_callable_v<MemberInfo, ReturnType(Args...)>
   && is_function_member_v<MemberInfo>,
   ReturnType
> invoke_member(
   std::tuple<Args...>&& args,
   Class*,
   std::index_sequence<Indexes...>)
{
   constexpr decltype(auto) member_ptr = get_member_ptr<MemberInfo>();
   return static_cast<ReturnType>(
      (member_ptr)(std::get<Indexes>(std::move(args))...)
   );
}

template<
   typename ReturnType,
   typename MemberInfo,
   typename Class,
   typename... Args,
   std::size_t... Indexes>
std::enable_if_t<
   !is_static_member_v<MemberInfo>
   && is_callable_v<MemberInfo, ReturnType(Args...)>
   && is_function_member_v<MemberInfo>,
   ReturnType
> invoke_member(
   std::tuple<Args...>&& args,
   Class* instance,
   std::index_sequence<Indexes...>)
{
   if (instance) {
      constexpr decltype(auto) member_ptr = get_member_ptr<MemberInfo>();

      return static_cast<ReturnType>(
         (instance->*member_ptr)(std::get<Indexes>(std::move(args))...)
      );
   }

   throw invalid_non_static_function_call{
      std::string{ "Cannot call non-static function: " }
      + member_key<MemberInfo>()
   };
}

template<typename, typename, typename> struct invoke_member_generator;

template<
   typename Class,
   typename ReturnType,
   typename... Args,
   std::size_t... Indexes>
struct invoke_member_generator<
   Class,
   ReturnType(Args...),
   std::index_sequence<Indexes...>>
{
   using function_type = ReturnType(
      std::tuple<Args...>&&,
      Class*,
      std::index_sequence<Indexes...>
   );

   template<typename MemberInfo>
   constexpr static function_type* create() noexcept {
      return &invoke_member<ReturnType, MemberInfo>;
   }
};

template<typename ReturnType, typename Class, typename... Args>
ReturnType do_invoke(
   std::tuple<Args...>&& args,
   Class* class_instance,
   std::size_t type)
{
   using function_generator = invoke_member_generator<
      Class, ReturnType(Args...), std::index_sequence_for<Args...>
   >;
   using function_table = function_table_t<function_generator, Class>;

   return function_table::functions[type](
      std::move(args),
      class_instance,
      std::index_sequence_for<Args...>{}
   );
}

}
}
