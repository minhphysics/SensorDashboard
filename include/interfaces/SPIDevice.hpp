#ifndef SPIDEVICE_H
#define SPIDEVICE_H

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "macro.hpp"

class SPIDevice {
   private:
    int fd;
    uint32_t _speed;
    uint8_t _mode;
    uint8_t _bits;

   public:
    SPIDevice(const std::string& device, uint32_t speedHz, uint8_t mode, uint8_t bitsPerWord);
    ~SPIDevice();

    // Full-duplex transfer: send and receive data
    std::vector<uint8_t> transfer(const std::vector<uint8_t>& txBuffer);

    // Send only
    int sendOnly(const std::vector<uint8_t>& txBuffer);

    // Receive only
    int receiveOnly(std::vector<uint8_t>& rxBuffer);
};
#endif  // SPIDEVICE_H