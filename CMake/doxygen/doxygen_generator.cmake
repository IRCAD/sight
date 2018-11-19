# Generates doxygen for all specified projects.
# Works only with sight CMake script.
function(doxygenGenerator)
    include(${CMAKE_ROOT}/Modules/Documentation.cmake)
    find_package(Doxygen QUIET)

    if(DOXYGEN_FOUND)
        set(INCLUDE_DIRS)
        foreach(PROJECT ${PROJECT_LIST})
            if(TARGET ${PROJECT})
                get_target_property(PROJECT_INCLUDE_DIRECTORIES ${PROJECT} INCLUDE_DIRECTORIES)
                if(PROJECT_INCLUDE_DIRECTORIES)
                    list(REMOVE_DUPLICATES PROJECT_INCLUDE_DIRECTORIES)
                    foreach(DEPENDENCY ${PROJECT_INCLUDE_DIRECTORIES})
                        list(APPEND INCLUDE_DIRS ${DEPENDENCY})
                    endforeach()
                endif()
            elseif(EXISTS "${${PROJECT}_DIR}/include")
                list(APPEND INCLUDE_DIRS "${${PROJECT}_DIR}/include/")
            endif()
        endforeach()
        list(REMOVE_DUPLICATES INCLUDE_DIRS)
        string(REPLACE ";" " " INCLUDE_DIRS "${INCLUDE_DIRS}")
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMake/doxygen/Doxyfile.in
                       ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Doxygen/Doxyfile @ONLY)
        add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Doxygen/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT
            "Generating API documentation with Doxygen" VERBATIM)
    endif()
endfunction()

# Create a target generating a Dash/Zeal compatible docset
function(docsetGenerator)
    # Generate a custom doxygen to be used for generating the docset
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMake/doxygen/Doxyfile.docset.in
                   ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Docset/Doxyfile @ONLY)
    add_custom_target(docset_dirs ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Docset
                      COMMENT "Creating docset directories" VERBATIM)
    add_custom_target(docset_clean
                      COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Docset/html
                      COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Docset/sight.docset
                      DEPENDS docset_dirs
                      COMMENT "Cleaning up previous docset build" VERBATIM)
    add_custom_target(docset_doxygen ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Docset/Doxyfile
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      DEPENDS docset_clean ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Docset/Doxyfile
                      COMMENT "Generating API documentation with Doxygen (docset version)" VERBATIM)

    # Create the json consumed by the docset generation script
    set(BUNDLES_JSON)
    set(SRCLIBS_JSON)
    foreach(PROJECT ${PROJECT_LIST})
        if(${PROJECT}_TYPE STREQUAL "LIBRARY")
            set(SRCLIBS_JSON "${SRCLIBS_JSON}, \"${PROJECT}\"")
        elseif(${PROJECT}_TYPE STREQUAL "BUNDLE")
            set(BUNDLES_JSON "${BUNDLES_JSON}, \"${PROJECT}\"")
        endif()
    endforeach()
    string(REGEX REPLACE "^[ ]*,[ ]*" "" BUNDLES_JSON "${BUNDLES_JSON}")
    string(REGEX REPLACE "^[ ]*,[ ]*" "" SRCLIBS_JSON "${SRCLIBS_JSON}")
    set(REPOSITORIES_JSON \"${CMAKE_SOURCE_DIR}\")
    foreach(REPO ${ADDITIONAL_REPOSITORIES})
        set(REPOSITORIES_JSON "${REPOSITORIES_JSON}, \"${REPO}\"")
    endforeach()

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMake/doxygen/projects.json.in
                   ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Docset/projects.json @ONLY)

    # Run the generation script
    find_package(PythonInterp 3 QUIET)
    if(NOT PYTHONINTERP_FOUND)
        message(WARNING "A Python3 interpreter is required to build the Dash docset, but none was found.")
        return()
    else()
        add_custom_target(docset ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/CMake/doxygen/build_docset.py
                          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Docset
                          DEPENDS docset_doxygen ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Docset/projects.json
                          COMMENT "Generating dash docset"
                          VERBATIM)
        add_custom_command(TARGET docset POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/CMake/doxygen/Info.plist
                                                            ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Docset/sight.docset/Contents
                           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/CMake/doxygen/f4s_logo_16x16.png
                                                            ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Docset/sight.docset/icon.png
                           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/CMake/doxygen/f4s_logo_32x32.png
                                                            ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Docset/sight.docset/icon@2x.png
                           COMMENT "Copying dash docset configuration files"
                           VERBATIM)
    endif()
endfunction()
