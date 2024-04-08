cmake_minimum_required(VERSION 3.16)
project(AFGS1)

option(BUILD_T35_APP "Build the AFGS1 T35 application" ON)
option(BUILD_SEI_APP "Build the AFGS1 SEI application" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${AFGS1_SOURCE_DIR}/bin)

# Add the AFGS1 functions and include them in the search path
add_subdirectory("Common")
include_directories("Common")

# Sample application to generate an AFGS1 T35 message
if(BUILD_T35_APP)
    add_subdirectory("Apps/T35Afgs1App")
endif(BUILD_T35_APP)

# Sample application to insert AFGS1 T35 messages in an HEVC bit-stream
if(BUILD_SEI_APP)
    add_subdirectory("Apps/SEIAfgs1App")
endif(BUILD_SEI_APP)

