CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )

set(PRJ KrioTest)

set(CMAKE_CXX_STANDARD 17)

project(${PRJ})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

add_executable(${PRJ}
main.cpp
master.cpp
)

target_link_libraries(${PRJ} X11 GL)
