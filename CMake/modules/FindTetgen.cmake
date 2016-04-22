# Locate Tetgen library
# This module defines
#  TETGEN_FOUND, if false, do not try to link to TETGEN
#  TETGEN_LIBRARIES
#  TETGEN_INCLUDE_DIR, where to find tetgen.h

find_path(TETGEN_INCLUDE_DIR tetgen.h
  HINTS
  $ENV{TETGEN_DIR}
  PATH_SUFFIXES include include/tetgen
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)

find_library(TETGEN_LIBRARY 
  NAMES tetlib
  HINTS
  $ENV{TETGEN_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

set(TETGEN_LIBRARIES "${TETGEN_LIBRARY}" CACHE STRING "Tetgen Libraries")

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set TETGEN_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TETGEN DEFAULT_MSG TETGEN_LIBRARIES TETGEN_INCLUDE_DIR)

add_definitions(-DTETLIBRARY)

MARK_AS_ADVANCED(TETGEN_INCLUDE_DIR TETGEN_LIBRARIES TETGEN_LIBRARY)

