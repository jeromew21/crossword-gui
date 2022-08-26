#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
    
TEST_CASE( "foo", "[main]" ) {
   REQUIRE(1 == 1);
}
