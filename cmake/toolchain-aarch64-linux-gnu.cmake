# Cross-compilation toolchain for Linux aarch64 (ARM64)
# ============================================================
# PURPOSE
# ============================================================
# Host:   x86_64 Linux
# Target: aarch64 Linux
# ============================================================
# REQUIRED PACKAGES (Debian / Ubuntu)
# ============================================================
# sudo apt install \
# gcc-aarch64-linux-gnu \
# g++-aarch64-linux-gnu \
# binutils-aarch64-linux-gnu

# ============================================================
# TARGET SYSTEM
# ============================================================

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# ============================================================
# CROSS COMPILERS
# ============================================================

set(CMAKE_C_COMPILER   aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_STRIP        aarch64-linux-gnu-strip)

# ============================================================
# SYSROOT
# ============================================================

set(CMAKE_SYSROOT /)

list(APPEND CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)
list(APPEND CMAKE_FIND_ROOT_PATH /usr/lib/aarch64-linux-gnu)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

# ============================================================
# LINKER FLAGS
# ============================================================

set(CMAKE_EXE_LINKER_FLAGS_INIT "--sysroot=/")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "--sysroot=/")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "--sysroot=/")
