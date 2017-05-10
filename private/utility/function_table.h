#pragma once

#include "type_and_index.h"
#include "typelist.h"

namespace reflect {
namespace detail {

template<typename> struct class_reflection_info;

template<typename FunctionGenerator, typename, typename... TypeInfo>
struct function_table {
   using function_type = typename FunctionGenerator::function_type;

   constexpr static auto count = sizeof...(TypeInfo);
   constexpr static function_type *const functions[count]{
      FunctionGenerator::create<TypeInfo::type>()...
   };
};

template<typename FunctionGenerator, typename Class, typename... Members>
struct function_table<FunctionGenerator, Class, typelist<Members...>>
   : function_table<
      FunctionGenerator,
      Class,
      type_and_index_t<Class, Members>...
   >
{
};

template<typename FunctionGenerator, typename Class>
using function_table_t = function_table<
   FunctionGenerator,
   Class,
   typename class_reflection_info<Class>::member_map::Members
>;

}
}
