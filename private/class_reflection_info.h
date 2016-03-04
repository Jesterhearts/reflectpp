#pragma once

#include "member_info.h"
#include "member_names.h"
#include "member_typemap.h"

namespace reflect {
namespace detail {

template<typename> struct class_reflection_info;

#define REFLECT_ENABLE(Class, ...)                      \
    REFLECT_CREATE_KEYS(Class, __VA_ARGS__)             \
                                                        \
    namespace reflect {                                 \
    namespace detail {                                  \
    template<>                                          \
    struct class_reflection_info<Class> {               \
        using TypeInfo = member_typemap<                \
            Class,                                      \
            REFLECT_LIST_MEMBERS(Class, __VA_ARGS__)    \
        >;                                              \
                                                        \
        static auto reflect(Class& instance) {          \
            return TypeInfo{ instance };                \
        }                                               \
                                                        \
        static auto reflect() {                         \
            return TypeInfo{};                          \
        }                                               \
    };                                                  \
    }                                                   \
    }

}
}
