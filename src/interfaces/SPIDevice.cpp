#include "SPIDevice.hpp"

SPIDevice::SPIDevice(const std::string& device, uint32_t speedHz = 500000,
                     uint8_t mode = SPI_MODE_0, uint8_t bitsPerWord = 8)
    : fd(-1), _speed(speedHz), _mode(mode), _bits(bitsPerWord) {
    int ret;
    fd = open(device.c_str(), O_RDWR);
    if (fd < 0) {
        std::cerr << "Failed to open SPI Device\n";
        fd = 0;
        return;
    }
    // Configure device
    ret = ioctl(fd, SPI_IOC_WR_MODE, &_mode);
    if (ret) {
        std::cerr << "Failed to set SPI mode\n";
        return;
    }
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &_bits);
    if (ret) {
        std::cerr << "Failed to set SPI data width\n";
        return;
    }
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &_speed);
    if (ret) {
        std::cerr << "Failed to set SPI speed\n";
        return;
    }
}

SPIDevice::~SPIDevice() {
    if (fd >= 0) close(fd);
}

// Full-duplex transfer: send and receive data
std::vector<uint8_t> SPIDevice::transfer(const std::vector<uint8_t>& txBuffer) {
    std::vector<uint8_t> rxBuffer(txBuffer.size());
    int ret;

    struct spi_ioc_transfer xfer = {
        .tx_buf = (unsigned long)txBuffer.data(),
        .rx_buf = (unsigned long)rxBuffer.data(),
        .len = static_cast<uint32_t>(txBuffer.size()),
        .speed_hz = _speed,
        .delay_usecs = 0,
        .bits_per_word = _bits,
    };
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
    if (ret < 1) {
        std::cerr << "Failed to transfer SPI\n";
    }
    return rxBuffer;
}

// Send only
int SPIDevice::sendOnly(const std::vector<uint8_t>& txBuffer) {
    int ret;

    struct spi_ioc_transfer xfer = {
        .tx_buf = (unsigned long)txBuffer.data(),
        .rx_buf = (unsigned long)0x00,
        .len = static_cast<uint32_t>(txBuffer.size()),
        .speed_hz = _speed,
        .delay_usecs = 0,
        .bits_per_word = _bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
    if (ret < 1) {
        std::cerr << "Failed to transfer SPI\n";
        return ret;
    }
    return 0;
}

// Receive only
int SPIDevice::receiveOnly(std::vector<uint8_t>& rxBuffer) {
    std::vector<uint8_t> txBuffer(rxBuffer.size(), 0);
    int ret;

    struct spi_ioc_transfer xfer = {
        .tx_buf = (unsigned long)txBuffer.data(),
        .rx_buf = (unsigned long)rxBuffer.data(),
        .len = static_cast<uint32_t>(rxBuffer.size()),
        .speed_hz = _speed,
        .delay_usecs = 0,
        .bits_per_word = _bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
    if (ret < 1) {
        std::cerr << "Failed to transfer SPI\n";
        return ret;
    }

    return 0;
}
