#pragma once

#include "member_info.h"
#include "member_names.h"

#include "utility/typelist.h"

namespace reflect {
namespace detail {

template<typename> struct member_list;

#define REFLECT_ENABLE(Class, ...)                                         \
   REFLECT_CREATE_KEYS(Class, __VA_ARGS__)                                 \
                                                                           \
   namespace reflect {                                                     \
   namespace detail {                                                      \
   template<>                                                              \
   struct member_list<Class> {                                             \
      using members = typelist<REFLECT_LIST_MEMBERS(Class, __VA_ARGS__)>;  \
   };                                                                      \
   }                                                                       \
   }

template<typename Class>
using member_list_t = typename member_list<std::decay_t<Class>>::members;

}
}
