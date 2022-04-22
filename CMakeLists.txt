cmake_minimum_required(VERSION 3.13)  # CMake version check
project(CrosswordGui)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Cairo)
include_directories(${CAIRO_INCLUDE_DIRS})

find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)
include(${wxWidgets_USE_FILE})

include_directories(src)

add_executable(crossword-gui
        src/widgets/main_entry.cpp
        src/widgets/main_window.cpp
        src/widgets/grid.cpp
        src/widgets/drawing.cpp
        src/widgets/event_handlers.cpp
        src/widgets/dialog.cpp
        src/widgets/cell_renderer.cpp

        src/crossword/cell.cpp
        src/crossword/clue.cpp
        src/crossword/word.cpp
        src/crossword/crossword_action.cpp
        src/crossword/crossword_serialization.cpp
        src/crossword/database.cpp
        src/crossword/trie.cpp
        src/crossword/logging.cpp
        src/crossword/search.cpp
        src/crossword/crossword.cpp)

target_link_libraries(crossword-gui ${wxWidgets_LIBRARIES} ${CAIRO_LIBRARIES})
