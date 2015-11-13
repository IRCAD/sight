fwLoadProperties()

fwUseForwardInclude(
    fwData
    fwRuntime
    fwServices
    fwJobs
    fwThread
    fwVtkIO
    io
)

find_package(VTK REQUIRED)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

if (NOT ANDROID)
    fwLink(
        vtkIOImport
    )
endif()
