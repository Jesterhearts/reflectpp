#pragma once

#include "reflected_instance.h"
#include "reflected_member.h"

#include "utility/type_and_index.h"

namespace reflect {
namespace detail {

template<typename> struct class_reflection_info;

template<typename Class, typename MemberInfo>
struct member : reflected_member<Class> {
   Class* get_instance() {
      return static_cast<reflected_instance<Class>*>(
         static_cast<typename class_reflection_info<Class>::member_map*>(this)
      )->instance;
   }

   std::size_t get_type() const noexcept override final {
      return type_and_index_t<Class, MemberInfo>::index;
   }
};

}
}
