cmake_minimum_required(VERSION 2.8.0)
project( equirectangular  CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package( OpenCV 4 REQUIRED )

set(ROOT ./)
include_directories(${ROOT}/src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB equirectangular_SRC
    "src/*.?pp"
)

add_executable( equirectangular ${equirectangular_SRC} )
target_link_libraries( equirectangular ${OpenCV_LIBS} stdc++fs) 

add_custom_target(run
    COMMAND bin/equirectangular
    DEPENDS equirectangular
)


