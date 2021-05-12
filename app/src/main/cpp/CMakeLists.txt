cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(liveEffect LANGUAGES C CXX)

get_filename_component(SAMPLE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
set (PARSELIB_DIR libs/parselib)
set (IOLIB_DIR libs/iolib)

include(${PARSELIB_DIR}/CMakeLists.txt)
include(${IOLIB_DIR}/CMakeLists.txt)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    )

add_library(liveEffect
    SHARED
        ${SAMPLE_ROOT_DIR}/debug-utils/trace.cpp
        jni_bridge.cpp
        AudioEngine.cpp
        DuplexEngine.cpp
        decoder/WavDecoder.cpp
    )

target_include_directories(liveEffect
    PRIVATE
        ${SAMPLE_ROOT_DIR}
        ${SAMPLE_ROOT_DIR}/decoder
        ${SAMPLE_ROOT_DIR}/debug-utils
    )

# Find the Oboe package
find_package (oboe REQUIRED CONFIG)

target_link_libraries(liveEffect
    PRIVATE
        iolib
        parselib
        oboe::oboe
        android
        atomic
        log
    )

# Enable optimization flags: if having problems with source level debugging,
# disable -Ofast ( and debug ), re-enable it after done debugging.
# target_compile_options(liveEffect PRIVATE -Wall -Werror "$<$<CONFIG:RELEASE>:-Ofast>")
# if(MSVC)
#     target_compile_options(liveEffect PRIVATE /W4 /WX)
# else()
#     target_compile_options(liveEffect PRIVATE -Wall -Wextra -pedantic)
# endif()

target_compile_options(liveEffect PRIVATE -Ofast)
