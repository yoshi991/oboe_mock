cmake_minimum_required(VERSION 3.6)

set (PARSELIB_DIR ../parselib)
set (FLOWGRAPH_DIR ../flowgraph)

include_directories(
        ${PARSELIB_DIR}
        ${FLOWGRAPH_DIR}
        ${CMAKE_CURRENT_LIST_DIR}
    )

add_library(iolib
    STATIC
        ${CMAKE_CURRENT_LIST_DIR}/player/SampleSource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/player/SampleBuffer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/player/OneShotSampleSource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/player/SimpleMultiPlayer.cpp
    )

target_link_libraries(iolib
        oboe::oboe
        log
    )