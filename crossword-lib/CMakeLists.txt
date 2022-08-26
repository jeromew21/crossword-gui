# https://gitlab.com/CLIUtils/modern-cmake/-/blob/master/examples/extended-project/src/CMakeLists.txt

#add CPP files here
add_library(crossword-lib ./src/lib.cpp)

# public means everyone gets to see the include
target_include_directories(crossword-lib PUBLIC ./include)

# target_include_directories to declare dependencies
target_link_libraries(crossword-lib PRIVATE fmt::fmt)

target_compile_features(crossword-lib PUBLIC cxx_std_17)