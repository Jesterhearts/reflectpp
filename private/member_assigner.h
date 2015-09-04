#pragma once

#include "type_info.h"

namespace reflect {
namespace detail {

template<typename Class, typename Base, typename Type>
struct member_assigner : Base {
    member_assigner() : Base{ TypeInfo<Class, Type>::index } {}

    virtual operator Type&() = 0;
    virtual Type& operator=(const Type& type) = 0;
    virtual Type& operator=(Type& type) = 0;
    virtual Type& operator=(Type&& type) = 0;
};

}
}
