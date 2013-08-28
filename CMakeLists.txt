project(mtp)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

add_definitions(-DMTP_DEVICE -DMTP_HOST)

set(MTP_VERSION_MAJOR 1)
set(MTP_VERSION_MINOR 0)
set(MTP_VERSION_PATCH 0)

find_package(Qt5Core REQUIRED)

set(
    MTP_HEADERS
    include/MtpDatabase.h
    include/MtpDataPacket.h
    include/MtpDebug.h
    include/MtpDevice.h
    include/MtpDeviceInfo.h
    include/MtpEventPacket.h
    include/mtp.h
    include/MtpObjectInfo.h
    include/MtpPacket.h
    include/MtpProperty.h
    include/MtpRequestPacket.h
    include/MtpResponsePacket.h
    include/MtpServer.h
    include/MtpStorage.h
    include/MtpStorageInfo.h
    include/MtpStringBuffer.h
    include/MtpTypes.h
    include/MtpUtils.h
)

set(
    MTP_SRCS
    src/MtpDataPacket.cpp
    src/MtpDebug.cpp
    src/MtpDevice.cpp
    src/MtpDeviceInfo.cpp
    src/MtpEventPacket.cpp
    src/MtpObjectInfo.cpp
    src/MtpPacket.cpp
    src/MtpProperty.cpp
    src/MtpRequestPacket.cpp
    src/MtpResponsePacket.cpp
    src/MtpServer.cpp
    src/MtpStorage.cpp
    src/MtpStorageInfo.cpp
    src/MtpStringBuffer.cpp
    src/MtpUtils.cpp)
    
include_directories(
    include/
    libusbhost/include
    ${Qt5Core_INCLUDE_DIRS}
)

add_library(
    mtp SHARED
    ${MTP_SRCS}
)

set_target_properties(
  mtp

  PROPERTIES
  VERSION ${MTP_VERSION_MAJOR}.${MTP_VERSION_MINOR}.${MTP_VERSION_PATCH}
  SOVERSION ${MTP_VERSION_MAJOR}
)

set_target_properties(
  mtp
  PROPERTIES PUBLIC_HEADER "${MTP_HEADERS}"
)

install(
  TARGETS mtp
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib
  PUBLIC_HEADER DESTINATION include/mtp
)

add_subdirectory(libusbhost)
add_subdirectory(examples)
