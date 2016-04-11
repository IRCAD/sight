fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwData
    fwDataTools
    fwGui
    fwGuiQt
    fwMath
    fwRender
    fwRuntime
    fwServices
    fwTools
)

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

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(OGRE_PLUGIN_DIR "${OGRE_PLUGIN_DIR_DBG}")
else()
    set(OGRE_PLUGIN_DIR "${OGRE_PLUGIN_DIR_REL}")
endif()

# This is a hack to copy plugins inside the build directory
# Most developers executes apps inside the build directory so this is done as a convenience
if(NOT APPLE)
    file(GLOB OGRE_PLUGINS "${OGRE_PLUGIN_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
    file(INSTALL ${OGRE_PLUGINS} DESTINATION "${PROJECT_BINARY_DIR}/ogreplugins/")

    # This copies the plugins into the install directory
    install(DIRECTORY "${PROJECT_BINARY_DIR}/ogreplugins/" DESTINATION "ogrePlugins/")
else()
    # Ogre search frameworks plugings in /Contents/Frameworks/
    set(FRAMEWORKS_DIR "${PROJECT_BINARY_DIR}/bin/Contents/Frameworks/")
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
