sight_add_target( ui_history TYPE LIBRARY WARNINGS_AS_ERRORS OFF)



target_link_libraries(ui_history PUBLIC 
                      core
                      data
                      filter_image
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
