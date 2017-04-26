#pragma once
#include <string>
#include <type_traits>

#include "exceptions.h"
#include "member_assigner_base.h"
#include "reflected_member.h"

#include "macros/member_names.h"
#include "macros/member_info.h"

namespace reflect {
namespace detail {

template<typename, typename> struct member;

template<
   typename Class,
   typename MemberType,
   MemberType* Member>
struct member<Class, member_info<MemberType*, Member>>
   : member_assigner<Class, reflected_member<Class>, MemberType>
{
   using Type = MemberType;
   using Info = member_info<MemberType*, Member>;

   Type& get() override final {
      return *Member;
   }

   void operator=(const Type& type) {
      assign_impl(type, std::is_const<Type>{});
   }

   void operator=(Type&& type) {
      assign_impl(std::move(type), std::is_const<Type>{});
   }

   template<typename _Type>
   void assign_impl(_Type&& type, std::false_type) {
      *Member = std::forward<_Type>(type);
   }

   template<typename _Type>
   void assign_impl(_Type&&, std::true_type) {
      throw invalid_assignment_to_const{
          std::string{ "Cannot assign to const member: " }
          + member_name<Info>::key()
      };
   }

   std::intptr_t get_type() const noexcept override final {
      return get_type_info<Class, Type>().id;
   }
};

template<
   typename Class,
   typename MemberType,
   size_t MemberSize,
   MemberType (*Member) [MemberSize]>
struct member<Class, member_info<MemberType(*)[MemberSize], Member>>
   : member_assigner<Class, reflected_member<Class>, MemberType[MemberSize]>
{
   using Type = MemberType[MemberSize];
   using Info = member_info<MemberType(*)[MemberSize], Member>;

   Type& get() override final {
      MemberType(*temp1)[MemberSize] = Member;
      return *temp1;
   }

   void operator=(const Type& type) {
      assign_impl(type, std::is_const<Type>{});
   }

   void operator=(Type&& type) {
      assign_impl(std::move(type), std::is_const<Type>{});
   }

   template<typename _Type>
   void assign_impl(_Type&& type, std::false_type) {
      *Member = std::forward<_Type>(type);
   }

   template<typename _Type>
   void assign_impl(_Type&&, std::true_type) {
      throw invalid_assignment_to_const{
          std::string{ "Cannot assign to const member: " }
          + member_name<Info>::key()
      };
   }

   std::intptr_t get_type() const noexcept override final {
      return get_type_info<Class, Type>().id;
   }
};

}
}
