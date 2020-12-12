cmake_minimum_required(VERSION 3.7...3.19)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(xcomp
    VERSION 0.1
    DESCRIPTION "description of xcomp"
    LANGUAGES CXX
)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDANT_CODE ON)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Debug (-DCMAKE_BUILD_TYPE=Release) if you need release build")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_BUILD_TYPE STREQUAL Release)
    set(W
        $<$<CXX_COMPILER_ID:MSVC>: /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>: -Werror>
    )
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(W
        ${W}
        -Wall
        -Wextra
        -pedantic
    )
elseif(MSVC)
    set(W
        ${W}
        /DMSVC
        /W4
        /GR
        /EHs
        /D_CRT_SECURE_NO_DEPRECATE
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(W
        ${W}
        -pedantic
        -Weverything
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-padded
        -Wno-global-constructors
        -Wno-exit-time-destructors
        -Wno-weak-vtables
        -Wno-covered-switch-default
        -Wno-documentation-unknown-command
        -Wno-switch-default
        -Wno-switch-enum
        -Wno-shadow-field
        -Wno-documentation
        -Wno-shadow
    )
else()
    message(WARNING "Warning: your compiler has not been setup by the CMake script, do not expect it to work")
endif()

# set OS preprocessor defines
if (APPLE)
    add_definitions(-DMACOSX)
elseif (UNIX)
    add_definitions(-DLINUX)
elseif (WINDOWS)
    add_definitions(-DWIN32)
endif()

# Unit tests
add_subdirectory(3rd_party/google-test)
find_package(GTest REQUIRED)

# Enable CTest testing
enable_testing()

# Add testing executables
add_subdirectory(tests)

# executable build rules
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(preprocessor)
add_subdirectory(xcomp)
