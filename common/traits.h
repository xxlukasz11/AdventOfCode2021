#pragma once

#include <type_traits>

namespace common {

template<typename T>
using EnableIntegral = std::enable_if_t<std::is_integral_v<T>>;

}