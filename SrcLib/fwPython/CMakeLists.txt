find_package(Boost)
if(${Boost_VERSION} GREATER 105700)
    message(AUTHOR_WARNING "Boost ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION} is not compatible with this library.")
else()
    find_package(Boost COMPONENTS python REQUIRED)
    fwLoadProperties()
    find_package(PythonLibs 2.7 REQUIRED )
    get_filename_component(PYTHON_DIR_NAME "${PYTHON_INCLUDE_DIR}" NAME)
    get_filename_component(PYTHON_DIR "${PYTHON_INCLUDE_DIR}" PATH)
    get_filename_component(PYTHON_DIR "${PYTHON_DIR}" PATH)

    if(WIN32)
        string(REGEX REPLACE "2.7$" "27" PYTHON_DIR_NAME "${PYTHON_DIR_NAME}")
    endif()

    file(COPY "${PYTHON_INCLUDE_DIR}/" DESTINATION "${CMAKE_BINARY_DIR}/PythonHome/include/")
    file(COPY "${PYTHON_DIR}/lib/${PYTHON_DIR_NAME}/" DESTINATION "${CMAKE_BINARY_DIR}/PythonHome/lib/")

    if(${NAME}_INSTALL)
        install(DIRECTORY "${PYTHON_INCLUDE_DIR}/" DESTINATION PythonHome/include/ COMPONENT Python)
        install(DIRECTORY "${PYTHON_DIR}/lib/${PYTHON_DIR_NAME}/" DESTINATION PythonHome/lib/ COMPONENT Python)
    endif()


    fwForwardInclude(
        ${Boost_INCLUDE_DIRS}
        ${PYTHON_INCLUDE_DIRS}
    )

    fwLink(
        ${Boost_PYTHON_LIBRARY}
    )

    # Test to avoid policy CMP0022
    if(PYTHON_DEBUG_LIBRARY)
        fwForwardLink(${PYTHON_DEBUG_LIBRARY})
    else()
        fwForwardLink(${PYTHON_LIBRARY})
    endif()
endif()
