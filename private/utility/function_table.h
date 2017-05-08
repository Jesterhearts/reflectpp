#pragma once

#include <utility>

#include "type_and_index.h"

namespace reflect {
namespace detail {

template<typename> struct reflected_member;

template<typename FunctionType, typename Class, typename... TypeInfo>
struct function_table;

template<typename ReturnType, typename... Args, typename Class, typename... TypeInfo>
struct function_table<ReturnType(Args...), Class, TypeInfo...> {
   using function_type = ReturnType(reflected_member<Class>&, Args&&...);
   using table_entry_t = std::pair<std::size_t, function_type*>;

   constexpr static const std::size_t count = sizeof...(TypeInfo);
   constexpr static const table_entry_t entries[count]{
      { TypeInfo::index, (&invoke_member<ReturnType, TypeInfo::type, Class, Args...>) }...
   };

   constexpr static auto first_id = entries[0].first;
   constexpr static auto last_id = entries[count - 1].first;

private:
   template<std::size_t Item0>
   constexpr static bool ids_ascend_only(std::index_sequence<Item0>) {
      return true;
   }

   template<std::size_t Item0, std::size_t Item1, std::size_t... Items>
   constexpr static bool ids_ascend_only(std::index_sequence<Item0, Item1, Items...>) {
      return Item0 < Item1 && ids_ascend_only(std::index_sequence<Item1, Items...>{});
   }

   static_assert(ids_ascend_only(std::index_sequence<TypeInfo::index...>{}), "");
};

template<typename FunctionType, typename Class, typename... Members>
using function_table_t = function_table<FunctionType, Class, type_and_index_t<Class, Members>...>;


}
}
