/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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

#include <hybris/properties/properties.h>
#include <glog/logging.h>

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
    char product_name[PROP_VALUE_MAX];
    int fd = open("/dev/mtp_usb", O_RDWR);

    google::InitGoogleLogging(argv[0]);

    LOG(INFO) << "MTP server starting...";

    if (fd < 0)
    {
        LOG(FATAL) << "Error opening /dev/mtp_usb, aborting now...";
    }
 
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

    property_get ("ro.product.model", product_name, "Ubuntu Touch device");

    home_storage = new android::MtpStorage(
        MTP_STORAGE_FIXED_RAM, 
        userdata->pw_dir,
	product_name,
        1024 * 1024 * 100,  /* 100 MB reserved space, to avoid filling the disk */
        false,
        1024 * 1024 * 1024 * 2  /* 2GB arbitrary max file size */);

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
