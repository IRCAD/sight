sight_add_target( ui_history TYPE LIBRARY )



target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      filter_image
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
