#read arguments from file
find_path(ArgsFile "argsService.cmake"  "${CMAKE_BINARY_DIR}/")
if(${ArgsFile} STREQUAL ArgsFile-NOTFOUND)
    message( FATAL_ERROR "${CMAKE_BINARY_DIR}/argsService.cmake not found, maybe you forgot to copy the arguments file into your build directory?")
else()
    include("${CMAKE_BINARY_DIR}/argsService.cmake")
endif()

#check required parameters
if(NOT DEFINED BUNDLE_NAME)
    message( FATAL_ERROR "BUNDLE_NAME parameter is not defined, please specify.")
elseif(NOT DEFINED SERVICE_NAME)
    message( FATAL_ERROR "SERVICE_NAME parameter is not defined, please specify.")
elseif(NOT DEFINED SUPERCLASS)
    message( FATAL_ERROR "SUPERCLASS parameter is not defined, please specify.")
elseif(NOT DEFINED SUPERCLASS_NAMESPACE)
    message( FATAL_ERROR "SUPERCLASS_NAMESPACE parameter is not defined, please specify.")
elseif(NOT DEFINED OBJECT)
    message( FATAL_ERROR "OBJECT parameter is not defined, please specify.")
elseif(NOT DEFINED OBJECT_NAMESPACE)
    message( FATAL_ERROR "OBJECT_NAMESPACE parameter is not defined, please specify.")
endif()


# read extension
file(READ "${SOURCEDIR}/CMake/build/Service/rc/extensionService.txt" extension)

string(FIND ${SUPERCLASS_NAMESPACE} "::" SUPERCLASS_NAMESPACE_FLAG)
if("${SUPERCLASS_NAMESPACE_FLAG}" GREATER "-1")
    string(REPLACE "::" "/" SUPERCLASS_NAMESPACE_INCLUDE ${SUPERCLASS_NAMESPACE})
else()
    set(SUPERCLASS_NAMESPACE_INCLUDE ${SUPERCLASS_NAMESPACE})
endif()

if(DEFINED SERVICE_SUBDIRECTORY)
    string(CONCAT SERVICE_SUBDIRECTORY_CPP ${SERVICE_SUBDIRECTORY} "::")
endif()

string(TOUPPER ${BUNDLE_NAME} BUNDLE_NAME_UPPER)
string(TOUPPER ${SERVICE_NAME} SERVICE_NAME_UPPER)

# prepare additional directory for bundle search
string(REPLACE " " ";" ADDITIONAL_PROJECTS ${ADDITIONALDIR})

# find path to bundle
find_path(Path ${BUNDLE_NAME}  "${SOURCEDIR}/Bundles/*/")
if(NOT ${Path})
    foreach(ADDITIONAL_PROJECT ${ADDITIONAL_PROJECTS})
        find_path(Path ${BUNDLE_NAME}  "${ADDITIONAL_PROJECT}/Bundles/*/")
    endforeach()
endif()

if(${Path} STREQUAL Path-NOTFOUND)
    message( FATAL_ERROR "Bundle '${BUNDLE_NAME}' not found")
endif()

#copy files of new service with configured parameters
configure_file(
    "${SOURCEDIR}/CMake/build/Service/include/newService.hpp.in"
    "${Path}/${BUNDLE_NAME}/include/${BUNDLE_NAME}/${SERVICE_SUBDIRECTORY}/${SERVICE_NAME}.hpp"
    @ONLY
    NEWLINE_STYLE UNIX)
    
configure_file(
    "${SOURCEDIR}/CMake/build/Service/src/newService.cpp.in"
    "${Path}/${BUNDLE_NAME}/src/${BUNDLE_NAME}/${SERVICE_SUBDIRECTORY}/${SERVICE_NAME}.cpp"
    @ONLY
    NEWLINE_STYLE UNIX)

# manipulation of the plugin.xml
file(READ "${Path}/${BUNDLE_NAME}/rc/plugin.xml" extensionXML)
string(REPLACE "</plugin>" "" extensionXMLmodified ${extensionXML})
    
# write extension for service
file(WRITE "${Path}/${BUNDLE_NAME}/rc/plugin.xml" ${extensionXMLmodified})
file(APPEND "${Path}/${BUNDLE_NAME}/rc/plugin.xml" ${extension})

configure_file(
    "${Path}/${BUNDLE_NAME}/rc/plugin.xml"
    "${Path}/${BUNDLE_NAME}/rc/plugin.xml"
    @ONLY
    NEWLINE_STYLE UNIX)
    
message("New service ${SERVICE_NAME} of bundle ${BUNDLE_NAME} created, please do not forget to reconfigure with cmake" )    
