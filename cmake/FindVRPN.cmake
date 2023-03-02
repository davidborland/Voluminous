# - try to find VRPN library
#
# Cache Variables:
#  VRPN_LIBRARY
#  VRPN_SERVER_LIBRARY
#  VRPN_PHANTOM_LIBRARY
#  VRPN_INCLUDE_DIR
#
# Non-cache variables you might use in your CMakeLists.txt:
#  VRPN_FOUND
#  VRPN_SERVER_LIBRARIES
#  VRPN_LIBRARIES
#  VRPN_PHANTOM_LIBRARY
#  VRPN_INCLUDE_DIRS
#
# VRPN_ROOT_DIR is searched preferentially for these files
#
# Requires these CMake modules:
#  FindPackageHandleStandardArgs (known included with CMake >=2.6.2)
#
# Original Author:
# 2009-2010 Ryan Pavlik <rpavlik@iastate.edu> <abiryan@ryand.net>
# http://academic.cleardefinition.com
# Iowa State University HCI Graduate Program/VRAC
#
# Copyright Iowa State University 2009-2010.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(VRPN_ROOT_DIR
    "${VRPN_ROOT_DIR}"
    CACHE
    PATH
    "Root directory to search for VRPN")

if("${CMAKE_SIZEOF_VOID_P}" MATCHES "8")
    set(_libsuffixes lib64 lib)

    # 64-bit dir: only set on win64
    file(TO_CMAKE_PATH "$ENV{ProgramW6432}" _progfiles)
else()
    set(_libsuffixes lib)
    if(NOT "$ENV{ProgramFiles(x86)}" STREQUAL "")
        # 32-bit dir: only set on win64
        file(TO_CMAKE_PATH "$ENV{ProgramFiles(x86)}" _progfiles)
    else()
        # 32-bit dir on win32, useless to us on win64
        file(TO_CMAKE_PATH "$ENV{ProgramFiles}" _progfiles)
    endif()
endif()

include(SelectLibraryConfigurations)

###
# Configure VRPN
###

# If the VRPN_ROOT_DIR contains a CMakeCache.txt file, assume it was
# configured and built with CMake and find the source directory
file(GLOB VRPN_CMAKECACHE_FILE "${VRPN_ROOT_DIR}/CMakeCache.txt")
if (VRPN_CMAKECACHE_FILE)
  set(VRPN_SOURCE_DIR_REGEX "VRPN_SOURCE_DIR:STATIC=")
  string(LENGTH "${VRPN_SOURCE_DIR_REGEX}" VRPN_SOURCE_DIR_REGEX_LENGTH)
  file(STRINGS "${VRPN_CMAKECACHE_FILE}" VRPN_SOURCE_DIR_FULL_LINE REGEX "${VRPN_SOURCE_DIR_REGEX}")
  string(SUBSTRING "${VRPN_SOURCE_DIR_FULL_LINE}" ${VRPN_SOURCE_DIR_REGEX_LENGTH} -1 VRPN_SOURCE_DIR)
  set(QUATLIB_ROOT_DIR "${VRPN_ROOT_DIR}/quat")

  load_cache( "${VRPN_ROOT_DIR}" READ_WITH_PREFIX ""
    HDAPI_LIBRARY_DEBUG
    HDAPI_LIBRARY_RELEASE
    HDAPI_HDU_LIBRARY_DEBUG
    HDAPI_HDU_LIBRARY_RELEASE
    HLAPI_LIBRARY_DEBUG
    HLAPI_LIBRARY_RELEASE
    HLAPI_HLU_LIBRARY_DEBUG
    HLAPI_HLU_LIBRARY_RELEASE
    )

  select_library_configurations(HDAPI)  
  select_library_configurations(HDAPI_HDU)
  select_library_configurations(HLAPI)
  select_library_configurations(HLAPI_HLU)
endif()

find_path(VRPN_INCLUDE_DIR
    NAMES
    vrpn_Connection.h
    PATH_SUFFIXES
    include
    include/vrpn
    HINTS
    "${VRPN_ROOT_DIR}"
    PATHS
    "${_progfiles}/VRPN"
    /usr/local
    "${VRPN_SOURCE_DIR}"
    )

find_library(VRPN_LIBRARY_DEBUG
    NAMES
    vrpn
    PATH_SUFFIXES
    ${_libsuffixes}
    Debug
    HINTS
    "${VRPN_ROOT_DIR}"
    PATHS
    "${_progfiles}/VRPN"
    /usr/local
    "${VRPN_ROOT_DIR}"
    )

find_library(VRPN_LIBRARY_RELEASE
    NAMES
    vrpn
    PATH_SUFFIXES
    ${_libsuffixes}
    Release
    HINTS
    "${VRPN_ROOT_DIR}"
    PATHS
    "${_progfiles}/VRPN"
    /usr/local
    "${VRPN_ROOT_DIR}"
    )

select_library_configurations(VRPN)

find_library(VRPN_SERVER_LIBRARY_DEBUG
    NAMES
    vrpnserver
    PATH_SUFFIXES
    ${_libsuffixes}
    Debug
    HINTS
    "${VRPN_ROOT_DIR}"
    PATHS
    "${_progfiles}/VRPN"
    /usr/local
    "${VRPN_ROOT_DIR}"
    )

find_library(VRPN_SERVER_LIBRARY_RELEASE
    NAMES
    vrpnserver
    PATH_SUFFIXES
    ${_libsuffixes}
    Release
    HINTS
    "${VRPN_ROOT_DIR}"
    PATHS
    "${_progfiles}/VRPN"
    /usr/local
    "${VRPN_ROOT_DIR}"
    )

select_library_configurations(VRPN_SERVER)

find_library(VRPN_PHANTOM_LIBRARY_DEBUG
    NAMES
    vrpnserver
    PATH_SUFFIXES
    ${_libsuffixes}
    server_src/Debug
    HINTS
    "${VRPN_ROOT_DIR}"
    PATHS
    "${_progfiles}/VRPN"
    /usr/local
    "${VRPN_ROOT_DIR}"
    )

find_library(VRPN_PHANTOM_LIBRARY_RELEASE
    NAMES
    vrpnserver
    PATH_SUFFIXES
    ${_libsuffixes}
    server_src/Debug
    HINTS
    "${VRPN_ROOT_DIR}"
    PATHS
    "${_progfiles}/VRPN"
    /usr+/local
    "${VRPN_ROOT_DIR}"
    )

select_library_configurations(VRPN_PHANTOM)

###
# Dependencies
###
set(_deps_libs)
set(_deps_includes)
set(_deps_check)

find_package(quatlib)
list(APPEND _deps_libs ${QUATLIB_LIBRARIES})
list(APPEND _deps_includes ${QUATLIB_INCLUDE_DIRS})
list(APPEND _deps_check QUATLIB_FOUND)

if(NOT WIN32)
    find_package(Threads)
    list(APPEND _deps_libs ${CMAKE_THREAD_LIBS_INIT})
    list(APPEND _deps_check CMAKE_HAVE_THREADS_LIBRARY)
endif()


# handle the QUIETLY and REQUIRED arguments and set xxx_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VRPN
    DEFAULT_MSG
    VRPN_LIBRARY
    VRPN_INCLUDE_DIR
    ${_deps_check})

if(VRPN_FOUND)
    set(VRPN_INCLUDE_DIRS "${VRPN_INCLUDE_DIR}" ${_deps_includes})
    set(VRPN_LIBRARIES "${VRPN_LIBRARY}" ${_deps_libs})
    set(VRPN_SERVER_LIBRARIES "${VRPN_SERVER_LIBRARY}" ${_deps_libs})

    mark_as_advanced(VRPN_ROOT_DIR)

    if (VRPN_CMAKECACHE_FILE)
      # We need to add a compiler definition to tell VRPN that we want to use
      # the vrpn_Configure.h file in the CMake build directory instead of the
      # source directory
      add_definitions("-DVRPN_USING_CMAKE=\"${VRPN_ROOT_DIR}/vrpn_Configure.h\"")
    endif()
endif()

mark_as_advanced(VRPN_LIBRARY
    VRPN_SERVER_LIBRARY
    VRPN_INCLUDE_DIR)
