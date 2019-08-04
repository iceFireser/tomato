
project(trans)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/../include)
link_directories(${PROJECT_SOURCE_DIR}/../lib)

add_executable(trans
	main.cpp
	do_trans.cpp
	front_desk.cpp
	)


target_link_libraries(trans
	libpthread.so
	libttools.a
	)

