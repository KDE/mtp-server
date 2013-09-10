/*
 * Copyright (C) 2010 The Android Open Source Project
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

#include <cstdlib>

// #include <cutils/tztime.h>
#include "MtpUtils.h"

using namespace android;

BOOST_AUTO_TEST_CASE(UtilsParseDateTime)
{
    time_t seconds;

    setenv("TZ", "UTC", 1);

    parseDateTime("20130909T114143", seconds);
    BOOST_CHECK_EQUAL(seconds, 1378726903l);
}

BOOST_AUTO_TEST_CASE(UtilsFormatDateTime)
{
    time_t seconds = 1378726903;
    char buffer[25];
    char *expected = "20130909T114143";

    setenv("TZ", "UTC", 1);

    formatDateTime(seconds, buffer, 25);
    BOOST_CHECK_EQUAL(strcmp(expected, buffer), 0);
}
