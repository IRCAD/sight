sight_add_target(ui_qt TYPE LIBRARY)

target_compile_definitions(
    ${SIGHT_TARGET} PRIVATE "SIGHT_APP_VENDOR=\"${SIGHT_APP_VENDOR}\""
                            "SIGHT_APP_VENDOR_URL=\"${SIGHT_APP_VENDOR_URL}\""
)

# find_package(Qt6 ... ) is performed in Dependencies.cmake
target_link_libraries(${SIGHT_TARGET} PUBLIC Qt6::Widgets Qt6::Concurrent Qt6::OpenGLWidgets)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

target_link_libraries(${SIGHT_TARGET} PUBLIC core data ui service)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
