sight_add_target( module_io_itk TYPE MODULE )




target_link_libraries(module_io_itk PUBLIC 
                      core
                      data
                      ui_base
                      io_itk
                      service
                      io_base
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
