# Crossword Editor
A complete cross-platform* guided user interface for creating, editing, and solving crossword puzzles.

## Screenshots
![screenshot of crossword-gui](https://raw.githubusercontent.com/jeromew21/crossword-gui/main/resources/sc.png)

## Features
Word lookup, fill algorithm, puzzle creation,
PDF export

### Future
full testing suite, improved database normalization,
compilation to WASM and web frontend port

## Design Philosophy

Portable: ideally compiles on everything, including WebASM.

Readable: code well-documented and human-readable.

Performant: optimized for maximal performance.

### IFAQ (Infrequently Asked Questions):

Q: Why C++/WxWidgets not Python or a webapp?

A: Performance and low-level control over data structures is critical
especially for the long-term project that I expect this to become. 

## Building (Desktop)

Requires C++11, wxWidgets and Cairo.

### Linux
Tested on Arch Linux.
```
mkdir build
cd build
cmake ..
make
```

### *Mac

Not tested.

### Windows
Tested in Visual Studio 2022 running on Windows 11.

Recommended: use vcpkg to install Cairo and just use the installer for wxWidgets.

Create build directory and cd into it (not sure if the VCPKG argument is needed).
```
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
```
In VS: Change from console application to Windows (Linker->System->SubSystem)

To fix blurry UI: add AMD dpi manifest (Manifest Tool->Input and Output) `%WXWIN%/include/wx/msw/wx_dpi_aware.manifest`

## Running
To load a database at startup:
```
./crossword-gui -d path_to_database
```

## *Building (Web)

Not tested or built yet.

## Contributing

### Documentation

Documented with Doxygen.

### Testing

Tests not written yet.

## License

MIT License

## Acknowledgements 
I would like to acknowledge the people in my life who got me addicted to the NYT crossword. You know who you are.

## Algorithms

A short description of the fill algorithm: we use a modified backtracking/depth-first search, where we value nodes 
in an order based on two values: 1) a frequency word taken from the database
to favor common English words and 2) a letter frequency score, to favor words with common letters that should 
make for a better branch to traverse.

Ideally, the nodes are searched in order based on letter frequency score, while a cutoff is imposed 
on the frequecy (readability) score. 

## Crossword Clue Database

Source 

### CSV Format

## File Format
We use a custom crossword file format.
