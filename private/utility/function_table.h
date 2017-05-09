#pragma once

#include <utility>

#include "type_and_index.h"
#include "typelist.h"

namespace reflect {
namespace detail {

template<typename> struct reflected_member;

template<typename FunctionGenerator, typename, typename... TypeInfo>
struct function_table {
   using function_type = typename FunctionGenerator::function_type;

   constexpr static auto count = sizeof...(TypeInfo);

   constexpr static const std::size_t    ids[count]{ TypeInfo::index... };
   constexpr static function_type *const functions[count]{
      FunctionGenerator::create<TypeInfo::type>()...
   };

   constexpr static auto first_id = ids[0];
   constexpr static auto last_id = ids[count - 1];

   constexpr static std::size_t id_at(std::size_t index) {
      return ids[index];
   }

   constexpr static function_type* function_at(std::size_t index) {
      return functions[index];
   }

private:
   constexpr static bool ids_ascend_only(std::size_t) {
      return true;
   }

   template<typename... Args>
   constexpr static bool ids_ascend_only(std::size_t id0, std::size_t id1, Args... args) {
      return id0 < id1 && ids_ascend_only(id1, args...);
   }

   constexpr static bool ids_ascend = ids_ascend_only(TypeInfo::index...);
   static_assert(ids_ascend, "");
};

template<typename FunctionGenerator, typename Class>
struct function_table<FunctionGenerator, Class, typelist<>> {
   using function_type = typename FunctionGenerator::function_type;

   constexpr static auto count = 0;
   constexpr static auto first_id = 0;
   constexpr static auto last_id = 0;

   constexpr static std::size_t id_at(std::size_t index) {
      return 0;
   }

   constexpr static function_type* function_at(std::size_t index) {
      return nullptr;
   }
};

template<typename FunctionGenerator, typename Class, typename... Members>
struct function_table<FunctionGenerator, Class, typelist<Members...>>
   : function_table<FunctionGenerator, Class, type_and_index_t<Class, Members>...>
{
};

template<typename FunctionGenerator, typename Class, typename MemberList>
using function_table_t = function_table<FunctionGenerator, Class, MemberList>;

}
}
