fwLoadProperties()

set(FWPROJECT_FULLNAME ${${FWPROJECT_NAME}_FULLNAME})

# handle share different path on Unix systems
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/resources.cfg.in"
                "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}/resources.cfg" @ONLY)
