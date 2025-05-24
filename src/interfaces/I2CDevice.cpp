#include "I2CDevice.hpp"

I2CDevice::I2CDevice(uint8_t &i2c_bus, uint8_t &i2c_address) { i2cInit(i2c_bus, i2c_address); }

I2CDevice::~I2CDevice() { i2cClose(); }

void I2CDevice::i2cInit(uint8_t &i2c_bus, uint8_t &i2c_address) {
    char filename[32];
    int ret;

    sprintf(filename, "/dev/i2c-%d", i2c_bus);
    fd = open(filename, O_RDWR);
    if (fd < 0) {
        std::cout << "Cannot open i2c bus " << i2c_bus << "\n";
        fd = 0;
        return;
    }

    _i2c_bus = i2c_bus;
    _i2c_address = i2c_address;

    ret = ioctl(fd, I2C_SLAVE, i2c_address);
    if (ret < 0) {
        std::cout << "Cannot detect address 0x" << std::setw(2) << std::setfill('0') << std::hex
                  << i2c_address << "\n";
        fd = 0;
    }
}

void I2CDevice::i2cClose() {
    if (fd >= 0) {
        close(fd);
    }
}

bool I2CDevice::isActive() {
    int ret;
    uint8_t test_reg = 0x0;
    uint8_t data;

    ret = write(fd, &test_reg, 1);
    ret = read(fd, &data, 1);
    return (ret != 1) ? false : true;
}
int I2CDevice::getFileDescriptor() { return fd; }

int I2CDevice::dumpToFile(uint8_t startAddress, uint8_t endAddress, char *fileName) {
    int ret;
    uint8_t reg;
    uint8_t data;

    std::ofstream file(fileName, std::ios::out);
    if (!file) {
        std::cerr << "Failed to create file" << std::endl;
        return -ENOENT;
    }

    if (!isActive()) {
        std::cerr << "Cannot access device" << std::endl;
        return -EINVAL;
    }

    reg = startAddress & 0xFF;
    /* Only use for 8-bit register sensor */
    while (reg <= endAddress) {
        ret = write(fd, &reg, 1);
        ret = read(fd, &data, 1);
        if (ret != 1)
            file << "0x" << std::setw(2) << std::setfill('0') << std::hex << reg << ": " << "--"
                 << std::endl;
        else
            file << "0x" << std::setw(2) << std::setfill('0') << std::hex << reg << ": 0x" << data
                 << std::endl;
        reg += 1;
    }

    file.close();
    return 0;
}

uint8_t I2CDevice::readByte(uint16_t registerAddress) {
    uint8_t reg;
    uint8_t data = 0;
    int ret;

    reg = registerAddress & 0x00FF;
    ret = write(fd, &reg, 1);

    ret = read(fd, &data, 1);
    if (ret != 1) {
        std::cout << "I2C Transaction failed!\n";
        return 0;
    }

    return data;
}

uint16_t I2CDevice::readWord(uint8_t registerAddress) {
    uint8_t reg[2] = {0};
    uint8_t data[2] = {0};
    int ret;

    reg[0] = registerAddress >> 8;
    reg[1] = registerAddress & 0x00FF;
    ret = write(fd, reg, 2);

    ret = read(fd, data, 2);
    if (ret != 2) {
        std::cout << "I2C Transaction failed!\n";
        return 0;
    } else
        return data[0] << 8 | data[1];
}

uint32_t I2CDevice::readDoubleWord(uint16_t registerAddress) {
    uint8_t reg[2] = {0};
    uint8_t data[4] = {0};
    int ret;

    reg[0] = registerAddress >> 8;
    reg[1] = registerAddress & 0x00FF;
    ret = write(fd, reg, 2);

    ret = read(fd, data, 4);
    if (ret != 4) {
        std::cout << "I2C Transaction failed!\n";
        return 0;
    } else
        return data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
}

bool I2CDevice::writeByte(uint8_t registerAddress, uint8_t data) {
    uint8_t buffer[2] = {0};
    buffer[0] = registerAddress & 0x00FF;
    buffer[1] = data;

    if (write(fd, buffer, 2) != 2) {
        std::cout << "I2C Transaction failed!";
        return false;
    }
    return true;
}

bool I2CDevice::writeWord(uint16_t registerAddress, uint16_t data) {
    uint8_t buffer[4] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;
    buffer[2] = data >> 8;
    buffer[3] = data & 0x00FF;

    if (write(fd, buffer, 4) != 4) {
        std::cout << "I2C Transaction failed!";
        return false;
    }
    return true;
}

bool I2CDevice::writeDoubleWord(uint16_t registerAddress, uint32_t data) {
    uint8_t buffer[6] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;
    buffer[2] = (data >> 24) & 0x00FF;
    buffer[3] = (data >> 16) & 0x00FF;
    buffer[4] = (data >> 8) & 0x00FF;
    buffer[5] = data & 0x00FF;

    if (write(fd, buffer, 6) != 6) {
        std::cout << "I2C Transaction failed!";
        return false;
    }
    return true;
}

std::ostream &operator<<(std::ostream &output, I2CDevice &that) {
    return output << " bus-" << std::hex << that._i2c_bus << " address-0x" << std::hex
                  << that._i2c_address << "active-" << (that.isActive() ? "yes" : "no") << " ";
}
