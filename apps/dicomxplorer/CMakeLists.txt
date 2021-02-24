sight_add_target( dicomxplorer TYPE APP )


add_dependencies(${FWPROJECT_NAME} 
                 module_service
                 data
                 module_ui_base
                 module_ui_qt
                 module_activity
                 module_appXml
                 module_memory
                 fwlauncher
                 module_io_atoms
                 module_io_itk
                 module_io_vtk
                 module_io_dicom
                 module_io_matrix
                 module_io_activity
                 activity_io_dicom
                 activity_io_dicomweb
                 module_ui_icons
                 atoms
                 module_ui_base
)

moduleParam(module_appXml PARAM_LIST config parameters PARAM_VALUES VRRenderBase VRRenderAppBase)

sight_add_shortcut(${FWPROJECT_NAME})
