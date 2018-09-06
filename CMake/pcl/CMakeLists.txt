# This is a hacky CMakeLists.txt which aim is to discover PCL and cache the result
# Unfortunately, we can not do that in our main CMakeLists.txt because that will set
# VTK compile definitions for the whole project and will lead to compile errors at the end
#
# If found, it will fill the following variables:
# - PCL_FOUND
# - PCL_INCLUDE_DIRS
# - PCL_LIBRARIES
find_package(PCL QUIET)

set(PCL_FOUND ${PCL_FOUND} PARENT_SCOPE)
set(PCL_INCLUDE_DIRS ${PCL_INCLUDE_DIRS} PARENT_SCOPE)
set(PCL_LIBRARIES ${PCL_LIBRARIES} PARENT_SCOPE)
set(PCL_COMMON_INCLUDE_DIR ${PCL_COMMON_INCLUDE_DIR} PARENT_SCOPE)
