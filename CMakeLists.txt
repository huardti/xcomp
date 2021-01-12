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

# Warning setup
if(CMAKE_COMPILER_IS_GNUCXX)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
        message(FATAL_ERROR "Require gcc 5.0.0 or newer.")
    endif()

    set(W
        ${W}
        -pedantic
        -Wall
        -Wextra

        -Wcast-align
        -Wcast-qual
        -Wconditionally-supported
        -Wconversion
        -Wdate-time
        -Wdisabled-optimization
        -Wdouble-promotion
        -Wfloat-equal
        -Wformat-security
        -Wformat-signedness
        -Wformat=2
        -Winit-self
        #-Winline
        -Winvalid-pch
        -Wlogical-op
        -Wmissing-declarations
        -Wmissing-include-dirs
        -Wnoexcept
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpacked
        -Wpointer-arith
        -Wredundant-decls
        -Wshadow
        -Wstrict-null-sentinel
        -Wswitch-default
        -Wswitch-enum
        -Wtrampolines
        -Wundef
        -Wunreachable-code
        -Wunused-macros
        -Wuseless-cast
        -Wvector-operation-performance
        -Wvla
        -Wwrite-strings
        -Wzero-as-null-pointer-constant

        -Wno-unused-but-set-variable

        -Wsuggest-attribute=const
        -Wsuggest-attribute=noreturn
        -Wsuggest-attribute=pure
        -Wsuggest-final-methods
        -Wsuggest-final-types
        -Wsuggest-override
    )

    set(RUNTIME_CHECKS
        -fsanitize=address
        -fsanitize=float-cast-overflow
        -fsanitize=float-divide-by-zero
        -fsanitize=leak
        -fsanitize=undefined
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls
    )

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6)
        set(W
            ${W}
            -Wduplicated-cond
            -Wnull-dereference
        )
    endif()

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7)
        set(W
            ${W}
            -Waligned-new
            -Walloc-zero
            -Walloca
            -Wduplicated-branches
            -Wformat-overflow=2
        )
    endif()

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
        set(W
            ${W}
            -Wcast-align=strict
            -Wclass-memaccess
            -Wmultistatement-macros

            -fdiagnostics-show-template-tree
            -fno-elide-type
        )
        set(RUNTIME_CHECKS
            ${RUNTIME_CHECKS}
            -fsanitize=pointer-compare
            -fsanitize=pointer-subtract
        )
    endif()

    # No new warning for gcc 9

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10)
        set(W
            ${W}
            -Warith-conversion
        )
    endif()

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
        -Wno-covered-switch-default
        -Wno-documentation
        -Wno-documentation-unknown-command
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-padded
        -Wno-shadow
        -Wno-shadow-field
        -Wno-switch-default
        -Wno-switch-enum
        -Wno-used-but-marked-unused
        -Wno-weak-vtables
    )
    set(RUNTIME_CHECKS
        -fsanitize=address
        -fsanitize=float-cast-overflow
        -fsanitize=float-divide-by-zero
        -fsanitize=leak
        -fsanitize=undefined
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls
    )
else()
    message(WARNING "Warning: your compiler has not been setup by the CMake script, do not expect it to work")
endif()

option(FSANITIZE_FLAGS "Build with -fsanitize= flags" ON)
if(CMAKE_BUILD_TYPE STREQUAL Release OR NOT FSANITIZE_FLAGS)
    set(RUNTIME_CHECKS ) # Clear the flags    
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
