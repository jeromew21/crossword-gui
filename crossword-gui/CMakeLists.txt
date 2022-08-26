if (MSVC)
    add_executable(crossword-gui WIN32 ./src/main.cpp)
else()
    add_executable(crossword-gui ./src/main.cpp)
endif()

target_compile_features(crossword-gui PRIVATE cxx_std_17)

set(wxBUILD_SHARED OFF CACHE BOOL "Build wx libraries as shared libs")
set(wxBUILD_PRECOMP OFF CACHE BOOL "Use precompiled headers")
set(wxBUILD_MONOLITHIC OFF CACHE BOOL "Build a single library")

FetchContent_Declare(
    wxWidgets
    GIT_REPOSITORY git@github.com:wxWidgets/wxWidgets.git
    GIT_TAG v3.2.0
    GIT_SHALLOW true
)
FetchContent_GetProperties(wxWidgets)
FetchContent_MakeAvailable(wxWidgets)

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

target_link_libraries(crossword-gui PRIVATE crossword-lib wxgl wxbase wxcore wxnet)

# Add hidpi support for Windows
if (MSVC)
    target_sources(crossword-gui PRIVATE ${wxWidgets_SOURCE_DIR}/include/wx/msw/wx_dpi_aware.manifest)
endif()


# copy custom assets to build directory
#add_custom_target(game_assets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources)
#add_dependencies(crossword-gui game_assets)