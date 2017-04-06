#pragma once
#include <cstdint>
#include <limits>

namespace reflect {
namespace detail {

template<bool, bool, bool> struct type_repr;

template<>
struct type_repr<true, true, true> {
    using type = uint8_t;
};

template<>
struct type_repr<false, true, true> {
    using type = uint16_t;
};

template<>
struct type_repr<false, false, true> {
    using type = uint32_t;
};

template<>
struct type_repr<false, false, false> {
    using type = uint64_t;
};

template<typename... types>
using type_repr_t = typename type_repr<
    sizeof...(types) <= std::numeric_limits<uint8_t>::max(),
    sizeof...(types) <= std::numeric_limits<uint16_t>::max(),
    sizeof...(types) <= std::numeric_limits<uint32_t>::max()
>::type;

}
}
