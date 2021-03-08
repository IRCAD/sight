###############################################################################
# Find OpenNI2
#
# This sets the following variables:
# OPENNI2_FOUND - True if OPENNI2 was found.
# OPENNI2_INCLUDE_DIRS - Directories containing the OPENNI include files.
# OPENNI2_LIBRARY - Libraries needed to use OPENNI.
# OPENNI2_DRIVERS_DIR - Directories containing the openNi driver files.

find_package(PkgConfig QUIET)

# Find lib (not useful for windows)
set(OPENNI2_FOUND FALSE CACHE BOOL "" FORCE)
find_library(OPENNI2_LIBRARY
    NAMES OpenNI2
    PATH_SUFFIXES lib/openni2/
)

# Find include (not useful for windows)
find_path(OPENNI2_INCLUDE_DIRS
    NAMES OpenNI.h
    PATH_SUFFIXES include/openni2/
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenNI2 DEFAULT_MSG OPENNI2_LIBRARY OPENNI2_INCLUDE_DIRS)

message(STATUS "(OPENNI2_FOUND : ${OPENNI2_FOUND} include: ${OPENNI2_INCLUDE_DIRS}, lib: ${OPENNI2_LIBRARY})")

mark_as_advanced(OPENNI2_FOUND)

if(OPENNI2_FOUND)
    set(OPENNI2_FOUND TRUE CACHE BOOL "" FORCE)
    if(WIN32)
        # Find the driver folder
        find_path(OPENNI2_DRIVERS_DIR NAMES Drivers PATH_SUFFIXES bin/OpenNI2)
    else()
        find_path(OPENNI2_DRIVERS_DIR NAMES Drivers PATH_SUFFIXES lib/OpenNI2)        
    endif()
    message(STATUS "OpenNI2 found ( include: ${OPENNI2_INCLUDE_DIRS}, lib: ${OPENNI2_LIBRARY})")
endif()
