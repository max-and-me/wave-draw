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

target_compile_features(wave-draw 
    PRIVATE 
        cxx_std_17
)

# executable
add_executable(wave-draw-app
    source/main.cpp
)

target_link_libraries(wave-draw-app
    PRIVATE
        wave-draw
)

target_compile_features(wave-draw-app
    PRIVATE 
        cxx_std_17
)
