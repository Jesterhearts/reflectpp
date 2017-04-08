#pragma once

#include "find_duplicate_type.h"

namespace reflect {
namespace detail {

template<typename> struct class_reflection_info;

template<typename Class, typename Type>
constexpr auto get_type_info() noexcept {
   return class_reflection_info<Class>::member_map::get_type_info<Type>();
}

template<typename Class>
using FnTypes = typename class_reflection_info<Class>::member_map::FnTypes;

template<typename Class>
using ObjTypes = typename class_reflection_info<Class>::member_map::ObjTypes;

}
}

