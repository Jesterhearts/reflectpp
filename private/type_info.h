#pragma once

#include "find_duplicate_type.h"

namespace reflect {
namespace detail {

template<typename> struct class_reflection_info;

//TODO a bug in GCC prevents class_reflection_info<Class>::TypeInfo::get_type_id<T> from
//  working in some cases, this is a workaround
template<typename Class, typename type>
using TypeInfo = find_duplicate_type<
    type,
    typename class_reflection_info<Class>::TypeInfo::Types
>;

}
}

