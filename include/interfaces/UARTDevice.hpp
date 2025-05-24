#ifndef UARTDEVICE_H
#define UARTDEVICE_H

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "macro.hpp"

class UARTDevice {
   private:
    int fd;
    int _baudrate;
    int configurePort(int baudrate);  // Only configure baudrate for now
   public:
    UARTDevice(const std::string& device, int baudrate);
    ~UARTDevice();
    int reconfigurePort(int baudrate);
    int writeData(const std::string& data);
    int readData(std::string& data, size_t num_bytes);

    void flush();
};
#endif  // UARTDEVICE_H