
project(cli)

set(CMAKE_CXX_STANDARD 11)

include_directories(../include)

add_executable(cli
	main.cpp
	t_connect.cpp
	)


