macro(qt_plugins_setup)

    # check if there is a PLUGINS variable in the current bundle properties.cmake
    if(PLUGINS)
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
            string(FIND "${PLUGINS}" ${QT_NAME} QT_TEST)
            if( NOT ${QT_TEST} EQUAL -1 )

                file(GLOB_RECURSE QT_FILES  "${CURRENT_DIR}/*")

                string(LENGTH ${CURRENT_DIR} CURRENT_LENGTH)
                MATH( EXPR FINAL_LENGTH "${CURRENT_LENGTH} - ${QT_LENGTH}" )
                string(SUBSTRING ${CURRENT_DIR} ${QT_LENGTH} ${FINAL_LENGTH} SUB_DIR)

                list(APPEND PLUGINS_LIST ${SUB_DIR})

            endif()
         endforeach()

         set( QT_REQUIREMENTS "${QT_REQUIREMENTS};${PLUGINS_LIST}" PARENT_SCOPE)

    endif()

endmacro(qt_plugins_setup)
