#
# Copyright 2018 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(liveEffect LANGUAGES C CXX)

get_filename_component(SAMPLE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    )

add_library(liveEffect
    SHARED
        ${SAMPLE_ROOT_DIR}/debug-utils/trace.cpp
        jni_bridge.cpp
        DuplexEngine.cpp
    )

target_include_directories(liveEffect
    PRIVATE
        ${SAMPLE_ROOT_DIR}
        ${SAMPLE_ROOT_DIR}/debug-utils
    )

# Find the Oboe package
find_package (oboe REQUIRED CONFIG)

target_link_libraries(liveEffect
    PRIVATE
        oboe::oboe
        android
        atomic
        log
    )

# Enable optimization flags: if having problems with source level debugging,
# disable -Ofast ( and debug ), re-enable it after done debugging.
# target_compile_options(liveEffect PRIVATE -Wall -Werror "$<$<CONFIG:RELEASE>:-Ofast>")
if(MSVC)
    target_compile_options(liveEffect PRIVATE /W4 /WX)
else()
    target_compile_options(liveEffect PRIVATE -Wall -Wextra -pedantic)
endif()

target_compile_options(liveEffect PRIVATE -Ofast)
