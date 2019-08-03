
project(trans)

set(CMAKE_CXX_STANDARD 11)

add_executable(trans
	main.cpp
	do_trans.cpp
	front_desk.cpp
	)

include_directories(../include)

target_link_libraries(trans
	libpthread.so
	)

