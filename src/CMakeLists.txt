cmake_minimum_required(VERSION 3.4.1)

SET(LIBNAME "voice_engine")
project(${LIBNAME})

include(../cmake/c++11_check.cmake)

set(LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(SRC_LIST
#                native-audio-jni.cpp
        OpenSLESCore.cpp
        my_voice_engine_impl.cpp
        VoiceMgr.cpp
        CodecMgr.cpp
        RecordChannel.cpp
        PlayerChannel.cpp
        G7221Helper.cpp
        XUtil/XThreadBase.cpp
        XUtil/XCritSec.cpp
        XUtil/XAutoLock.cpp
        XUtil/XListPtr.cpp
        XUtil/XDataPool.cpp
        XUtil/XUtil.cpp
        XUtil/OperationMgr.cpp)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/libg7221 SRC_G7221_LIST)

add_library(${LIBNAME} SHARED ${SRC_LIST} ${SRC_G7221_LIST})

# Include libraries needed for native-audio-jni lib
target_link_libraries(${LIBNAME}
        log
        OpenSLES)
