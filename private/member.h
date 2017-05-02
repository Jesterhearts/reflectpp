#pragma once

#include "reflected_instance.h"
#include "reflected_member.h"

#include "utility/type_and_index.h"

namespace reflect {
namespace detail {

template<typename> struct class_reflection_info;

template<typename Class, typename MemberInfo>
struct member : reflected_member<Class> {

   std::size_t get_type() const noexcept override {
      return type_and_index_t<Class, MemberInfo>::index;
   }

};

}
}
