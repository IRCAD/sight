
# If found, it will fill the following variables:
# - PCL_FOUND
# - PCL_INCLUDE_DIRS
# - PCL_LIBRARIES
find_package(PCL QUIET)

set(PCL_FOUND ${PCL_FOUND} PARENT_SCOPE)
set(PCL_INCLUDE_DIRS ${PCL_INCLUDE_DIRS} PARENT_SCOPE)
set(PCL_LIBRARIES ${PCL_LIBRARIES} PARENT_SCOPE)
set(PCL_COMMON_INCLUDE_DIR ${PCL_COMMON_INCLUDE_DIR} PARENT_SCOPE)

# Macro that allows to skip the find_package(PCL) which is very slow
macro(fwQuickFindPCL)
    if(NOT PCL_FOUND)
        message(SEND_ERROR "PCL not found")
    endif()

    # This is necessary for now to run this, at least on Linux, in order to resolve the version number of the libraries,
    # .i.e from -lvtksys to -lvtksys7.1
    find_package(VTK QUIET)
endmacro()
