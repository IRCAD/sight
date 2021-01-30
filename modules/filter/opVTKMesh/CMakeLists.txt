fwLoadProperties()

find_package(VTK QUIET COMPONENTS vtkInteractionWidgets REQUIRED)

if(VCPKG_TARGET_TRIPLET)
    # VTK9
    target_link_libraries(${FWPROJECT_NAME} PRIVATE 
        VTK::InteractionWidgets
    )

else()
    include(${VTK_USE_FILE})
    target_link_libraries(${FWPROJECT_NAME} PRIVATE
        vtkInteractionWidgets
    )
endif()

