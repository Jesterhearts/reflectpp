#pragma once

#include <utility>

#include "member_assigner.h"
#include "member_info.h"
#include "reflected_instance.h"
#include "reflected_member.h"

namespace reflect {
namespace detail {

template<typename, typename> struct member;

template<
    typename TypeRepr,
    typename Class,
    typename MemberType,
    MemberType Class::* Member>
struct member<member_info<MemberType Class::*, Member>, TypeRepr>
    : member_assigner<Class, reflected_member<Class, TypeRepr>, MemberType>
{
    using Type = MemberType;

    operator Type&() {
        return static_cast<reflected_instance<Class>*>(
            static_cast<typename class_reflection_info<Class>::TypeInfo*>(this)
        )->instance.*Member;
    }

    Type& operator=(const Type& type) override {
        return static_cast<reflected_instance<Class>*>(
            static_cast<typename class_reflection_info<Class>::TypeInfo*>(this)
        )->instance.*Member = type;
    }

    Type& operator=(Type& type) override {
        return static_cast<reflected_instance<Class>*>(
            static_cast<typename class_reflection_info<Class>::TypeInfo*>(this)
        )->instance.*Member = type;
    }

    Type& operator=(Type&& type) override {
        return static_cast<reflected_instance<Class>*>(
            static_cast<typename class_reflection_info<Class>::TypeInfo*>(this)
        )->instance.*Member = type;
    }
};

template<
    typename TypeRepr,
    typename Class,
    typename ReturnType,
    typename... Args,
    ReturnType(Class::* MemberFn)(Args...)>
struct member<member_info<ReturnType(Class::*)(Args...), MemberFn>, TypeRepr>
    : member_invoker<Class, reflected_member<Class, TypeRepr>, ReturnType, Args...>
{
    using Type = ReturnType(Args...);

    ReturnType operator()(Args... args) override {
        return (static_cast<reflected_instance<Class>*>(
            static_cast<typename class_reflection_info<Class>::TypeInfo*>(this)
        )->instance.*MemberFn)(std::forward<Args>(args)...);
    }
};

}
}
