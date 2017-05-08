#pragma once

#include <utility>

#include "type_and_index.h"
#include "typelist.h"

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
   constexpr static bool ids_ascend_only(std::size_t) {
      return true;
   }

   template<typename Arg0, typename Arg1, typename... Args>
   constexpr static bool ids_ascend_only(Arg0 arg0, Arg1 arg1, Args... args) {
      return arg0 < arg1 && ids_ascend_only(arg1, args...);
   }

   constexpr static bool ids_ascend = ids_ascend_only(TypeInfo::index...);
   static_assert(ids_ascend, "");
};

template<typename FunctionType, typename Class, typename... Members>
using function_table_t = function_table<FunctionType, Class, type_and_index_t<Class, Members>...>;

}
}
