CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )

set(PRJ Uroboro)

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
target_link_libraries(${PRJ} X11 GL)
endif()

