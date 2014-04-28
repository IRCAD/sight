fwLoadProperties()


if(NOT EXISTS "${EXTERNAL_LIBRARIES}/lib/site-packages/")
    message(SEND_ERROR "numpy is required for TutoPython and it is not found in : ${EXTERNAL_LIBRARIES}/lib/site-packages/ ")
else()
    file(COPY "${EXTERNAL_LIBRARIES}/lib/site-packages/" DESTINATION "${CMAKE_BINARY_DIR}/Python/")
    if(${NAME}_INSTALL)
        install(DIRECTORY "${EXTERNAL_LIBRARIES}/lib/site-packages/" DESTINATION share/${FWPROJECT_NAME}_${DASH_VERSION} COMPONENT Python)
    endif()
endif()
