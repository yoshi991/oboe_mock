cmake_minimum_required(VERSION 3.6)

add_library(parselib
    STATIC
        # ${CMAKE_CURRENT_LIST_DIR}/stream/FileInputStream.cpp
        ${CMAKE_CURRENT_LIST_DIR}/stream/InputStream.cpp
        ${CMAKE_CURRENT_LIST_DIR}/stream/MemInputStream.cpp

        ${CMAKE_CURRENT_LIST_DIR}/wav/AudioEncoding.cpp
        ${CMAKE_CURRENT_LIST_DIR}/wav/WavChunkHeader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/wav/WavFmtChunkHeader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/wav/WavRIFFChunkHeader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/wav/WavStreamReader.cpp
    )

target_link_libraries(parselib
        log
    )