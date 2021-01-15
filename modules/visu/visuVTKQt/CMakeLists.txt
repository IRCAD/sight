fwLoadProperties()

find_package(VTK QUIET COMPONENTS
    vtkImagingCore
    REQUIRED
)

if(VCPKG_TARGET_TRIPLET)
    # This is required by VTK ???
    find_package(glew QUIET REQUIRED)

    # VTK9
    target_link_libraries(${FWPROJECT_NAME} PRIVATE 
        VTK::GUISupportQt
    )


else()
    include(${VTK_USE_FILE})
    target_link_libraries(${FWPROJECT_NAME} PRIVATE
        vtkGUISupportQt
    )
endif()

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

message(DEPRECATION "The VTK renderer is deprecated and will be removed in sight 22.0.
                     Please switch to the Ogre renderer.")