#pragma once

#include <tuple>

#include "exceptions.h"
#include "member_invoke.h"

namespace reflect {
namespace detail {

template<typename Class, typename... Args>
struct reflected_member_call {

   template<typename ReturnType>
   operator ReturnType() {
      called = true;
      return do_invoke<ReturnType>(
         std::move(params),
         class_instance,
         this_type
      );
   }

   ~reflected_member_call() noexcept(false) {
      if (called) return;
      called = true;
      do_invoke<void>(
         std::move(params),
         class_instance,
         this_type
      );
   }

private:
   template<typename> friend struct reflected_member;

   Class *const        class_instance;
   const std::size_t   this_type;
   std::tuple<Args...> params;
   bool                called;

   reflected_member_call(
      Class* instance,
      std::size_t type,
      std::tuple<Args...>&& args
   ) : class_instance{ instance },
      this_type{ type },
      params{ std::move(args) },
      called(false)
   {}

   reflected_member_call(reflected_member_call&& other) :
      class_instance{ other.instance },
      this_type{ other.this_type },
      params{ std::move(other.params) },
      called(false)
   {
      other.called = true;
   }

   reflected_member_call(const reflected_member_call&) = delete;
   reflected_member_call& operator=(const reflected_member_call&) = delete;
   reflected_member_call& operator=(reflected_member_call&&) = delete;
};

template<typename Class>
struct reflected_member_call<Class> {
   template<typename ReturnType>
   operator ReturnType() {
      called = true;
      return do_invoke<ReturnType>(
         std::tuple<>{},
         class_instance,
         this_type
      );
   }

   ~reflected_member_call() noexcept(false) {
      if (called) return;
      called = true;
      do_invoke<void>(
         std::tuple<>{},
         class_instance,
         this_type
      );
   }

private:
   template<typename> friend struct reflected_member;

   Class *const        class_instance;
   const std::size_t   this_type;
   bool                called;

   reflected_member_call(
      Class* instance,
      std::size_t type,
      std::tuple<>&&
   ) : class_instance{ instance },
      this_type{ type },
      called(false)
   {}

   reflected_member_call(reflected_member_call&& other) :
      class_instance{ other.class_instance },
      this_type{ other.this_type },
      called(false)
   {
      other.called = true;
   }

};

}
}
