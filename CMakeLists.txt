cmake_minimum_required(VERSION 2.6)
project(LPCNet-Thrift)

##########################################
# SET CXX LANG
##########################################
# set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
# set (CMAKE_CXX_STANDARD c99)

add_definitions("-Wall -g -w -fpermissive -O3")


include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
# CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


##########################################
# SET BUILD TYPE
##########################################
# set(DEFAULT_BUILD_TYPE "RelWithDebInfo")
# # Set a default build type if none was specified
# if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
#   message("Setting build type to 'RelWithDebInfo' as none was specified.")
#   set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "Choose the type of build." FORCE)
#   # Set the possible values of build type for cmake-gui
#   set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
#     "MinSizeRel" "RelWithDebInfo")
# endif()


##########################################
# Create a library
##########################################
set(EXECUTABLE_NAME run-plcnet-thrift-server)
# set(LIBRARY_OUTPUT_NAME example_5)


set(EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/external_libraries/)
set(CMAKE_BUILD_DIR ${CMAKE_SOURCE_DIR}/build)


set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

##########################################
# include headers and sources
##########################################
file(GLOB CPP_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB C_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)


include_directories(${PROJECT_SOURCE_DIR}/include/
                    ${EXTERNAL_DIR}thrift/cmake-build/thrift-package/include/
                    ${EXTERNAL_DIR}lame-3.100/cmake-build/libraries/include/
                    ${EXTERNAL_DIR}fdk-aac/cmake-build/fdk-acc-package/include/
                    ${EXTERNAL_DIR}libevent-2.1.8-stable/cmake-build/include/
                    )

##########################################
# linking to library
##########################################


##########################################
# linking to shared library
##########################################
# file(GLOB PROJECT_LINK_LIBS_thrift  ${EXTERNAL_DIR}thrift/cmake-build/thrift-package/lib/*.so)
set(PROJECT_LINK_LIBS_thrift    ${EXTERNAL_DIR}thrift/cmake-build/thrift-package/lib/libthriftnb.so
                                ${EXTERNAL_DIR}thrift/cmake-build/thrift-package/lib/libthrift.so
    )
file(GLOB PROJECT_LINK_LIBS_event   ${EXTERNAL_DIR}libevent-2.1.8-stable/cmake-build/lib/libevent.so)
file(GLOB PROJECT_LINK_LIBS_lame    ${EXTERNAL_DIR}lame-3.100/cmake-build/libraries/lib/*.so)
file(GLOB PROJECT_LINK_LIBS_fdkacc  ${EXTERNAL_DIR}fdk-aac/cmake-build/fdk-acc-package/lib/*.so)


link_directories(${EXTERNAL_DIR}thrift/cmake-build/thrift-package/lib/
                ${EXTERNAL_DIR}lame-3.100/cmake-build/libraries/lib/
                ${EXTERNAL_DIR}fdk-aac/cmake-build/fdk-acc-package/lib/
                ${EXTERNAL_DIR}libevent-2.1.8-stable/cmake-build/lib/
                ) # path to .so files


##########################################
# linking to executable object
##########################################
link_libraries( ${PROJECT_LINK_LIBS_thrift}
                ${PROJECT_LINK_LIBS_lame}
                ${PROJECT_LINK_LIBS_event}
                ${PROJECT_LINK_LIBS_fdkacc}
            )

add_executable(${EXECUTABLE_NAME} ${CPP_SOURCES} ${C_SOURCES})
# add_executable(${EXECUTABLE_NAME} ${C_SOURCES})

target_link_libraries(${EXECUTABLE_NAME} 
                        m
                        stdc++
)

##########################################
# installation
##########################################

# Libraries
install(TARGETS ${LIBRARY_OUTPUT_NAME} 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

# Binaries
install(TARGETS ${EXECUTABLE_NAME} 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# Header files
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

# Config
# install (FILES cmake-examples.conf
#     DESTINATION etc)
