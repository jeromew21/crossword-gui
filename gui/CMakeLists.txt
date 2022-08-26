add_executable(crossword-gui ./src/main.cpp)

target_compile_features(crossword-gui PRIVATE cxx_std_17)

target_link_libraries(crossword-gui PRIVATE b12)

# copy custom assets to build directory
#add_custom_target(game_assets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources)
#add_dependencies(crossword-gui game_assets)