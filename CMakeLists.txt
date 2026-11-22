cmake_minimum_required(VERSION 3.15)
project(screamer C)
set(CMAKE_C_STANDARD 11)

add_executable(screamer src/main.c src/block.c)
target_link_libraries(screamer winmm user32 advapi32)

add_executable(screamer1 src/screamer.c)
add_executable(screamer2 src/screamer.c)
add_executable(screamer3 src/screamer.c)
add_executable(screamer4 src/screamer.c)
add_executable(screamer5 src/screamer.c)

foreach(t screamer1 screamer2 screamer3 screamer4 screamer5)
  target_link_libraries(${t} winmm user32)
endforeach()
