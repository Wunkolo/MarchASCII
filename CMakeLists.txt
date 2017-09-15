cmake_minimum_required( VERSION 3.2.2 )
project( March )

### Standard
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

### Verbosity
set( CMAKE_COLOR_MAKEFILE ON )
set( CMAKE_VERBOSE_MAKEFILE ON )

add_executable(
	March 
	Operations.cpp
	Shapes.cpp
	main.cpp
)

# Find Threads
find_package( Threads REQUIRED )
target_link_libraries(
	March ${CMAKE_THREAD_LIBS_INIT}
)

# Find GLM
find_package( glm REQUIRED )
target_include_directories(
	March PRIVATE
	${GLM_INCLUDE_DIRS}
)
link_directories( ${GLM_LIBRARY_DIRS} )

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
