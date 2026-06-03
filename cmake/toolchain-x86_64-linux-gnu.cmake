# Native compilation toolchain for Linux x86_64 (AMD64)
# ============================================================
# PURPOSE
# ============================================================
# Host:   x86_64 Linux
# Target: x86_64 Linux
# ============================================================
# REQUIRED PACKAGES (Debian / Ubuntu)
# ============================================================
# sudo apt install \
# gcc \
# g++ \
# binutils

# ============================================================
# TARGET SYSTEM
# ============================================================

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# ============================================================
# COMPILERS
# ============================================================

set(CMAKE_C_COMPILER   gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_STRIP strip)
