
# MtpUtils
add_executable(
    test-utils
    TestMtpUtils.cpp
)

target_link_libraries(
    test-utils
    mtp
    usbhost
    ${Boost_LIBRARIES}
    ${Boost_unit_test_framework_LIBRARIES}
)
add_test(test-utils test-utils)

# MtpDebug
add_executable(
    test-debug
    TestMtpDebug.cpp
)

target_link_libraries(
    test-debug
    mtp
    usbhost
    ${Boost_LIBRARIES}
    ${Boost_unit_test_framework_LIBRARIES}
)
add_test(test-debug test-debug)

# MtpPacket
add_executable(
    test-packet
    TestMtpPacket.cpp
)

target_link_libraries(
    test-packet
    mtp
    usbhost
    ${Boost_LIBRARIES}
    ${Boost_unit_test_framework_LIBRARIES}
)
add_test(test-packet test-packet)

# MtpProperty
add_executable(
    test-property
    TestMtpProperty.cpp
)

target_link_libraries(
    test-property
    mtp
    usbhost
    ${Boost_LIBRARIES}
    ${Boost_unit_test_framework_LIBRARIES}
)
add_test(test-property test-property)

# MtpStorage
add_executable(
    test-storage
    TestMtpStorage.cpp
)

target_link_libraries(
    test-storage
    mtp
    usbhost
    ${Boost_LIBRARIES}
    ${Boost_unit_test_framework_LIBRARIES}
)
add_test(test-storage test-storage)

# MtpServer
add_executable(
    test-server
    TestMtpServer.cpp
)

target_link_libraries(
    test-server
    mtp
    usbhost
    ${Boost_LIBRARIES}
    ${Boost_unit_test_framework_LIBRARIES}
)
add_test(test-server test-server)

