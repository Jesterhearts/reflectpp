#pragma once

#include "type_info.h"

namespace reflect {
namespace detail {

template<typename Class, typename Base, typename Type, typename = void>
struct member_assigner : Base {
    member_assigner() : Base{ TypeInfo<Class, Type>::index } {}
    virtual Type& get() = 0;
};

template<typename Class, typename Base, typename Type>
struct member_assigner<
    Class,
    Base,
    Type,
    std::enable_if_t<
        (std::is_copy_assignable<Type>::value
            || std::is_copy_constructible<Type>::value)
        && (std::is_move_assignable<Type>::value
            || std::is_move_constructible<Type>::value)>
> : Base {
    member_assigner() : Base{ TypeInfo<Class, Type>::index } {}

    virtual Type& get() = 0;
    virtual void operator=(const Type& type) = 0;
    virtual void operator=(Type&& type) = 0;
};

template<typename Class, typename Base, typename Type>
struct member_assigner<
    Class,
    Base,
    Type,
    std::enable_if_t<
        (std::is_copy_assignable<Type>::value
            || std::is_copy_constructible<Type>::value)
        && (!std::is_move_assignable<Type>::value
            && !std::is_move_constructible<Type>::value)>
> : Base {
    member_assigner() : Base{ TypeInfo<Class, Type>::index } {}

    virtual Type& get() = 0;
    virtual void operator=(const Type& type) = 0;
};

template<typename Class, typename Base, typename Type>
struct member_assigner<
    Class,
    Base,
    Type,
    std::enable_if_t<
        (!std::is_copy_assignable<Type>::value
            && !std::is_copy_constructible<Type>::value)
        && (std::is_move_assignable<Type>::value
            || std::is_move_constructible<Type>::value)>
> : Base {
    member_assigner() : Base{ TypeInfo<Class, Type>::index } {}

    virtual Type& get() = 0;
    virtual void operator=(Type&& type) = 0;
};

}
}

