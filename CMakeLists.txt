CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )

# Set CROSS to 1 for linux to windows cross compilation with mingw
set(CROSS 0)

set(PRJ Uoroboro)

set(CMAKE_CXX_STANDARD 17)

project(${PRJ})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

add_executable(${PRJ}
main.cpp
master.cpp
)

if(WIN32)
target_link_libraries(${PRJ} user32 kernel32 gdi32 opengl32)
endif()

if(UNIX AND NOT APPLE)
	if(CROSS)
		set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
		target_link_libraries(${PRJ} gdi32 opengl32)
	else()
		target_link_libraries(${PRJ} X11 GL)
	endif()
endif()

