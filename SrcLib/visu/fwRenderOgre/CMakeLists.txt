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
    if(WIN32)
        file(GLOB OGRE_PLUGINS "${OGRE_CONFIG_DIR}/RenderSystem*.dll" "${OGRE_CONFIG_DIR}/Plugin_*.dll")
    else()
        file(GLOB OGRE_PLUGINS "${OGRE_PLUGIN_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
    endif()
    file(INSTALL ${OGRE_PLUGINS} DESTINATION "${PROJECT_BINARY_DIR}/ogreplugins/")

    # This copies the plugins into the install directory
    install(DIRECTORY "${PROJECT_BINARY_DIR}/ogreplugins/" DESTINATION "ogrePlugins/")
else()
    # Ogre search frameworks plugings in /Contents/Plugins/
    set(FRAMEWORKS_DIR "${PROJECT_BINARY_DIR}/bin/Contents/Plugins")
    file(GLOB OGRE_PLUGINS "${OGRE_PLUGIN_DIR}/*")
    file(INSTALL ${OGRE_PLUGINS} DESTINATION ${FRAMEWORKS_DIR})

    # This copies the plugins into the install directory
    install(DIRECTORY ${FRAMEWORKS_DIR} DESTINATION "/bin/Contents/Plugins/")
endif()

fwForwardInclude(
    ${OGRE_INCLUDE_DIRS}
    ${OGRE_Overlay_INCLUDE_DIRS}
)

# The new cmake config files are broken, they give only the library names without any path or suffix
# We hack them but definitely this should be fixed upstream
if(WIN32)
    foreach(OGRE_LIB ${OGRE_LIBRARIES})
        if(IS_ABSOLUTE ${OGRE_LIB})
            list(APPEND OGRE_FIXED_LIBRARIES ${OGRE_LIB})
        else()
            list(APPEND OGRE_FIXED_LIBRARIES "${OGRE_LIBRARY_DIRS}/${OGRE_LIB}.lib")
        endif()
    endforeach()

    fwForwardLink(
        ${OGRE_FIXED_LIBRARIES}
        ${Boost_REGEX_LIBRARY}
    )
else()
    fwForwardLink(
        -L${OGRE_LIBRARY_DIRS}
        ${OGRE_LIBRARIES}
        ${Boost_REGEX_LIBRARY}
    )
endif()
