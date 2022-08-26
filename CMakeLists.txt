# https://gitlab.com/CLIUtils/modern-cmake/-/blob/master/examples/extended-project/CMakeLists.txt

cmake_minimum_required(VERSION 3.14...3.23)

project(
    crossword-gui
	VERSION 1.0.0
	LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)
    

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing only available if this is the main app
    # Note this needs to be done in the main CMakeLists
    # since it calls enable_testing, which must be in the
    # main CMakeLists.
    include(CTest)

    # Docs only available if this is the main app
    # find_package(Doxygen)
    # if(Doxygen_FOUND)
    #     add_subdirectory(docs)
    # else()
    #     message(STATUS "Doxygen not found, not building docs")
    # endif()
endif()

# set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}  -fPIC" )
# set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC" )


include(FetchContent)

FetchContent_Declare(
    fmtlib
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        8.1.1
)
FetchContent_MakeAvailable(fmtlib)


add_subdirectory(crossword-lib)
add_subdirectory(crossword-gui)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
   AND BUILD_TESTING)
  add_subdirectory(tests)
endif()