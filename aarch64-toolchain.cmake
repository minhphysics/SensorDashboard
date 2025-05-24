# aarch64-toolchain.cmake
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR aarch64)

# Specify the cross compiler
SET(CMAKE_C_COMPILER   aarch64-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++-9)

# Optional: Add sysroot (if needed)
# SET(CMAKE_SYSROOT /usr/aarch64-linux-gnu)

# Optional: Set search paths
# SET(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)

# Don’t search for programs on the target system
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)