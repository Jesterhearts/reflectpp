#pragma once

#include "dependencies/boost/preprocessor/seq/for_each.hpp"
#include "dependencies/boost/preprocessor/seq/pop_front.hpp"

#include "dependencies/boost/preprocessor/variadic/to_seq.hpp"
#include "dependencies/boost/preprocessor/variadic/elem.hpp"

namespace reflect {
namespace detail {

template<typename MemberType, MemberType> struct member_info;

#define MEMBER_INFO(Class, Member)                       \
   member_info<decltype(&Class::Member), &Class::Member>

#define REFLECT_LIST_MEMBERS(Class, ...)        \
   REFLECT_MEMBER_INFO(                         \
      Class,                                    \
      BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__)    \
   )                                            \
   BOOST_PP_SEQ_FOR_EACH(                       \
      REFLECT_CREATE_MEMBER_INFO, Class,        \
      BOOST_PP_SEQ_POP_FRONT(                   \
         BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)  \
      )                                         \
   )

#define REFLECT_CREATE_MEMBER_INFO(s, Class, Member)  \
   , REFLECT_MEMBER_INFO(Class, Member)

#define REFLECT_MEMBER_INFO(Class, Member)                  \
   member_info<decltype(&Class:: Member), &Class:: Member>

}
}
