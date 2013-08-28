add_definitions(-DMTP_DEVICE -DMTP_HOST)

add_executable(
    mtp-server
    server.cpp
)

set_target_properties(mtp-server PROPERTIES COMPILE_FLAGS -fPIC)

target_link_libraries(
    mtp-server
    mtp
    usbhost
    ${Qt5Core_LIBRARIES}
)

install(
  TARGETS mtp-server
  RUNTIME DESTINATION bin
)
