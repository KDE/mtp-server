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

#include "mtp.h"
#include "MockMtpDatabase.h"
#include "MtpServer.h"
#include "MtpStorage.h"

using namespace android;

BOOST_AUTO_TEST_CASE(ServerConstructor)
{
    MtpDatabase *db = new MockMtpDatabase ();
    MtpServer *server = new MtpServer (0, db, false, 0, 0, 0);

    BOOST_CHECK(server);
}

BOOST_AUTO_TEST_CASE(ServerAddGetStorage)
{
    MtpDatabase *db = new MockMtpDatabase ();
    MtpServer *server = new MtpServer (0, db, false, 0, 0, 0);
    MtpStorage *storage = new MtpStorage (666, "/tmp", "Test storage", 0, false, 64);

    server->addStorage(storage);

    BOOST_CHECK(server->getStorage(666) != NULL);
}

BOOST_AUTO_TEST_CASE(ServerGetStorageNull)
{
    MtpDatabase *db = new MockMtpDatabase ();
    MtpServer *server = new MtpServer (0, db, false, 0, 0, 0);

    BOOST_CHECK(server->getStorage(666) == NULL);
}

BOOST_AUTO_TEST_CASE(ServerHasStorageTrue)
{
    MtpDatabase *db = new MockMtpDatabase ();
    MtpServer *server = new MtpServer (0, db, false, 0, 0, 0);
    MtpStorage *storage = new MtpStorage (666, "/tmp", "Test storage", 0, false, 64);

    server->addStorage(storage);

    BOOST_CHECK(server->hasStorage(666));
}

BOOST_AUTO_TEST_CASE(ServerHasStorageFalse)
{
    MtpDatabase *db = new MockMtpDatabase ();
    MtpServer *server = new MtpServer (0, db, false, 0, 0, 0);

    BOOST_CHECK(server->hasStorage(667) == false);
}
