#include "MtpImageDatabase.h"

#include <MtpServer.h>
#include <MtpStorage.h>

#include <iostream>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace
{
struct FileSystemConfig
{
    static const int file_group = 1023; // Taken from android's config. Might need to be adjusted.
    static const int file_perm = 0664;
    static const int directory_perm = 0755;
};

android::MtpStorage* removable_storage = new android::MtpStorage(
    MTP_STORAGE_REMOVABLE_RAM, 
    "/home/phablet/Pictures",
    "Image storage", 
    1024 * 1024 * 1024 * 200,
    true,
    1024 * 1024 * 1024);
}

int main(int argc, char** argv)
{
    int fd = open("/dev/mtp_usb", O_RDWR);
    
    if (fd < 0)
    {
        std::cout << "Error opening /dev/mtp_usb, aborting now..." << std::endl;
    }
        
    std::shared_ptr<android::MtpServer> server
    {
        new android::MtpServer(
            fd, 
            new android::MtpImageDatabase(),
            false, 
            FileSystemConfig::file_group, 
            FileSystemConfig::file_perm, 
            FileSystemConfig::directory_perm)
    };
    server->addStorage(removable_storage);
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
