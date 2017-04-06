#pragma once

#include "number_list.h"

namespace reflect {
namespace detail {

template<typename> struct typelist_to_numberlist_impl;

template<template<typename...> class typelist, typename... types>
struct typelist_to_numberlist_impl<typelist<types...>> {
    using list = number_list_t<sizeof...(types)>;
};

template<typename typelist>
using typelist_to_numberlist = typename typelist_to_numberlist_impl<typelist>::list;

}
}
