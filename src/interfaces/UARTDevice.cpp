#include "UARTDevice.hpp"

UARTDevice::UARTDevice(const std::string& device, int baudrate) {
    int ret;
    fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        std::cerr << "Cannot open device " << device << std::endl;
        fd = 0;
        return;
    }
    _baudrate = baudrate;
    configurePort(baudrate);
}

UARTDevice::~UARTDevice() {
    this->flush();
    if (fd >= 0) {
        close(fd);
    }
}

int UARTDevice::configurePort(int baudrate) {
    struct termios option;
    speed_t speed;
    int ret;

    if (tcgetattr(fd, &option) != 0) {
        std::cerr << "Cannot get current settings for port\n";
        return -EINVAL;
    }

    // Set baud rate
    switch (baudrate) {
        case 9600:
            speed = B9600;
            break;
        case 19200:
            speed = B19200;
            break;
        case 38400:
            speed = B38400;
            break;
        case 57600:
            speed = B57600;
            break;
        case 115200:
            speed = B115200;
            break;
        default:
            std::cerr << "Invalid baurate\n";
            return -EINVAL;
    }

    cfsetospeed(&option, speed);
    cfsetispeed(&option, speed);

    // 8N1 mode
    option.c_cflag &= ~PARENB;  // No parity
    option.c_cflag &= ~CSTOPB;  // One stop bit
    option.c_cflag &= ~CSIZE;
    option.c_cflag |= CS8;             // 8 bits
    option.c_cflag &= ~CRTSCTS;        // No flow control
    option.c_cflag |= CREAD | CLOCAL;  // Enable receiver, ignore modem lines

    option.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Raw input
    option.c_iflag &= ~(IXON | IXOFF | IXANY);          // No software flow control
    option.c_oflag &= ~OPOST;                           // Raw output

    option.c_cc[VMIN] = 0;
    option.c_cc[VTIME] = 10;  // Timeout in deciseconds (1s)
    this->flush();
    if (tcsetattr(fd, TCSANOW, &option) != 0) {
        return -EINVAL;
    }
    return 0;
}

int UARTDevice::reconfigurePort(int baudrate) {
    if (fd >= 0)
        return configurePort(baudrate);
    else
        return -EINVAL;
}

int UARTDevice::writeData(const std::string& data) {
    int ret;
    ret = write(fd, data.c_str(), data.size());
    if (ret < 0) {
        std::cerr << "UART write failed\n";
        return -EINVAL;
    }
    return 0;
}

int UARTDevice::readData(std::string& data, size_t num_bytes) {
    int ret;
    char* buffer = new char[data.size()];

    /* Convert string to char* */
    std::strcpy(buffer, data.c_str());
    if (num_bytes > data.size()) {
        std::cerr << "invalid num_bytes\n";
        ret = -EINVAL;
        goto out;
    }

    ret = read(fd, buffer, num_bytes);
    if (ret < 0) {
        std::cerr << "UART read failed\n";
        ret = -EINVAL;
    } else if (ret == 0) {
        std::cerr << "No data received (timeout)\n";
        ret = -ETIMEDOUT;
    } else {
        /* Convert char* back to string */
        data = std::string(buffer);
        ret = 0;
    }

out:
    delete[] buffer;
    return ret;
}

void UARTDevice::flush() {
    if (fd >= 0) tcflush(fd, TCIOFLUSH);
}