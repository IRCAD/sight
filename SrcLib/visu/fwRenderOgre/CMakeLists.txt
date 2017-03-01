# include first to be taken into account in the PCH compile flags
include(FindOpenMP)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.6)
        message("Try forcing OpenMP support for Clang")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -fopenmp=libiomp5")
    endif()
endif()

fwLoadProperties()

find_package (Boost COMPONENTS regex REQUIRED)

if(WIN32)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${EXTERNAL_LIBRARIES}/cmake)
elseif(APPLE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${EXTERNAL_LIBRARIES}/CMake)
    set(CMAKE_FRAMEWORK_PATH ${EXTERNAL_LIBRARIES}/lib/${CMAKE_BUILD_TYPE})
else()
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${EXTERNAL_LIBRARIES}/lib/OGRE/cmake)
endif()

find_package(OGRE REQUIRED)

# This is a hack to copy plugins inside the build directory
# Most developers executes apps inside the build directory so this is done as a convenience
if(NOT APPLE)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(OGRE_PLUGIN_DIR "${OGRE_PLUGIN_DIR_DBG}")
    else()
        set(OGRE_PLUGIN_DIR "${OGRE_PLUGIN_DIR_REL}")
    endif()

    file(GLOB OGRE_PLUGINS "${OGRE_PLUGIN_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
    file(INSTALL ${OGRE_PLUGINS} DESTINATION "${PROJECT_BINARY_DIR}/ogreplugins/")

    # This copies the plugins into the install directory
    install(DIRECTORY "${PROJECT_BINARY_DIR}/ogreplugins/" DESTINATION "ogrePlugins/")
else()

    # Seen on macOS 10.12.3, the OGRE_PLUGIN_DIR_DBG is empty in Debug, but the
    #OGRE_PLUGIN_DIR_REL is correctly set. Not sure if this a bug of our side or ogre mac
    # As a workaround, we check the content if it is empty, we take the other one
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        if("${OGRE_PLUGIN_DIR_DBG}" STREQUAL "")
            set(OGRE_PLUGIN_DIR "${OGRE_PLUGIN_DIR_REL}")
        else()
            set(OGRE_PLUGIN_DIR "${OGRE_PLUGIN_DIR_DBG}")
        endif()
    else()
        if("${OGRE_PLUGIN_DIR_REL}" STREQUAL "")
            set(OGRE_PLUGIN_DIR "${OGRE_PLUGIN_DIR_DBG}")
        else()
            set(OGRE_PLUGIN_DIR "${OGRE_PLUGIN_DIR_REL}")
        endif()
    endif()

    # Ogre search frameworks plugings in /Contents/Frameworks/
    set(FRAMEWORKS_DIR "${PROJECT_BINARY_DIR}/bin/Contents/Frameworks")
    file(GLOB OGRE_PLUGINS "${OGRE_PLUGIN_DIR}/*")
    file(INSTALL ${OGRE_PLUGINS} DESTINATION ${FRAMEWORKS_DIR})

    # This copies the plugins into the install directory
    install(DIRECTORY ${FRAMEWORKS_DIR} DESTINATION "/bin/Contents/Frameworks/")
endif()

fwForwardInclude(
    ${OGRE_INCLUDE_DIRS}
    ${OGRE_Overlay_INCLUDE_DIRS}
)

fwForwardLink(
    ${OGRE_LIBRARIES}
    ${OGRE_Overlay_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
)
