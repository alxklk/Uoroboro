export LIBS="-lgdi32 -lopengl32"
export OPTS="-static-libstdc++ -static-libgcc -O0 -g3"
export SRCS="main.cpp master.cpp"
x86_64-w64-mingw32-g++ -oUoroboro $SRCS $OPTS $LIBS
