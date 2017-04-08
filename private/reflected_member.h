#pragma once

#include <utility>

#include "exceptions.h"
#include "member_assigner_base.h"
#include "member_invoker_base.h"
#include "reflected_member_call.h"

namespace reflect {
namespace detail {

template<typename Class, typename TypeRepr>
struct reflected_member {
   using ThisType = reflected_member<Class, TypeRepr>;

   reflected_member() = default;

   template<typename Type, typename = std::enable_if_t<!std::is_pointer_v<Type>>>
   operator Type&() {
      using TypeInfo = TypeInfo<Class, Type>;
      using Options = decltype(
         filter_compatible_types<Type>(ObjTypes<Class>())
      );

      if (TypeInfo::value && TypeInfo::index == get_type()) {
         return static_cast<member_assigner<Class, ThisType, Type>&>(
            *this
         ).get();
      }

      return get<Type&>(Options());
   }

   template<typename Type, typename = std::enable_if_t<std::is_pointer_v<Type>>>
   operator Type() {
      using TypeInfo = TypeInfo<Class, Type>;
      using Options = decltype(
         filter_compatible_types<Type>(ObjTypes<Class>())
      );

      if (TypeInfo::value && TypeInfo::index == get_type()) {
         return static_cast<member_assigner<Class, ThisType, Type>&>(
            *this
         ).get();
      }

      return get<Type>(Options());
   }

   template<typename Type>
   void operator=(Type&& arg) {
      using TypeInfo = TypeInfo<Class, Type>;
      using Options = decltype(
         filter_compatible_types<Type>(ObjTypes<Class>())
      );

      if (TypeInfo::value && TypeInfo::index == get_type()) {
         return static_cast<member_assigner<Class, ThisType, Type>&>(
            *this
         ) = std::forward<Type>(arg);
      }

      return assign(Options(), std::forward<Type>(arg));
   }

   template<typename ReturnType, typename... Args>
   ReturnType invoke(Args&&... args) {
      using Type = ReturnType(Args&&...);
      using TypeInfo = TypeInfo<Class, Type>;
      using Options = decltype(
         filter_compatible_types<Type>(FnTypes<Class>())
      );

      if (TypeInfo::value && TypeInfo::index == get_type()) {
         return static_cast<ReturnType>(
            static_cast<
            member_invoker<Class, ThisType, ReturnType, Args&&...>&
            >(*this)(std::forward<Args>(args)...)
         );
      }

      return invoke<ReturnType>(Options(), std::forward<Args>(args)...);
   }

   template<typename... Args>
   reflected_member_call<Class, ThisType, Args&&...> operator()(Args&&... args) {
      return{ *this, std::forward<Args>(args)... };
   }

private:
   template<typename ReturnType, typename... Args>
   ReturnType invoke(typelist<>, Args&&... args) {
      throw invalid_function_call{
         "No matching function for argument list"
      };
   }

   template<
      typename ReturnType,
      typename... Args,
      typename OptionRT,
      typename... OptionArgs,
      typename... Options>
      ReturnType invoke(
         typelist<OptionRT(OptionArgs...), Options...>,
         Args&&... args)
   {
      using TypeInfo = TypeInfo<Class, OptionRT(OptionArgs...)>;

      if (TypeInfo::index == get_type()) {
         return static_cast<ReturnType>(
            static_cast<
            member_invoker<Class, ThisType, OptionRT, OptionArgs...>&
            >(*this)(static_cast<OptionArgs>(std::forward<Args>(args))...)
         );
      }

      return invoke<ReturnType>(
         typelist<Options...>(),
         std::forward<Args>(args)...
      );
   }

   template<typename Type>
   void assign(typelist<>, Type&& arg) {
      throw invalid_assignment_type{
         "Attempting to assign wrong type to member"
      };
   }

   template<typename Type, typename Option, typename... Options>
   void assign(typelist<Option, Options...>, Type&& arg)
   {
      using TypeInfo = TypeInfo<Class, Option>;

      if (TypeInfo::index == get_type()) {
         return static_cast<member_assigner<Class, ThisType, Option>&>(
            *this
         ) = std::forward<Type>(arg);
      }

      return assign(typelist<Options...>(), std::forward<Type>(arg));
   }

   template<typename Type>
   Type get(typelist<>) {
      throw invalid_requested_member_type{
         "Member type does not match requested type"
      };
   }

   template<
      typename Type,
      typename Option,
      typename... Options>
      Type get(typelist<Option, Options...>)
   {
      using TypeInfo = TypeInfo<Class, Option>;

      if (TypeInfo::index == get_type()) {
         return (Type) static_cast<member_assigner<Class, ThisType, Option>&>(
            *this
         ).get();
      }

      return get<Type>(typelist<Options...>());
   }

   virtual std::intptr_t get_type() const = 0;

   constexpr reflected_member(const reflected_member&) = default;
   constexpr reflected_member(reflected_member&&) = default;

   template<typename, typename, typename, typename>
   friend struct member_assigner;

   template<typename, typename, typename, typename...>
   friend struct member_invoker;
};

}
}
