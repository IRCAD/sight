fwLoadProperties()

find_package(VTK QUIET REQUIRED COMPONENTS
    vtkRenderingVolume
    vtkRenderingVolumeOpenGL2
)

if(VCPKG_TARGET_TRIPLET)
    # VTK9
    target_link_libraries(${FWPROJECT_NAME} PRIVATE 
        VTK::RenderingVolume
        VTK::RenderingVolumeOpenGL2
    )

else()
    # VTK7
    fwForwardInclude(
        ${VTK_INCLUDE_DIRS}
    )
    fwForwardLink(
        vtkRenderingVolume2
        vtkRenderingVolumeOpenGL2
    )
endif()

message(DEPRECATION "The VTK renderer is deprecated and will be removed in sight 22.0.
                     Please switch to the Ogre renderer.")