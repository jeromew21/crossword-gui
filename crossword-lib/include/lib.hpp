#pragma once

#include <cstddef>
#include <string>

namespace crossword_lib
{
    using uint = std::size_t;
    using String = std::string;

    struct Coordinates {
        uint x; /* column index */
        uint y; /* row index */
    };
}
