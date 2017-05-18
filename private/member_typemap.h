#pragma once

#include <cstring>

#include "exceptions.h"
#include "reflected_member.h"

#include "macros/class_reflection_info.h"
#include "utility/typelist.h"

namespace reflect {
namespace detail {

template<typename Class>
struct member_map {
   using Members       = member_list_t<Class>;
   using ReflectedType = reflected_member<Class>;

   constexpr member_map() noexcept : instance{ nullptr } {}
   constexpr member_map(Class& instance) noexcept :
      instance{ std::addressof(instance) }
   {}

   template<size_t Len>
   ReflectedType operator[](const char(&name)[Len]) {
      return get_member(name, Members());
   }

   template<typename Type, std::enable_if_t<std::is_same_v<char, Type>>>
   ReflectedType operator[](const Type* name) {
      return get_member(name, std::strlen(name), Members());
   }

   ReflectedType operator[](const std::string& name) {
      return get_member(name.c_str(), name.length(), Members());
   }

private:
   Class* instance;

   template<std::size_t>
   ReflectedType get_member(const char* name, size_t, typelist<>) const {
      throw member_access_error{ std::string{"No member named: "} + name };
   }

   template<std::size_t>
   ReflectedType get_member(const char* name, typelist<>) const {
      throw member_access_error{ std::string{"No member named: "} + name };
   }

   template<std::size_t index = 0, typename Option, typename... Options>
   ReflectedType get_member(
      const char* name,
      size_t len,
      typelist<Option, Options...>)
   {
      const auto& key = member_name<Option>::key();
      constexpr auto KeyLen = (sizeof(member_name<Option>::key()) - 1);
      if (len != KeyLen || std::strncmp(key, name, KeyLen) != 0) {
         return get_member<index + 1>(name, len, typelist<Options...>());
      }

      return{ index, instance };
   }

   template<
      std::size_t index = 0,
      std::size_t Len,
      typename Option,
      typename... Options>
   ReflectedType get_member(
      const char(&name)[Len],
      typelist<Option, Options...>)
   {
      if (!equal_strings(member_name<Option>::key(), name)) {
         return get_member<index + 1>(name, typelist<Options...>());
      }

      return{ index, instance };
   }

   template<size_t Index = 0, size_t LLen = 0, size_t RLen = 0>
   static constexpr bool equal_strings(
      const char(&lhs)[LLen],
      const char(&rhs)[RLen],
      std::enable_if_t<Index != LLen, bool> = true,
      std::enable_if_t<LLen == RLen, bool> = true)
   {
      return lhs[Index] == rhs[Index]
         ? equal_strings<Index + 1>(lhs, rhs)
         : false;
   }

   template<size_t Index, size_t LLen, size_t RLen>
   static constexpr bool equal_strings(
      const char(&)[LLen],
      const char(&)[RLen],
      std::enable_if_t<Index == LLen, bool> = true,
      std::enable_if_t<LLen == RLen, bool> = true)
   {
      return true;
   }

   template<size_t Index = 0, size_t LLen = 0, size_t RLen = 0>
   static constexpr bool equal_strings(
      const char(&)[LLen],
      const char(&)[RLen],
      std::enable_if_t<LLen != RLen, bool> = true)
   {
      return false;
   }
};

}
}
