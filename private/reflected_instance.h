#pragma once

namespace reflect {
namespace detail {

template<typename Class>
struct reflected_instance {
   constexpr reflected_instance() noexcept : instance{ nullptr } {}
   constexpr reflected_instance(Class& instance) noexcept :
      instance{ std::addressof(instance) }
   {}

   Class* instance;
};

}
}
