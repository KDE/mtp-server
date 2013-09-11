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
#include "MtpPacket.h"

using namespace android;

BOOST_AUTO_TEST_CASE(Packet)
{
    MtpPacket *p = new MtpPacket (4);

    BOOST_REQUIRE(p);
}

BOOST_AUTO_TEST_CASE(PacketReset)
{
    MtpPacket *p = new MtpPacket (MTP_CONTAINER_PARAMETER_OFFSET + 4);
    uint32_t value = UINT32_MAX;
    uint32_t result;

    BOOST_REQUIRE(p);

    p->setParameter(1, value);
    result = p->getParameter(1);
    BOOST_CHECK_EQUAL (value, result);

    p->reset();

    result = p->getParameter(1);
    BOOST_CHECK (value != result);
    BOOST_CHECK (result == 0);
}
