sight_add_target( ui_historyTest TYPE TEST )






target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      ui_history
                      utestData
)
