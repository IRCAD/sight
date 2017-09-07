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
find_package(OGRE REQUIRED)

# This is a hack to copy plugins inside the build directory
# Most developers executes apps inside the build directory so this is done as a convenience
if(NOT APPLE)
    file(GLOB OGRE_PLUGINS "${OGRE_PLUGIN_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
    file(INSTALL ${OGRE_PLUGINS} DESTINATION "${PROJECT_BINARY_DIR}/ogreplugins/")

    # This copies the plugins into the install directory
    install(DIRECTORY "${PROJECT_BINARY_DIR}/ogreplugins/" DESTINATION "ogrePlugins/")
else()
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
    -L${OGRE_LIBRARY_DIRS}
    ${OGRE_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
)
