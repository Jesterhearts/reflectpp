#pragma once

#include <utility>

#include "exceptions.h"
#include "member_assigner_base.h"
#include "member_invoker_base.h"
#include "reflected_member_call.h"

namespace reflect {
namespace detail {

template<typename Class>
struct reflected_member {
   using ThisType = reflected_member<Class>;

   reflected_member() = default;

   template<typename Type, typename = std::enable_if_t<!std::is_pointer_v<Type>>>
   operator Type&() {
      constexpr auto type_info = get_type_info<Class, Type>();
      using Options = decltype(
         filter_compatible_types<Type>(ObjTypes<Class>())
      );

      if (type_info.value && type_info.id == get_type()) {
         return static_cast<member_assigner<Class, ThisType, Type>&>(
            *this
         ).get();
      }

      return get<Type&>(Options());
   }

   template<typename Type, typename = std::enable_if_t<std::is_pointer_v<Type>>>
   operator Type() {
      constexpr auto type_info = get_type_info<Class, Type>();
      using Options = decltype(
         filter_compatible_types<Type>(ObjTypes<Class>())
      );

      if (type_info.value && type_info.id == get_type()) {
         return static_cast<member_assigner<Class, ThisType, Type>&>(
            *this
         ).get();
      }

      return get<Type>(Options());
   }

   template<typename Type>
   void operator=(Type&& arg) {
      constexpr auto type_info = get_type_info<Class, Type>();
      using Options = decltype(
         filter_compatible_types<Type>(ObjTypes<Class>())
      );

      if (type_info.value && type_info.id == get_type()) {
         return static_cast<member_assigner<Class, ThisType, Type>&>(
            *this
         ) = std::forward<Type>(arg);
      }

      return assign(Options(), std::forward<Type>(arg));
   }

   template<typename ReturnType, typename... Args>
   ReturnType invoke(Args&&... args) {
      using Type = ReturnType(Args&&...);
      constexpr auto type_info = get_type_info<Class, Type>();
      using Options = decltype(
         filter_compatible_types<Type>(FnTypes<Class>())
      );

      if (type_info.value && type_info.id == get_type()) {
         static_cast<
            member_invoker<Class, ThisType, ReturnType, Args&&...>&
         >(*this)(std::forward<Args>(args)...);
      }

      return invoke<ReturnType>(Options(), std::forward<Args>(args)...);
   }

   template<typename... Args>
   reflected_member_call<Class, ThisType, Args&&...> operator()(Args&&... args) {
      return{ *this, std::forward<Args>(args)... };
   }

   virtual std::intptr_t get_type() const noexcept = 0;

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
   std::enable_if_t<std::is_void_v<ReturnType>> invoke(
      typelist<OptionRT(OptionArgs...), Options...>,
      Args&&... args)
   {
      constexpr auto type_info = get_type_info<Class, OptionRT(OptionArgs...)>();

      if (type_info.id == get_type()) {
         static_cast<
            member_invoker<Class, ThisType, OptionRT, OptionArgs...>&
         >(*this)(static_cast<OptionArgs>(std::forward<Args>(args))...);
      }
      else {
         invoke<ReturnType>(
            typelist<Options...>(),
            std::forward<Args>(args)...
         );
      }
   }

   template<
      typename ReturnType,
      typename... Args,
      typename OptionRT,
      typename... OptionArgs,
      typename... Options>
   std::enable_if_t<!std::is_void_v<ReturnType>, ReturnType> invoke(
      typelist<OptionRT(OptionArgs...), Options...>,
      Args&&... args)
   {
      constexpr auto type_info = get_type_info<Class, OptionRT(OptionArgs...)>();

      if (type_info.id == get_type()) {
         return static_cast<
            member_invoker<Class, ThisType, OptionRT, OptionArgs...>&
         >(*this)(static_cast<OptionArgs>(std::forward<Args>(args))...);
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
      constexpr auto type_info = get_type_info<Class, Option>();
      static_assert(!(std::is_pointer_v<Type> && std::is_array_v<Option>), "");

      if (type_info.id == get_type()) {
         #pragma warning(push)
         #pragma warning(disable: 4800 4267)
         return static_cast<member_assigner<Class, ThisType, Option>&>(
            *this
         ) = std::forward<Type>(arg);
         #pragma warning(pop)
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
      constexpr auto type_info = get_type_info<Class, Option>();

      if (type_info.id == get_type()) {
         return (Type) static_cast<member_assigner<Class, ThisType, Option>&>(
            *this
         ).get();
      }

      return get<Type>(typelist<Options...>());
   }

   constexpr reflected_member(const reflected_member&) = default;
   constexpr reflected_member(reflected_member&&) = default;

   template<typename, typename, typename, typename>
   friend struct member_assigner;

   template<typename, typename, typename, typename...>
   friend struct member_invoker;
};

}
}
