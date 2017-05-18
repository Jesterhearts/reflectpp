#pragma once

#include "typelist.h"

namespace reflect {
namespace detail {

template<typename, bool> struct member_map;

template<typename, typename, typename> struct function_table;

template<typename FunctionGenerator, typename Class, typename... Members>
struct function_table<FunctionGenerator, Class, typelist<Members...>> {
   using function_type = typename FunctionGenerator::function_type;

   constexpr static auto count = sizeof...(Members);
   constexpr static function_type *const functions[count]{
      FunctionGenerator::create<Members>()...
   };
};

template<typename FunctionGenerator, typename Class, bool is_null>
using function_table_t = function_table<
   FunctionGenerator,
   Class,
   typename member_map<Class, is_null>::members
>;

}
}
