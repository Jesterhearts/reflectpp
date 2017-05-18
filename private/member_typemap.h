#pragma once

#include <cstring>

#include "exceptions.h"
#include "reflected_member.h"

#include "macros/class_reflection_info.h"
#include "utility/typelist.h"

namespace reflect {
namespace detail {

template<typename Class, bool is_null = false>
struct member_map {
   using members        = member_list_t<Class>;
   using reflected_type = reflected_member<Class, is_null>;

   template<typename = std::enable_if_t<is_null>>
   constexpr member_map() noexcept {};

   template<typename = std::enable_if_t<!is_null>>
   constexpr member_map(Class& instance) noexcept :
      instance{ instance }
   {}

   template<size_t Len>
   reflected_type operator[](const char(&name)[Len]) {
      return get_member(name, members{});
   }

   template<typename Type, std::enable_if_t<std::is_same_v<char, Type>>>
   reflected_type operator[](const Type* name) {
      return get_member(name, std::strlen(name), members{});
   }

   reflected_type operator[](const std::string& name) {
      return get_member(name.c_str(), name.length(), members{});
   }

private:
   class_instance_data<Class, is_null> instance;

   template<std::size_t>
   reflected_type get_member(const char* name, size_t, typelist<>) const {
      throw member_access_error{ std::string{"No member named: "} + name };
   }

   template<std::size_t>
   reflected_type get_member(const char* name, typelist<>) const {
      throw member_access_error{ std::string{"No member named: "} + name };
   }

   template<std::size_t index = 0, typename Option, typename... Options>
   reflected_type get_member(
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
   reflected_type get_member(
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
      return lhs[Index] == rhs[Index] && equal_strings<Index + 1>(lhs, rhs);
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
