set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER   clang)
set(CMAKE_CXX_COMPILER clang++)

# Use MinGW as the 'GNU' environment so CMake knows what to expect
set(CMAKE_CXX_COMPILER_TARGET x86_64-w64-mingw32)
set(CMAKE_C_COMPILER_TARGET   x86_64-w64-mingw32)

# Use LLD linker for faster link times
set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld")
set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=lld")
