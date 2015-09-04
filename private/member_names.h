#pragma once

#include <boost/preprocessor/stringize.hpp>

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/pop_front.hpp>

#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/variadic/elem.hpp>

#include "member_info.h"

namespace reflect {
namespace detail {

template<typename MemberInfo> struct member_name;

#define REFLECT_CREATE_KEYS(Class, ...)         \
    BOOST_PP_SEQ_FOR_EACH(                      \
        REFLECT_CREATE_KEY, Class,              \
        BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)   \
    )

#define REFLECT_CREATE_KEY(s, Class, Member)                    \
    namespace reflect {                                         \
    namespace detail {                                          \
    template<>                                                  \
    struct member_name<REFLECT_MEMBER_INFO(Class, Member)> {    \
        constexpr static                                        \
        decltype(BOOST_PP_STRINGIZE(Member)) key() {            \
            return BOOST_PP_STRINGIZE(Member);                  \
        }                                                       \
    };                                                          \
    }                                                           \
    }

}
}
