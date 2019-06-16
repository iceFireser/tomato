
project(trans)

set(CMAKE_CXX_STANDARD 11)

add_executable(trans
	main.cpp
	do_trans.cpp do_trans.h front_desk.cpp front_desk.h)

target_link_libraries(pthread)

