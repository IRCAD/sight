# Generates doxygen for all specified projects.
# Works only with fw4spl CMake script.
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
    find_package(PythonInterp 3 QUIET)
    if(NOT PYTHONINTERP_FOUND)
        message(WARNING "A Python3 interpreter is required to build the Dash docset, but none was found.")
        return()
    endif()
    add_custom_target(docset ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/cmake/doxygen/build_docset.py
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      DEPENDS doc
                      COMMENT "Generating dash docset" VERBATIM)
    add_custom_command(TARGET docset POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/cmake/doxygen/Info.plist
                                                        ${CMAKE_CURRENT_BINARY_DIR}/Documentation/fw4spl.docset/Contents
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/cmake/doxygen/f4s_logo_16x16.png
                                                        ${CMAKE_CURRENT_BINARY_DIR}/Documentation/fw4spl.docset/icon.png
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/cmake/doxygen/f4s_logo_32x32.png
                                                        ${CMAKE_CURRENT_BINARY_DIR}/Documentation/fw4spl.docset/icon@2x.png
                       COMMENT "Copying dash docset configuration files"
                       VERBATIM
)
endfunction()
