#pragma once

#include <cstring>

#include "exceptions.h"
#include "member.h"
#include "reflected_instance.h"

#include "utility/type_and_index.h"
#include "utility/typelist.h"
#include "utility/typelist_to_numberlist.h"

namespace reflect {
namespace detail {
template<typename, typename, typename> struct member_typemap_impl;

template<typename Class, typename... types>
using member_typemap = member_typemap_impl<
   Class,
   typelist<types...>,
   std::index_sequence_for<std::size_t, types...>
>;

template<
   typename Class,
   typename... members,
   size_t... indexes>
struct member_typemap_impl<
   Class,
   typelist<members...>,
   std::integer_sequence<std::size_t, indexes...>
> : reflected_instance<Class>, member<Class, members>...
{
   using Members = typelist<members...>;
   using Indexes = std::integer_sequence<std::size_t, indexes...>;
   using ReflectedType = reflected_member<Class>;

   using reflected_instance<Class>::reflected_instance;

   ReflectedType& get_member(
      const char* name,
      size_t,
      typelist<>) const
   {
      throw member_access_error{ std::string{"No member named: "} +name };
   }

   ReflectedType& get_member(
      const char* name,
      typelist<>) const
   {
      throw member_access_error{ std::string{"No member named: "} +name };
   }

   template<typename Option, typename... Options>
   ReflectedType& get_member(
      const char* name,
      size_t len,
      typelist<Option, Options...>)
   {
      const auto& key = member_name<Option>::key();
      constexpr auto KeyLen = (sizeof(member_name<Option>::key()) - 1);
      if (len != KeyLen || std::strncmp(key, name, KeyLen) != 0) {
         return get_member(name, len, typelist<Options...>());
      }

      return static_cast<ReflectedType&>(
         static_cast<member<Class, Option>&>(*this)
      );
   }

   template<size_t Len, typename Option, typename... Options>
   ReflectedType& get_member(
      const char(&name)[Len],
      typelist<Option, Options...>)
   {
      if (!equal_strings(member_name<Option>::key(), name)) {
         return get_member(name, typelist<Options...>());
      }

      return static_cast<ReflectedType&>(
         static_cast<member<Class, Option>&>(*this)
      );
   }

   template<size_t Len>
   ReflectedType& operator[](const char(&name)[Len]) {
      return get_member(name, Members());
   }

   template<typename Type, std::enable_if_t<std::is_same_v<char, Type>>>
   ReflectedType& operator[](const Type* name) {
      return get_member(name, std::strlen(name), Members());
   }

   ReflectedType& operator[](const std::string& name) {
      return get_member(name.c_str(), name.length(), Members());
   }

private:
   template<size_t Index = 0, size_t LLen = 0, size_t RLen = 0>
   static constexpr bool equal_strings(
      const char(&lhs)[LLen],
      const char(&rhs)[RLen],
      std::enable_if_t<Index != LLen, bool> = true,
      std::enable_if_t<LLen == RLen, bool> = true)
   {
      return lhs[Index] == rhs[Index] ? equal_strings<Index + 1>(lhs, rhs) : false;
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
