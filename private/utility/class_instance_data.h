#pragma once
#include <cstddef>

template<typename Class, bool is_null = false>
struct class_instance_data {
   Class& data;
};

template<typename Class>
struct class_instance_data<Class, true> {
};
