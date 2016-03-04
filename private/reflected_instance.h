#pragma once

namespace reflect {
namespace detail {

template<typename Class>
struct reflected_instance {
    reflected_instance() = default;
    reflected_instance(Class& instance) : instance(&instance) {}

    Class* instance;
};

}
}
