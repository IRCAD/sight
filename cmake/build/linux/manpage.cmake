set(PANDOCCOMMAND_FOUND 0)

if(SIGHT_BUILD_MANPAGES)
    find_program(
        PANDOCCOMMAND_PATH NAMES pandoc DOC "Path to Pandoc processor. Used to generate man pages from Markdown."
    )

    if(NOT EXISTS ${PANDOCCOMMAND_PATH})
        message(FATAL_ERROR "Pandoc processor not found, man pages will not be generated. Install pandoc or use the "
                            "CMAKE_PROGRAM_PATH variable."
        )
    else()
        set(PANDOCCOMMAND_FOUND 1)
    endif()
endif()

# Add the targets to generate and install manual page with Pandoc
function(add_man_page TARGET)
    set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/man.md")

    if(EXISTS "${SRC}")

        if(NOT ${PANDOCCOMMAND_FOUND})
            return()
        endif()

        string(TOLOWER ${TARGET} TARGET_LOWER)
        set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_LOWER}.1)

        add_custom_command(
            OUTPUT ${OUTPUT_PATH}
            COMMAND ${PANDOCCOMMAND_PATH} -s -t man ${SRC} -o ${OUTPUT_PATH}
            DEPENDS ${SRC}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating man page ${SUBDIR_PATH_DIRECTORY}/${SUBDIR_PATH_NAME_WLE}"
            VERBATIM
        )

        add_custom_target(
            "man_${TARGET}" ALL DEPENDS ${OUTPUT_PATH} COMMENT "Dependency to trigger man page generation"
        )

        install(FILES ${OUTPUT_PATH} DESTINATION "${CMAKE_INSTALL_FULL_MANDIR}/man1" COMPONENT doc)
    endif()
endfunction(add_man_page)
