/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "mtp.h"
#include "MtpDebug.h"

using namespace android;

BOOST_AUTO_TEST_CASE(DebugGetOperationCodeName)
{
    BOOST_CHECK_EQUAL( MtpDebug::getOperationCodeName(MTP_OPERATION_GET_DEVICE_INFO), "MTP_OPERATION_GET_DEVICE_INFO" );
}

BOOST_AUTO_TEST_CASE(DebugGetFormatCodeName)
{
    BOOST_CHECK_EQUAL( MtpDebug::getFormatCodeName(MTP_FORMAT_PNG), "MTP_FORMAT_PNG" );
}

BOOST_AUTO_TEST_CASE(DebugGetObjectPropCodeName)
{
    BOOST_CHECK_EQUAL( MtpDebug::getObjectPropCodeName(MTP_PROPERTY_STORAGE_ID), "MTP_PROPERTY_STORAGE_ID" );
}

BOOST_AUTO_TEST_CASE(DebugGetDevicePropCodeName)
{
    BOOST_CHECK_EQUAL( MtpDebug::getDevicePropCodeName(MTP_DEVICE_PROPERTY_BATTERY_LEVEL), "MTP_DEVICE_PROPERTY_BATTERY_LEVEL" );
}

BOOST_AUTO_TEST_CASE(DebugGetCodeNameUnknown)
{
    BOOST_CHECK_EQUAL( MtpDebug::getOperationCodeName (1), "UNKNOWN");
}
