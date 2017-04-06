#pragma once

#include "private/exceptions.h"
#include "private/member_typemap.h"

#include "private/macros/class_reflection_info.h"

namespace reflect {
#define REFLECT_PRIVATE_MEMBERS()   \
    template<typename> friend struct reflect::detail::class_reflection_info;

template<typename Class>
auto reflect(Class& instance) {
    return detail::class_reflection_info<Class>::reflect(instance);
}

template<typename Class>
auto reflect() {
    return detail::class_reflection_info<Class>::reflect();
}

}
