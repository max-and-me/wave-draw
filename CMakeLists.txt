cmake_minimum_required(VERSION 3.19.0)

project("Wave Draw")

add_subdirectory(extern)

# library
add_library(wave-draw STATIC
    include/mam/wave-draw/wave-draw.h
    source/wave-draw.cpp
)

target_include_directories(wave-draw
    PUBLIC
        include
    PRIVATE
        source
)

target_link_libraries(wave-draw
    PUBLIC
        GSL
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_features(wave-draw 
        PUBLIC 
            cxx_std_20
    )
else()
    target_compile_features(wave-draw 
        PUBLIC 
            cxx_std_17
    )
endif()

# executable
add_executable(wave-draw-app
    source/main.cpp
)

target_link_libraries(wave-draw-app
    PRIVATE
        wave-draw
)
