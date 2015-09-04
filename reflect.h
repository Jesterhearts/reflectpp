#pragma once

#include "private/exceptions.h"
#include "private/class_reflection_info.h"
#include "private/member_typemap.h"

namespace reflect {
template<typename Class>
auto reflect(Class& instance) {
    return detail::class_reflection_info<Class>::reflect(instance);
}
}
