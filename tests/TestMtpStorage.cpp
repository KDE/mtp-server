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
#include "MtpStorage.h"

using namespace android;

BOOST_AUTO_TEST_CASE(StorageConstructor)
{
    MtpStorage *s = new MtpStorage (666, "/tmp", "Test storage", 0, false, 64);

    BOOST_CHECK(s);
}

BOOST_AUTO_TEST_CASE(StorageGetTypeFixed)
{
    MtpStorage *s = new MtpStorage (666, "/tmp", "Test storage", 0, false, 64);

    BOOST_CHECK (s->getType() == MTP_STORAGE_FIXED_RAM);
}

BOOST_AUTO_TEST_CASE(StorageGetTypeRemovable)
{
    MtpStorage *s = new MtpStorage (666, "/tmp", "Test storage", 0, true, 64);

    BOOST_CHECK (s->getType() == MTP_STORAGE_REMOVABLE_RAM);
}

BOOST_AUTO_TEST_CASE(StorageGetFileSystemType)
{
    MtpStorage *s = new MtpStorage (666, "/tmp", "Test storage", 0, true, 64);

    BOOST_CHECK (s->getFileSystemType() == MTP_STORAGE_FILESYSTEM_HIERARCHICAL);
}

BOOST_AUTO_TEST_CASE(StorageGetAccessCapa)
{
    MtpStorage *s = new MtpStorage (666, "/tmp", "Test storage", 0, true, 64);

    BOOST_CHECK (s->getAccessCapability() == MTP_STORAGE_READ_WRITE);
}

BOOST_AUTO_TEST_CASE(StorageGetMaxCapacity)
{
    MtpStorage *s = new MtpStorage (666, "/tmp", "Test storage", 0, true, 64);

    BOOST_CHECK (s->getMaxCapacity() > 0);
}

BOOST_AUTO_TEST_CASE(StorageGetMaxCapacityInvalidPath)
{
    MtpStorage *s = new MtpStorage (666, "", "Test storage", 0, true, 64);

    BOOST_CHECK (s->getMaxCapacity() == -1);
}

BOOST_AUTO_TEST_CASE(StoageGetFreeSpace)
{
    MtpStorage *s = new MtpStorage (666, "/", "Test storage", 0, true, 64);

    BOOST_CHECK (s->getFreeSpace() != -1);
}

BOOST_AUTO_TEST_CASE(StorageGetDescription)
{
    MtpStorage *s = new MtpStorage (666, "/", "Test storage", 0, true, 64);

    BOOST_CHECK_EQUAL( strcmp( s->getDescription(), "Test storage" ), 0);
}
