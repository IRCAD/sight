# Generates doxygen for all specified projects.
# Works only with fw4spl CMake script.
function(doxygenGenerator)
    include(${CMAKE_ROOT}/Modules/Documentation.cmake)
    find_package(Doxygen)
    
    if(DOXYGEN_FOUND)
        set(INCLUDE_DIRS)
        foreach(PROJECT ${PROJECT_LIST})
            list(APPEND INCLUDE_DIRS "${${PROJECT}_DIR}/include")
        endforeach(PROJECT)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMake/doxygen/Doxyfile.in 
                       ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Doxygen/Doxyfile @ONLY)
        add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Documentation/Doxygen/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT
            "Generating API documentation with Doxygen" VERBATIM)
    endif()
endfunction()
