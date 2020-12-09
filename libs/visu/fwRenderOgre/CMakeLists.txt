# include first to be taken into account in the PCH compile flags
find_package(OpenMP QUIET)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

fwLoadProperties()

find_package(OGRE REQUIRED COMPONENTS Overlay)
target_link_libraries(${FWPROJECT_NAME} PUBLIC OgreOverlay)

find_package(OpenGL QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})

set(FWPROJECT_FULLNAME ${${FWPROJECT_NAME}_FULLNAME})

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/resources.cfg.in"
                "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}/resources.cfg" @ONLY)