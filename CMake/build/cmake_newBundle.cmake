#read arguments from file
find_path(ArgsFile "argsBundle.cmake"  "${CMAKE_BINARY_DIR}/")
if(${ArgsFile} STREQUAL ArgsFile-NOTFOUND)
    message( FATAL_ERROR "${CMAKE_BINARY_DIR}/argsBundle.cmake not found, maybe you forgot to copy the arguments file into your build directory?")
else()
    include("${CMAKE_BINARY_DIR}/argsBundle.cmake")
endif()

set(PROJECT_VERSION @PROJECT_VERSION@)

#check required parameters
if(NOT DEFINED BUNDLE_NAME)
    message( FATAL_ERROR "BUNDLE_NAME parameter is not defined, please specify.")
elseif(NOT DEFINED  BUNDLE_PATH)
    message( FATAL_ERROR "BUNDLE_PATH parameter is not defined, please specify.")
endif()

string(TOUPPER ${BUNDLE_NAME} BUNDLE_NAME_UPPER)

# find path to bundle
find_path(PathFlag ${BUNDLE_PATH})

if(${PathFlag} STREQUAL Path-NOTFOUND)
    message( FATAL_ERROR "Bundle path '${BUNDLE_PATH}' not found")
endif()

#copy files of new service with configured parameters
configure_file(
    "${SOURCEDIR}/CMake/build/Bundle/include/namespace.hpp.in"
    "${BUNDLE_PATH}/${BUNDLE_NAME}/include/${BUNDLE_NAME}/namespace.hpp"
    @ONLY
    NEWLINE_STYLE UNIX)

configure_file(
    "${SOURCEDIR}/CMake/build/Bundle/include/Plugin.hpp.in"
    "${BUNDLE_PATH}/${BUNDLE_NAME}/include/${BUNDLE_NAME}/Plugin.hpp"
    @ONLY
    NEWLINE_STYLE UNIX)

configure_file(
    "${SOURCEDIR}/CMake/build/Bundle/src/Plugin.cpp.in"
    "${BUNDLE_PATH}/${BUNDLE_NAME}/src/${BUNDLE_NAME}/Plugin.cpp"
    @ONLY
    NEWLINE_STYLE UNIX)

configure_file(
    "${SOURCEDIR}/CMake/build/Bundle/rc/plugin.xml.in"
    "${BUNDLE_PATH}/${BUNDLE_NAME}/rc/plugin.xml"
    @ONLY
    NEWLINE_STYLE UNIX)

configure_file(
    "${SOURCEDIR}/CMake/build/Bundle/Properties.cmake.in"
    "${BUNDLE_PATH}/${BUNDLE_NAME}/Properties.cmake"
    @ONLY
    NEWLINE_STYLE UNIX)

configure_file(
    "${SOURCEDIR}/CMake/build/Bundle/CMakeLists.txt.in"
    "${BUNDLE_PATH}/${BUNDLE_NAME}/CMakeLists.txt"
    @ONLY
    NEWLINE_STYLE UNIX)

message("New bundle ${BUNDLE_NAME} created, please do not forget to reconfigure with cmake" )
