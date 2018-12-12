# include first to be taken into account in the PCH compile flags
find_package(OpenMP QUIET)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

fwLoadProperties()

find_package(Boost QUIET COMPONENTS regex REQUIRED)
find_package(OGRE QUIET REQUIRED)
find_package(OpenGL QUIET REQUIRED)

# Treat warnings as errors
if ( CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_compile_options(${FWPROJECT_NAME} PRIVATE "-Werror")
endif ()

# This is a hack to copy plugins inside the build directory
# Most developers executes apps inside the build directory so this is done as a convenience
if(NOT APPLE)
    if(WIN32)
        file(GLOB OGRE_PLUGINS "${OGRE_CONFIG_DIR}/RenderSystem*.dll" "${OGRE_CONFIG_DIR}/Plugin_*.dll" "${OGRE_CONFIG_DIR}/Codec_*.dll")
        set(FW_OGRE_PLUGINS_DIR "${PROJECT_BINARY_DIR}/ogreplugins/")

        # This copies the plugins into the build directory
        file(INSTALL ${OGRE_PLUGINS} DESTINATION "${FW_OGRE_PLUGINS_DIR}")

        # This copies the plugins into the install directory
        install(DIRECTORY "${FW_OGRE_PLUGINS_DIR}" DESTINATION "ogrePlugins/")
    else()
        file(GLOB OGRE_PLUGINS "${OGRE_PLUGIN_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
        set(FW_OGRE_PLUGINS_DIR "${PROJECT_BINARY_DIR}/lib/ogreplugins/")

        # This copies the plugins into the build directory
        file(INSTALL ${OGRE_PLUGINS} DESTINATION "${FW_OGRE_PLUGINS_DIR}")

        # This copies the plugins into the install directory
        install(DIRECTORY "${FW_OGRE_PLUGINS_DIR}" DESTINATION "lib/ogreplugins/")
    endif()

else()
    # Ogre search frameworks plugings in /Contents/Plugins/
    set(FRAMEWORKS_DIR "${PROJECT_BINARY_DIR}/bin/Contents/Plugins")
    file(GLOB OGRE_PLUGINS "${OGRE_PLUGIN_DIR}/*")
    file(INSTALL ${OGRE_PLUGINS} DESTINATION ${FRAMEWORKS_DIR})

    # This copies the plugins into the install directory
    install(DIRECTORY ${FRAMEWORKS_DIR} DESTINATION "bin/Contents")

endif()

fwForwardInclude(
    ${OGRE_INCLUDE_DIRS}
    ${OGRE_PREFIX_DIR}/include
    ${OGRE_Overlay_INCLUDE_DIRS}
)

# The new cmake config files are broken, they give only the library names without any path or suffix
# We hack them but definitely this should be fixed upstream
if(WIN32)
    foreach(OGRE_LIB ${OGRE_LIBRARIES})
        if(IS_ABSOLUTE ${OGRE_LIB})
            list(APPEND OGRE_FIXED_LIBRARIES ${OGRE_LIB})
        else()
            if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
                list(APPEND OGRE_FIXED_LIBRARIES "${OGRE_LIBRARY_DIRS}/${OGRE_LIB}_d.lib")
            else()
                list(APPEND OGRE_FIXED_LIBRARIES "${OGRE_LIBRARY_DIRS}/${OGRE_LIB}.lib")
            endif()
        endif()
    endforeach()

    fwForwardLink(
        ${OGRE_FIXED_LIBRARIES}
        ${Boost_REGEX_LIBRARY}
        ${OPENGL_LIBRARIES}
    )
else()
    fwForwardLink(
        -L${OGRE_LIBRARY_DIRS}
        ${OGRE_LIBRARIES}
        ${Boost_REGEX_LIBRARY}
        ${OPENGL_LIBRARIES}
    )

endif()

set(FWPROJECT_FULLNAME ${${FWPROJECT_NAME}_FULLNAME})

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/resources.cfg.in"
                "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}/resources.cfg" @ONLY)

if(USE_CONAN)
    # manage Ogre3D transitive dependencies
    fwForwardLink(
        CONAN_PKG::zzip
    )
endif()
