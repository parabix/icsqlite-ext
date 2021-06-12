# Copyright (c) 2021 International Characters.
# This software is licensed to the public under the Open Software License 3.0.

cmake_minimum_required(VERSION 2.8)
project(icgrep_ext)

###   Build Options   ###
option(SQLITE_BUILD_PATH  "SQLite build directory")
option(PARABIX_PATH "Parabix root directory")
option(LLVM_PATH "LLVM root directory")
option(ADD_EXEC "Generate executable (for testing purposes)" OFF)

# Build Artifact Configuration
set(ARTIFACT_TYPE "Static" CACHE STRING "Build frameword as shared (Shared) or static (Static) libraries")

# Validate framework build style
set(VALID_BUILD_TYPE "Shared" "Static")
list(FIND VALID_BUILD_TYPE ${ARTIFACT_TYPE} __at_index)
if(__at_index EQUAL -1)
    message(FATAL_ERROR "ARTIFACT_TYPE must be either Shared or Static")
endif()

# Ensure custom FindXYZ.cmake files are found
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

###   Import Dependency: LLVM   ###
if (LLVM_PATH)
find_package(LLVM REQUIRED CONFIG PATHS ${LLVM_PATH} NO_DEFAULT_PATH)
else()
find_package(LLVM REQUIRED CONFIG)
endif()
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} in: ${LLVM_DIR}")
set(LLVM_ALL_TARGETS X86)
llvm_map_components_to_libnames(REQ_LLVM_LIBRARIES ${LLVM_ALL_TARGETS} mcjit native IRReader Linker)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})
math(EXPR LLVM_VERSION_INTEGER "${LLVM_VERSION_MAJOR} * 10000 + ${LLVM_VERSION_MINOR} * 100 + ${LLVM_VERSION_PATCH}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLLVM_VERSION_INTEGER=${LLVM_VERSION_INTEGER}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLLVM_VERSION_major=${LLVM_VERSION_MAJOR} -DLLVM_VERSION_MINOR=${LLVM_VERSION_MINOR}")
message(STATUS "LLVM Version Integer: ${LLVM_VERSION_INTEGER}")


###   Import Dependency: Boost   ###

set(Boost_USE_STATIC_LIBS ${USE_STATIC_LIBS})
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.61 REQUIRED COMPONENTS system filesystem iostreams)
message(STATUS "Found Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
include_directories("${Boost_INCLUDE_DIRS}")
link_directories(${Boost_LIBRARY_DIRS})


###   Import Dependency: Z3   ###

find_package(Z3 REQUIRED) # libz3-dev
message(STATUS "Found Z3 ${Z3_VERSION_STRING} in: ${Z3_INCLUDE_DIR}")
include_directories(${Z3_INCLUDE_DIRS})
string(REGEX REPLACE "([0-9]+)[.].*" "\\1" Z3_VERSION_MAJOR "${Z3_VERSION_STRING}")
string(REGEX REPLACE "[0-9]+[.]([0-9]+).*" "\\1" Z3_VERSION_MINOR "${Z3_VERSION_STRING}")
string(REGEX REPLACE "[0-9]+[.][0-9]+[.]([0-9]+).*" "\\1" Z3_VERSION_PATCH "${Z3_VERSION_STRING}")
math(EXPR Z3_VERSION_INTEGER "${Z3_VERSION_MAJOR} * 10000 + ${Z3_VERSION_MINOR} * 100 + ${Z3_VERSION_PATCH}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DZ3_VERSION_INTEGER=${Z3_VERSION_INTEGER}")


###   Import Dependency: Parabix   ###
if (NOT PARABIX_PATH)
   message(FATAL_ERROR "You must pass Parabix root directory as an argument")
endif()
find_package(Parabix REQUIRED)
message(STATUS "Found Parabix in: ${PARABIX_INCLUDE_DIR}")
include_directories(${PARABIX_INCLUDE_DIR})


###   Import Dependency: SQLite    ###
if (NOT SQLITE_BUILD_PATH)
   message(FATAL_ERROR "You must pass SQLite build path as an argument")
endif()
include_directories(${SQLITE_BUILD_PATH})


### Import Local Headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

###   Project Configuration   ###

# Require that compilers support all features of C11 and C++11.
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

# Define `CXX_STANDARD` to enable conditional compilation based on standard version.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCXX_STANDARD=${CMAKE_CXX_STANDARD}")

# Add include directories.
include_directories(${PARABIX_PATH}/include)

# Place compiled executables in build/bin.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Use @rpath for dylibs on macOS
set(CMAKE_MACOSX_RPATH ON)

# Enable all warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

if(ARTIFACT_TYPE STREQUAL "Static")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSQLITE_CORE=ON")
endif()

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fvisibility-inlines-hidden" SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
if(SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

# no pie disables the generation of position-independent executables, which is a default security feature of newer compiles
# that prevents addr2line from being able to resolve which line corresponds to the code at run time. This in turn prevents
# CreateAssert from being able to provide the compilation call stack for each JIT'ed assertion error.

UNSET(HAS_EXECINFO CACHE)
UNSET(HAS_MACH_VM_TYPES CACHE)

# Disable RunTime Type Information
if(MSVC) # using Visual Studio C++
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
else() # using Clang, GCC, Intel C++, etc
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

set(CMAKE_REQUIRED_FLAGS)

# Use gold linker on Linux:gcc
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    endif()
endif()

###   Functions   ###

# Sets the output name for a given target.
# Naming scheme is "parabix_<target>" with any '.' replaced with a '_'
function(parabix_set_library_name NAME)
    string(REPLACE "." "_" DELIMITED_NAME ${NAME})
    string(CONCAT LIBRARY_NAME "" ${DELIMITED_NAME})
    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME ${LIBRARY_NAME})
endfunction(parabix_set_library_name)

include(CMakeParseArguments)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CheckIncludeFileCXX)
    CHECK_INCLUDE_FILE_CXX(execinfo.h HAS_EXECINFO)
    if (HAS_EXECINFO)
        ###   Import Dependency: DWARF   ###
        #find_package(LibDwarf) # libelf-dev libdwarf-dev
        #if (LIBDWARF_FOUND)
        #    include_directories(${LIBDWARF_INCLUDE_DIRS})
        #    link_directories(${LIBDWARF_LIBRARIES})
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_ASSERTION_TRACE")
        #endif()
    endif()
endif()

# Release & Debug flags

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -O1 -ggdb -fvisibility=default -fno-omit-frame-pointer -fno-optimize-sibling-calls")

function(parabix_add_module)
    cmake_parse_arguments(
        ARG             # resultant argument prefix
        ""              # boolean args
	"NAME"          # mono-valued arguments:
                        #   NAME: target name
        "SRC;DEPS"      # multi-valued arguments:
                        #   SRC:  source file list
                        #   DEPS: target dependencies
        ${ARGN}         # arguments to parse
    )
    if(NOT ARG_NAME)
        message(FATAL_ERROR "parabix_add_module: A target name is required")
    endif()

    if(ARTIFACT_TYPE STREQUAL "Static")
        add_library(${ARG_NAME} STATIC ${ARG_SRC})
    else()
        add_library(${ARG_NAME} SHARED ${ARG_SRC})
    endif()
        
    parabix_set_library_name(${ARG_NAME})
    target_link_libraries(${ARG_NAME} ${ARG_DEPS})
    target_link_libraries(${ARG_NAME} ${Z3_LIBRARIES})
    target_link_libraries(${ARG_NAME} ${PARABIX_LIBRARIES})
endfunction(parabix_add_module)

function(parabix_add_executable)
    cmake_parse_arguments(
        ARG         # resultant argument prefix
        ""          # boolean args
        "NAME"      # mono-valued arguments:
                    #   NAME: target name
        "SRC;DEPS"  # multi-valued arguments:
                    #   SRC:  source file list
                    #   DEPS: target dependencies
        ${ARGN}     # arguments to parse
    )
    if(NOT ARG_NAME)
        message(FATAL_ERROR "parabix_add_executable: A target name is required")
    endif()
    add_executable(${ARG_NAME} ${ARG_SRC})
    target_link_libraries(${ARG_NAME} ${Boost_LIBRARIES})
    target_link_libraries(${ARG_NAME} ${Z3_LIBRARIES})
    target_link_libraries(${ARG_NAME} ${PARABIX_LIBRARIES})
endfunction(parabix_add_executable)

### LIBS
parabix_add_module(
NAME
    icgrepext
SRC
    icgrep_common.cpp
    icgrep_ext.cpp 
DEPS
    # none for now
)

### EXECUTABLE

if(ADD_EXEC)
    parabix_add_executable(
    NAME
        icgrepexec
    SRC
        icgrep_common.cpp
	icgrep_exec.cpp
    DEPS
        # none for now
)
endif()
