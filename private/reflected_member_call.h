#pragma once

#include <tuple>

#include "class_reflection_info.h"
#include "exceptions.h"
#include "filter_types.h"
#include "implicitly_equal_types.h"
#include "member_invoker.h"
#include "number_list.h"

namespace reflect {
namespace detail {

template<typename Class, typename ReflectedType, typename... Args>
struct reflected_member_call {
    std::tuple<Args...> params;
    ReflectedType*      fnptr;
    bool                called;

    template<typename... _Args>
    reflected_member_call(ReflectedType* fnptr, _Args&&... args) :
        params{std::forward<Args>(args)...},
        fnptr(fnptr),
        called(false)
    {}

    template<typename ReturnType>
    operator ReturnType() {
        called = true;
        return invoke<ReturnType>(number_list_t<sizeof...(Args)>());
    }

    ~reflected_member_call() noexcept(false) {
        if(called) return;
        called = true;
        invoke<void>(number_list_t<sizeof...(Args)>());
    }

private:
    template<typename, typename> friend struct reflected_member;

    reflected_member_call(reflected_member_call&& other) :
        params(std::move(other.params)),
        fnptr(other.fnptr),
        called(false)
    {
        other.called = true;
    }

    template<typename ReturnType, size_t... ParamNum>
    ReturnType invoke(number_list<ParamNum...>) {
        return fnptr->invoke<ReturnType>(
            std::get<ParamNum>(std::move(params))...
        );
    }

    reflected_member_call(const reflected_member_call&)            = delete;
    reflected_member_call& operator=(const reflected_member_call&) = delete;
    reflected_member_call& operator=(reflected_member_call&&)      = delete;
};

template<typename Class, typename ReflectedType>
struct reflected_member_call<Class, ReflectedType> {
    ReflectedType*      fnptr;
    bool                called;

    reflected_member_call(ReflectedType* fnptr) :
        fnptr(fnptr),
        called(false)
    {}

    reflected_member_call(reflected_member_call&& other) :
        fnptr(other.fnptr),
        called(true)
    {}

    template<typename ReturnType>
    operator ReturnType() {
        called = true;
        return invoke<ReturnType>();
    }

    ~reflected_member_call() noexcept(false) {
        if (called) return;
        called = true;
        invoke<void>();
    }

private:
    template<typename ReturnType>
    ReturnType invoke() {
        return fnptr->invoke<ReturnType>();
    }
};

}
}
