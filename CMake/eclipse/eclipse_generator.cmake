# Generates Eclipse project (.project + .cproject) for all specified projects.
# Works only with fw4spl CMake script.
function(eclipseGenerator)

    #find compiler system include directories
    if(WIN32)
        set(SYSTEM_INCLUDES "$ENV{INCLUDE}")
    else()
        include(CMakeExtraGeneratorDetermineCompilerMacrosAndIncludeDirs)
        set(SYSTEM_INCLUDES ${CMAKE_EXTRA_GENERATOR_C_SYSTEM_INCLUDE_DIRS} ${CMAKE_EXTRA_GENERATOR_CXX_SYSTEM_INCLUDE_DIRS})
    endif()
    
    foreach(PROJECT ${ARGV})
        if(NOT TARGET ${PROJECT})
            message("Project target ${PROJECT} doesn't exist")
            continue()
        endif()
        message(STATUS "Generating ${PROJECT} Eclipse project")
        #configure eclipse project template
        set(BUILD_PATH ${CMAKE_BINARY_DIR})
        set(PROJECT_NAME ${PROJECT})

        set(TYPE ${${PROJECT}_TYPE})
        if( TYPE STREQUAL "EXECUTABLE" )
            set(PROJECT_NAME_USER "exec-${PROJECT}")
        elseif( TYPE STREQUAL "LIBRARY" )
            set(PROJECT_NAME_USER "lib-${PROJECT}")
        elseif( TYPE STREQUAL "BUNDLE" )
            set(PROJECT_NAME_USER "bundle-${PROJECT}")
        elseif( TYPE STREQUAL "TEST" )
            set(PROJECT_NAME_USER "test-${PROJECT}")
        elseif( TYPE STREQUAL "APP")
            set(PROJECT_NAME_USER "app-${PROJECT}")
        else()
            set(PROJECT_NAME_USER "unknown-${PROJECT}")
        endif()
        set(MAKE ${CMAKE_MAKE_PROGRAM})
        set(DEPS_INCLUDES "")
        
        get_target_property(PROJECT_INCLUDE_DIRECTORIES ${PROJECT} INCLUDE_DIRECTORIES)
        list(REMOVE_DUPLICATES PROJECT_INCLUDE_DIRECTORIES)
        foreach(INCLUDE_DIR ${PROJECT_INCLUDE_DIRECTORIES})
            set(DEPS_INCLUDES "${DEPS_INCLUDES}\n<listOptionValue builtIn=\"false\" value=\"${INCLUDE_DIR}\"/>")
        endforeach()
        
        foreach(DEPENDENCY ${${PROJECT}_DEPENDENCIES})
            get_target_property(DEPENDENCY_INCLUDE_DIRECTORIES ${DEPENDENCY} INCLUDE_DIRECTORIES)
            list(REMOVE_DUPLICATES DEPENDENCY_INCLUDE_DIRECTORIES)
            foreach(INCLUDE_DIR ${DEPENDENCY_INCLUDE_DIRECTORIES})
                set(DEPS_INCLUDES "${DEPS_INCLUDES}\n<listOptionValue builtIn=\"false\" value=\"${INCLUDE_DIR}\"/>")
            endforeach()
        endforeach()
        
        #add system include dirs
        foreach(SYS_INC ${SYSTEM_INCLUDES})
            string( REGEX REPLACE "\\\\" "/" SYS_INC ${SYS_INC} )
            set(DEPS_INCLUDES "${DEPS_INCLUDES}\n<listOptionValue builtIn=\"false\" value=\"${SYS_INC}\"/>")
        endforeach()
        
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMake/eclipse/.cproject.in ${${PROJECT}_DIR}/.cproject @ONLY )
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMake/eclipse/.project.in ${${PROJECT}_DIR}/.project @ONLY )
    endforeach()
endfunction()
