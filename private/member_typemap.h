#pragma once

#include <cstring>

#include "exceptions.h"
#include "member.h"
#include "reflected_instance.h"

#include "utility/extract_underlying_types.h"
#include "utility/find_duplicate_type.h"
#include "utility/type_repr.h"
#include "utility/typelist.h"
#include "utility/typelist_to_numberlist.h"

namespace reflect {
namespace detail {
template<typename, typename, typename, typename> struct member_typemap_impl;

template<typename Class, typename... types>
using member_typemap = member_typemap_impl<
   Class,
   typelist<types...>,
   member_underlyingtype_set<types...>,
   typelist_to_numberlist<member_underlyingtype_set<types...>>
>;

template<
   typename Class,
   template<typename...> class member_typelist,
   template<typename...> class member_underlyingtype_set,
   template<size_t...> class index_list,
   typename... members,
   typename... underlying_members,
   size_t... indexes>
   struct member_typemap_impl<
   Class,
   member_typelist<members...>,
   member_underlyingtype_set<underlying_members...>,
   index_list<indexes...>
   > : reflected_instance<Class>,
   member<Class, members, type_repr_t<underlying_members...>>...
{
   using Members = member_typelist<members...>;
   using Types = member_underlyingtype_set<underlying_members...>;
   using FnTypes = decltype(filter_fn_types(Types()));
   using ObjTypes = decltype(filter_obj_types(Types()));
   using Indexes = index_list<indexes...>;
   using TypeRepr = type_repr_t<underlying_members...>;
   using ReflectedType = reflected_member<Class, TypeRepr>;

   using reflected_instance<Class>::reflected_instance;

   ReflectedType& get_member(
      const char* name,
      size_t,
      member_typelist<>) const
   {
      throw member_access_error{ std::string{"No member named: "} +name };
   }

   ReflectedType& get_member(
      const char* name,
      member_typelist<>) const
   {
      throw member_access_error{ std::string{"No member named: "} +name };
   }

   template<typename Option, typename... Options>
   ReflectedType& get_member(
      const char* name,
      size_t len,
      member_typelist<Option, Options...>)
   {
      const auto& key = member_name<Option>::key();
      constexpr auto KeyLen = (sizeof(member_name<Option>::key()) - 1);
      if (len != KeyLen || std::strncmp(key, name, KeyLen) != 0) {
         return get_member(name, len, member_typelist<Options...>());
      }

      return static_cast<ReflectedType&>(
         static_cast<member<Class, Option, TypeRepr>&>(*this)
      );
   }

   template<size_t Len, typename Option, typename... Options>
   ReflectedType& get_member(
      const char(&name)[Len],
      member_typelist<Option, Options...>)
   {
      if (!equal_strings(member_name<Option>::key(), name)) {
         return get_member(name, member_typelist<Options...>());
      }

      return static_cast<ReflectedType&>(
         static_cast<member<Class, Option, TypeRepr>&>(*this)
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

   template<typename type>
   constexpr static auto get_type_info() noexcept {
      return get_type_info_impl<type>();
   }

private:
   template<typename type>
   constexpr static auto get_type_info_impl() noexcept {
      using TypeInfo = find_duplicate_type<type, Types>;
      return TypeInfo{};
   }

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
