
project(ttools)

set(CMAKE_CXX_STANDARD 11)


set (SOURCE_FILES
    systool.c)

include_directories(../include)

add_library(ttools STATIC ${SOURCE_FILES})

