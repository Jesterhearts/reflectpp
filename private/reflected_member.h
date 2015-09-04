#pragma once

#include <utility>

#include "exceptions.h"
#include "member_assigner.h"
#include "member_invoker.h"
#include "reflected_member_call.h"

namespace reflect {
namespace detail {

template<typename Class, typename TypeRepr>
struct reflected_member {
    using ThisType = reflected_member<Class, TypeRepr>;
    const TypeRepr tag;

    template<typename Type>
    operator Type&() {
        using TypeInfo = TypeInfo<Class, Type>;
        static_assert(
            TypeInfo::value,
            "Requested type is not a member type of this class"
        );

        if (TypeInfo::index != tag) {
            throw bad_member_access{
                "Attempting to read from wrong member type"
            };
        }

        return *static_cast<member_assigner<Class, ThisType, Type>*>(this);
    }

    template<typename Type>
    Type& operator=(Type&& value) {
        using TypeInfo = TypeInfo<Class, Type>;
        static_assert(
            TypeInfo::value,
            "Requested type is not a member type of this class"
        );

        if (TypeInfo::index != tag) {
            throw bad_member_access{
                "Attempting to assign wrong type to member"
            };
        }

        return *static_cast<member_assigner<Class, ThisType, Type>*>(
            this
        ) = value;
    }

    template<typename ReturnType, typename... Args>
    ReturnType invoke(Args&&... args) {
        using Type = ReturnType(*)(Args...);
        using TypeInfo = TypeInfo<Class, Type>;
        static_assert(
            TypeInfo::value,
            "Requested type is not a member type of this class"
        );

        if (TypeInfo::index != tag) {
            throw bad_member_access{
                "Attempting to call nonexistent function"
            };
        }

        return static_cast<
            member_invoker<Class, ThisType, ReturnType, Args...>&
        >(*this)(std::forward<Args>(args)...);
    }

    template<typename... Args>
    reflected_member_call<Class, ThisType, Args...> operator()(Args... args) {
        return reflected_member_call<Class, ThisType, Args...>{
            this,
            std::forward<Args>(args)...
        };
    }

private:
    constexpr reflected_member(const reflected_member&) = default;
    constexpr reflected_member(reflected_member&&)      = default;

    template<typename, typename, typename>
    friend struct member_assigner;

    template<typename, typename, typename, typename...>
    friend struct member_invoker;
;
};

}
}
