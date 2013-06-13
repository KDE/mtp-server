add_definitions(-DMTP_DEVICE -DMTP_HOST)

add_executable(
    mtp-server
    server.cpp
)

target_link_libraries(
    mtp-server
    mtp
    usbhost
)

install(
  TARGETS mtp-server
  RUNTIME DESTINATION bin
)