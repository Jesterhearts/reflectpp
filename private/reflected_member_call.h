#pragma once

#include <tuple>

#include "exceptions.h"
#include "member_invoke.h"

#include "utility/class_instance_data.h"

namespace reflect {
namespace detail {

template<typename Class, bool is_null, typename... Args>
struct reflected_member_call {

   template<typename ReturnType>
   operator ReturnType() {
      called = true;
      return do_invoke<ReturnType>(
         this_type,
         std::move(params),
         instance
      );
   }

   ~reflected_member_call() noexcept(false) {
      if (called) return;
      called = true;
      do_invoke(this_type, std::move(params), instance);
   }

private:
   template<typename, bool> friend struct reflected_member;

   const std::size_t                   this_type;
   std::tuple<Args...>                 params;
   class_instance_data<Class, is_null> instance;
   bool                                called;


   constexpr reflected_member_call(
      std::size_t type,
      std::tuple<Args...>&& args,
      class_instance_data<Class, is_null> instance
   ) : this_type{ type },
      params{ std::move(args) },
      instance{ instance },
      called(false)
   {}

   constexpr reflected_member_call(reflected_member_call&& other) :
      this_type{ other.this_type },
      params{ std::move(other.params) },
      instance{ other.instance },
      called(false)
   {
      other.called = true;
   }

   reflected_member_call(const reflected_member_call&) = delete;
   reflected_member_call& operator=(const reflected_member_call&) = delete;
   reflected_member_call& operator=(reflected_member_call&&) = delete;
};

template<typename Class, bool is_null>
struct reflected_member_call<Class, is_null> {
   template<typename ReturnType>
   operator ReturnType() {
      called = true;
      return do_invoke<ReturnType>(
         this_type,
         std::tuple<>{},
         instance
      );
   }

   ~reflected_member_call() noexcept(false) {
      if (called) return;
      called = true;
      do_invoke(this_type, std::tuple<>{}, instance);
   }

private:
   template<typename, bool> friend struct reflected_member;

   const std::size_t                   this_type;
   class_instance_data<Class, is_null> instance;
   bool                                called;

   constexpr reflected_member_call(
      std::size_t type,
      std::tuple<>&&,
      class_instance_data<Class, is_null> instance
   ) : this_type{ type },
      instance{ instance },
      called(false)
   {}

   constexpr reflected_member_call(reflected_member_call&& other) :
      instance{ other.instance },
      this_type{ other.this_type },
      called(false)
   {
      other.called = true;
   }

};

}
}
