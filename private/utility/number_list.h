#pragma once

namespace reflect {
namespace detail {

template<size_t... entries>
struct number_list {
    constexpr static const size_t length = sizeof...(entries);
};

template<size_t, typename...> struct create_number_list;

template<size_t... entries>
struct create_number_list<0, number_list<entries...>> {
    using list = number_list<entries...>;
};

template<size_t remain, size_t... entries>
struct create_number_list<
    remain,
    number_list<entries...>
> : create_number_list<remain - 1, number_list<remain - 1, entries...>>{};

template<size_t remain>
struct create_number_list<
    remain
> : create_number_list<remain - 1, number_list<remain - 1>>{};

template<>
struct create_number_list<0> {
    using list = number_list<>;
};


template<size_t count>
using number_list_t = typename create_number_list<count>::list;

}
}
