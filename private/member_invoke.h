#pragma once

#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include "utility/class_instance_data.h"
#include "utility/implicitly_equal_types.h"
#include "utility/query_member_info.h"

namespace reflect {
namespace detail {

template<
   typename ReturnType,
   typename MemberInfo,
   typename ClassInstanceData,
   typename... Args,
   std::size_t... Indexes>
std::enable_if_t<!is_function_member_v<MemberInfo>, ReturnType>
invoke_member(
   std::tuple<Args...>&&,
   ClassInstanceData,
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
   typename ClassInstanceData,
   typename... Args,
   std::size_t... Indexes>
std::enable_if_t<
   !is_callable_v<MemberInfo, ReturnType(Args...)>
   && is_function_member_v<MemberInfo>,
   ReturnType
> invoke_member(
   std::tuple<Args...>&&,
   ClassInstanceData,
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
   bool is_null,
   typename... Args,
   std::size_t... Indexes>
std::enable_if_t<
   is_null
   && !is_static_member_v<MemberInfo>
   && is_callable_v<MemberInfo, ReturnType(Args...)>
   && is_function_member_v<MemberInfo>,
   ReturnType
> invoke_member(
   std::tuple<Args...>&&,
   class_instance_data<Class, is_null>,
   std::index_sequence<Indexes...>)
{
   throw invalid_non_static_function_call{
      std::string{ "Cannot call non-static function: " }
      + member_key<MemberInfo>()
   };
}

template<
   typename ReturnType,
   typename MemberInfo,
   typename ClassInstanceData,
   typename... Args,
   std::size_t... Indexes>
std::enable_if_t<
   is_static_member_v<MemberInfo>
   && is_callable_v<MemberInfo, ReturnType(Args...)>
   && is_function_member_v<MemberInfo>,
   ReturnType
> invoke_member(
   std::tuple<Args...>&& args,
   ClassInstanceData,
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
   bool is_null,
   typename... Args,
   std::size_t... Indexes>
std::enable_if_t<
   !is_null
   && !is_static_member_v<MemberInfo>
   && is_callable_v<MemberInfo, ReturnType(Args...)>
   && is_function_member_v<MemberInfo>,
   ReturnType
> invoke_member(
   std::tuple<Args...>&& args,
   class_instance_data<Class, is_null> instance,
   std::index_sequence<Indexes...>)
{
   constexpr decltype(auto) member_ptr = get_member_ptr<MemberInfo>();
   return static_cast<ReturnType>(
      (instance.data.*member_ptr)(std::get<Indexes>(std::move(args))...)
   );
}

template<typename, bool, typename> struct invoke_member_generator;

template<typename Class, bool is_null, typename ReturnType, typename... Args>
struct invoke_member_generator<Class, is_null, ReturnType(Args...)> {
   using function_type = ReturnType(
      std::tuple<Args...>&&,
      class_instance_data<Class, is_null>,
      std::index_sequence_for<Args...>
   );

   template<typename MemberInfo>
   constexpr static function_type& create() noexcept {
      return invoke_member<ReturnType, MemberInfo>;
   }
};

template<
   typename ReturnType = void,
   typename Class,
   bool is_null,
   typename... Args>
ReturnType do_invoke(
   std::size_t type,
   std::tuple<Args...>&& args,
   class_instance_data<Class, is_null> class_instance)
{
   using function_generator = invoke_member_generator<
      Class,
      is_null,
      ReturnType(Args...)
   >;
   using function_table = function_table_t<function_generator, Class, is_null>;

   return function_table::functions[type](
      std::move(args),
      class_instance,
      std::index_sequence_for<Args...>{}
   );
}

}
}
