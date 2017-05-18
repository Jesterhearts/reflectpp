#pragma once

#include "private/exceptions.h"
#include "private/member_typemap.h"

#include "private/macros/class_reflection_info.h"

namespace reflect {
#define REFLECT_PRIVATE_MEMBERS()   \
   template<typename> friend struct reflect::detail::reflected_class;

template<typename Class>
auto reflect(Class& instance) {
   return detail::member_map<Class>{ instance };
}

template<typename Class>
auto reflect() {
   return detail::member_map<Class>{};
}

}
