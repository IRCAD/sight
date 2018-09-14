#This script is called at install time only on linux OS
#It will be configured at configure time, only '@' variables will be replaced.
#function to find libs and Bundles/* directories

function(findFwlibsAndBundles FWLIBS_BUNDLES_DIRECTORIES)
    file(GLOB_RECURSE LIBS @CMAKE_INSTALL_PREFIX@/@FWBUNDLE_LIB_PREFIX@/*@CMAKE_SHARED_LIBRARY_SUFFIX@*)
    if(@FW_BUILD_EXTERNAL@)
        file(GLOB_RECURSE LIBS @Sight_LIBRARY_DIR@/*@CMAKE_SHARED_LIBRARY_SUFFIX@*)
    endif()

    list(REMOVE_DUPLICATES LIBS)
    set(FOLDERS)
    foreach(LIB ${LIBS})
        get_filename_component(LIB_DIR "${LIB}" DIRECTORY)
        list(APPEND FOLDERS ${LIB_DIR})
    endforeach()
    if(FOLDERS)
        list(REMOVE_DUPLICATES FOLDERS)
    endif()
    set(${FWLIBS_BUNDLES_DIRECTORIES} ${FOLDERS} PARENT_SCOPE)
endfunction()

macro(qt_plugins_setup PROJECT_NAME)

    # check if there is a PLUGINS variable in the current bundle properties.cmake
    if(${PROJECT_NAME}_PLUGINS)
        string(LENGTH "${EXTERNAL_LIBRARIES}/" QT_LENGTH)

        if(WIN32)
            set(FW_QT5_PLUGINS_PATH "${EXTERNAL_LIBRARIES}/bin/qt5/plugins/*")
        else()
            set(FW_QT5_PLUGINS_PATH "${EXTERNAL_LIBRARIES}/lib/qt5/plugins/*")
        endif()

        # search in qml and plugins dirs
        file(GLOB QT_DIRS  "${EXTERNAL_LIBRARIES}/qml/*" "${FW_QT5_PLUGINS_PATH}")
        foreach(CURRENT_DIR ${QT_DIRS})

            get_filename_component(QT_NAME ${CURRENT_DIR} NAME)
            string(FIND "${${PROJECT_NAME}_PLUGINS}" ${QT_NAME} QT_TEST)
            if( NOT ${QT_TEST} EQUAL -1 )

                file(GLOB_RECURSE QT_FILES  "${CURRENT_DIR}/*")
                list(APPEND QT_PLUGINS ${QT_FILES})

                string(LENGTH ${CURRENT_DIR} CURRENT_LENGTH)
                MATH( EXPR FINAL_LENGTH "${CURRENT_LENGTH} - ${QT_LENGTH}" )
                string(SUBSTRING ${CURRENT_DIR} ${QT_LENGTH} ${FINAL_LENGTH} SUB_DIR)

                list(APPEND PLUGINS_LIST ${SUB_DIR})

            endif()
         endforeach()

         set( QT_REQUIREMENTS "${QT_REQUIREMENTS};${PLUGINS_LIST}" )
         set( QT_REQUIREMENTS ${QT_REQUIREMENTS} PARENT_SCOPE)

         file(TO_CMAKE_PATH "${QT_PLUGINS}" QT_PLUGINS)
         set( QT_PLUGINS ${QT_PLUGINS} PARENT_SCOPE)

    endif()

endmacro(qt_plugins_setup)

macro(install_qt_plugins)

    #qt plugins setup
    if(QT_REQUIREMENTS AND NOT BUILD_SDK) # set by helper.cmake -> qt_setup() macros
         foreach(QT_REQUIREMENT ${QT_REQUIREMENTS})
             get_filename_component(QT_REQ_DIR ${QT_REQUIREMENT} DIRECTORY)
             file(TO_CMAKE_PATH "${EXTERNAL_LIBRARIES}/${QT_REQUIREMENT}" QT_REQ_SRC_DIR)
             install(DIRECTORY "${QT_REQ_SRC_DIR}" DESTINATION "${QT_REQ_DIR}" )
        endforeach()
    endif()

endmacro(install_qt_plugins)
