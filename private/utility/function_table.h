#pragma once

#include <utility>

#include "type_and_index.h"
#include "typelist.h"

namespace reflect {
namespace detail {

template<typename> struct reflected_member;

template<typename FunctionGenerator, typename... TypeInfo>
struct function_table {
   using function_type = typename FunctionGenerator::function_type;
   using table_entry_t = std::pair<std::size_t, function_type*>;

   constexpr static const std::size_t count = sizeof...(TypeInfo);
   constexpr static const table_entry_t entries[count]{
      { TypeInfo::index, FunctionGenerator::create<TypeInfo::type>() }...
   };

   constexpr static auto first_id = entries[0].first;
   constexpr static auto last_id = entries[count - 1].first;

private:
   constexpr static bool ids_ascend_only(std::size_t) {
      return true;
   }

   template<typename... Args>
   constexpr static bool ids_ascend_only(
      std::size_t id0,
      std::size_t id1,
      Args... args)
   {
      return id0 < id1 && ids_ascend_only(id1, args...);
   }

   constexpr static bool ids_ascend = ids_ascend_only(TypeInfo::index...);
   static_assert(ids_ascend, "");
};

template<typename FunctionGenerator, typename Class, typename... Members>
using function_table_t = function_table<
   FunctionGenerator,
   type_and_index_t<Class, Members>...
>;

}
}
