#pragma once

#include <tuple>

#include "exceptions.h"

namespace reflect {
namespace detail {

template<typename Class, typename ReflectedType, typename... Args>
struct reflected_member_call {
   std::tuple<Args...> params;
   ReflectedType&      fnptr;
   bool                called;

   template<typename ReturnType>
   operator ReturnType() {
      called = true;
      return invoke<ReturnType>(std::index_sequence_for<Args>());
   }

   ~reflected_member_call() noexcept(false) {
      if (called) return;
      called = true;
      invoke<void>(std::index_sequence_for<Args>());
   }

private:
   template<typename> friend struct reflected_member;

   template<typename... _Args>
   reflected_member_call(ReflectedType& fnptr, _Args&&... args) :
      params{ std::forward<Args>(args)... },
      fnptr(fnptr),
      called(false)
   {}

   reflected_member_call(reflected_member_call&& other) :
      params(std::move(other.params)),
      fnptr(other.fnptr),
      called(false)
   {
      other.called = true;
   }

   template<typename ReturnType, std::size_t... ParamNum>
   ReturnType invoke(std::integer_sequence<std::size_t, ParamNum...>) {
      return fnptr.invoke<ReturnType>(
         std::get<ParamNum>(std::move(params))...
      );
   }

   reflected_member_call(const reflected_member_call&) = delete;
   reflected_member_call& operator=(const reflected_member_call&) = delete;
   reflected_member_call& operator=(reflected_member_call&&) = delete;
};

template<typename Class, typename ReflectedType>
struct reflected_member_call<Class, ReflectedType> {
   ReflectedType&      fnptr;
   bool                called;

   template<typename ReturnType>
   operator ReturnType() {
      called = true;
      return fnptr.invoke<ReturnType>();
   }

   ~reflected_member_call() noexcept(false) {
      if (called) return;
      called = true;
      fnptr.invoke<void>();
   }

private:
   template<typename> friend struct reflected_member;

   reflected_member_call(ReflectedType& fnptr) :
      fnptr(fnptr),
      called(false)
   {}

   reflected_member_call(reflected_member_call&& other) :
      fnptr(other.fnptr),
      called(false)
   {
      other.called = true;
   }

};

}
}
