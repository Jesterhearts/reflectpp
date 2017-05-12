#pragma once

#include "type_and_index.h"
#include "typelist.h"

namespace reflect {
namespace detail {

template<typename> struct class_reflection_info;

template<typename, typename, typename> struct function_table;

template<typename FunctionGenerator, typename Class, typename... Members>
struct function_table<FunctionGenerator, Class, typelist<Members...>> {
   using function_type = typename FunctionGenerator::function_type;

   constexpr static auto count = sizeof...(Members);
   constexpr static function_type *const functions[count]{
      FunctionGenerator::create<Members>()...
   };
};

template<typename FunctionGenerator, typename Class>
using function_table_t = function_table<
   FunctionGenerator,
   Class,
   typename class_reflection_info<Class>::member_map::Members
>;

}
}
