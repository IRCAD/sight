add_subdirectory(video)
add_subdirectory(dimse)
add_subdirectory(itk)
add_subdirectory(vision)
add_subdirectory(vtk)
add_subdirectory(igtl)
add_subdirectory(dicomweb)
add_subdirectory(document)
add_subdirectory(matrix)
add_subdirectory(dicom)
add_subdirectory(session)
add_subdirectory(bitmap)
add_subdirectory(zip)
if(SIGHT_ENABLE_REALSENSE)
    add_subdirectory(realsense)
endif()

if(${VTK_MAJOR_VERSION} VERSION_LESS "8")
    add_subdirectory(pcl)
endif()
