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

#include "UbuntuMtpDatabase.h"

#include <MtpServer.h>
#include <MtpStorage.h>

#include <iostream>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>

namespace
{
struct FileSystemConfig
{
    static const int file_perm = 0664;
    static const int directory_perm = 0755;
};

android::MtpStorage* home_storage;

}

int main(int argc, char** argv)
{
    struct passwd *userdata = getpwuid (getuid());
    int fd = open("/dev/mtp_usb", O_RDWR);
    
    if (fd < 0)
    {
        std::cout << "Error opening /dev/mtp_usb, aborting now..." << std::endl;
    }
        
    home_storage = new android::MtpStorage(
        MTP_STORAGE_FIXED_RAM, 
        userdata->pw_dir,
        userdata->pw_name, 
        1024 * 1024 * 100,  /* 100 MB reserved space, to avoid filling the disk */
        false,
        1024 * 1024 * 1024 * 2  /* 2GB arbitrary max file size */);

    std::shared_ptr<android::MtpServer> server
    {
        new android::MtpServer(
            fd, 
            new android::UbuntuMtpDatabase(userdata->pw_dir),
            false, 
            userdata->pw_gid, 
            FileSystemConfig::file_perm, 
            FileSystemConfig::directory_perm)
    };
    server->addStorage(home_storage);
    server->run();

    /*
      std::thread t{[&server](){ server->run(); }};

      sigset_t signal_set;
      sigemptyset(&signal_set);
      sigaddset(&signal_set, SIGINT);
      int signal;
      sigwait(&signal_set, &signal);

      if (t.joinable())
      t.join();
    */
    
}
