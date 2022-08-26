#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "crossword.hpp"
    
TEST_CASE( "crossword instance", "[main]" ) {
    REQUIRE(true);
    auto crossword = crossword_lib::Crossword(10, 10);
    REQUIRE(crossword.Height() == 10);
}
