#ifndef I2CDEVICE_H
#define I2CDEVICE_H

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cerrno>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "macro.hpp"

class I2CDevice {
   private:
    int fd;
    uint8_t _i2c_bus;
    uint8_t _i2c_address;

    void i2cInit(uint8_t &i2c_bus, uint8_t &i2c_address);
    void i2cClose();

   public:
    explicit I2CDevice(uint8_t &i2c_bus, uint8_t &i2c_address);
    ~I2CDevice();

    bool isActive();
    int getFileDescriptor();
    int dumpToFile(uint8_t startAddress, uint8_t endAddress, char *fileName);

    uint8_t readByte(uint16_t registerAddress);
    uint16_t readWord(uint8_t registerAddress);
    uint32_t readDoubleWord(uint16_t registerAddress);

    bool writeByte(uint8_t registerAddress, uint8_t data);
    bool writeWord(uint16_t registerAddress, uint16_t data);
    bool writeDoubleWord(uint16_t registerAddress, uint32_t data);

    friend std::ostream &operator<<(std::ostream &, I2CDevice &);
};
#endif  // I2CDEVICE_H