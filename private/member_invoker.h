#pragma once

#include "type_info.h"

namespace reflect {
namespace detail {

template<typename Class, typename Base, typename ReturnType, typename... Args>
struct member_invoker : Base {
    using Type = ReturnType(Args...);

    member_invoker() : Base{ TypeInfo<Class, Type>::index } {}

    virtual ReturnType operator()(Args... args) = 0;
};

}
}
