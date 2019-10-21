CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )

set(PRJ Uroboro)

set(CROSS 1)

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
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
		set(LIBS d3d11 d3dcompiler)
		target_link_libraries(${PRJ} gdi32 opengl32)
	else()
		target_link_libraries(${PRJ} X11 GL)
	endif()
endif()

