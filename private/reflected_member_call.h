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
         this_type,
         std::move(params),
         class_instance
      );
   }

   ~reflected_member_call() noexcept(false) {
      if (called) return;
      called = true;
      do_invoke(this_type, std::move(params), class_instance);
   }

private:
   template<typename> friend struct reflected_member;

   const std::size_t   this_type;
   std::tuple<Args...> params;
   Class *const        class_instance;
   bool                called;

   constexpr reflected_member_call(
      std::size_t type,
      std::tuple<Args...>&& args,
      Class* instance
   ) : this_type{ type },
      params{ std::move(args) },
      class_instance{ instance },
      called(false)
   {}

   constexpr reflected_member_call(reflected_member_call&& other) :
      this_type{ other.this_type },
      params{ std::move(other.params) },
      class_instance{ other.instance },
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
         this_type,
         std::tuple<>{},
         class_instance
      );
   }

   ~reflected_member_call() noexcept(false) {
      if (called) return;
      called = true;
      do_invoke(this_type, std::tuple<>{}, class_instance);
   }

private:
   template<typename> friend struct reflected_member;

   const std::size_t this_type;
   Class *const      class_instance;
   bool              called;

   constexpr reflected_member_call(
      std::size_t type,
      std::tuple<>&&,
      Class* instance
   ) : this_type{ type },
      class_instance{ instance },
      called(false)
   {}

   constexpr reflected_member_call(reflected_member_call&& other) :
      class_instance{ other.class_instance },
      this_type{ other.this_type },
      called(false)
   {
      other.called = true;
   }

};

}
}
