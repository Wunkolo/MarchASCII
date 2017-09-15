cmake_minimum_required( VERSION 3.2.2 )
project( MarchASCII )

### Standard
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

### Verbosity
set( CMAKE_COLOR_MAKEFILE ON )
set( CMAKE_VERBOSE_MAKEFILE ON )

### Target
file( GLOB_RECURSE SOURCE_FILES source/*.cpp )
add_executable(
	March
	${SOURCE_FILES}
)
include_directories(
	include
)

### Threads
find_package( Threads REQUIRED )
target_link_libraries(
	March ${CMAKE_THREAD_LIBS_INIT}
)

### GLM
target_include_directories(
	March PRIVATE
	dependencies/glm
)

### Optimizations
if( MSVC )
	add_compile_options( /Oxs )
	add_compile_options( /arch:AVX2 )
	add_compile_options( /W3 )
elseif( CMAKE_COMPILER_IS_GNUCXX )
	add_compile_options( -m64 )
	add_compile_options( -march=native )
	add_compile_options( -Ofast )
	add_compile_options( -Wall )
	add_compile_options( -Wextra )
endif()
