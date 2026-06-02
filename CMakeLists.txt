cmake_minimum_required(VERSION 3.28)

project(
    TrueContext
    VERSION 0.1.0
    DESCRIPTION "Git-independent project context collector"
    LANGUAGES CXX
)

set(
    PROJECT_PUBLIC_INCLUDE_DIR
    "${CMAKE_SOURCE_DIR}/include"
    CACHE INTERNAL
    "Include root for <truecontext/...> headers"
)

# ============================================================
# Options
# ============================================================

option(
    ENABLE_THREADS
    "Enable Threads::Threads linkage"
    OFF
)

option(
    ENABLE_IPO
    "Enable IPO/LTO in Release"
    ON
)

option(
    ENABLE_SANITIZERS
    "Enable Address/Undefined sanitizers"
    OFF
)

option(
    WARNINGS_AS_ERRORS
    "Treat warnings as errors"
    OFF
)

# ============================================================
# Global C++ settings
# ============================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(
        FATAL_ERROR
        "Unsupported compiler '${CMAKE_CXX_COMPILER_ID}'. Supports GCC and Clang only."
    )
endif()

# ============================================================
# Output directories
# ============================================================

set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}
)

set(
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/lib
)

set(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/lib
)

# ============================================================
# Project-wide dependencies and capabilities
# ============================================================

if(ENABLE_THREADS)
    find_package(Threads REQUIRED)
endif()

set(PROJECT_IPO_SUPPORTED FALSE)

if(ENABLE_IPO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT PROJECT_IPO_SUPPORTED)
endif()

# ============================================================
# Sanitizers
# ============================================================

function(enable_sanitizers target_name)
    target_compile_options(
        ${target_name}
        PRIVATE
        -fsanitize=address
        -fsanitize=undefined
    )

    target_link_options(
        ${target_name}
        PRIVATE
        -fsanitize=address
        -fsanitize=undefined
    )
endfunction()

# ============================================================
# Common target setup
# ============================================================

function(configure_target target_name)

    target_include_directories(
        ${target_name}
        PRIVATE
        "${PROJECT_PUBLIC_INCLUDE_DIR}"
    )

    target_compile_features(
        ${target_name}
        PRIVATE
        cxx_std_23
    )

    target_compile_options(
        ${target_name}
        PRIVATE

        -Wall
        -Wextra
        -Wpedantic

        -Wshadow
        -Wconversion
        -Wsign-conversion

        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-g>

        $<$<CONFIG:Release>:-O3>
    )

    if(WARNINGS_AS_ERRORS)
        target_compile_options(
            ${target_name}
            PRIVATE
            -Werror
        )
    endif()

    target_compile_definitions(
        ${target_name}
        PRIVATE
        $<$<CONFIG:Debug>:DEBUG_MODE>
    )

    if(ENABLE_THREADS)
        target_link_libraries(
            ${target_name}
            PRIVATE
            Threads::Threads
        )
    endif()

    if(
        ENABLE_SANITIZERS
        AND CMAKE_BUILD_TYPE STREQUAL "Debug"
    )
        enable_sanitizers(${target_name})
    endif()

    if(ENABLE_IPO AND PROJECT_IPO_SUPPORTED)
        set_property(
            TARGET ${target_name}
            PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
        )
    endif()

endfunction()

# ============================================================
# Targets (src/CMakeLists.txt)
# ============================================================

add_subdirectory(src)

# ============================================================
# Tests
# ============================================================

enable_testing()

add_test(
    NAME smoke_help
    COMMAND true_context --help
)

add_test(
    NAME smoke_version
    COMMAND true_context --version
)
