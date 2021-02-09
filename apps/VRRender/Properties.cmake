
set( NAME VRRender )
set( VERSION 0.9 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    module_services
    data

    module_ui_base
    module_ui_qt

    module_activities
    module_appXml
    module_memory
    fwlauncher

    module_io_atoms
    module_io_itk
    module_io_vtk
    ioGdcm
    module_io_matrix


    2DVisualizationActivity
    3DVisualizationActivity
    volumeRenderingActivity
    blendActivity
    ioActivity

    dicomFilteringActivity
    dicomPacsReaderActivity
    dicomPacsWriterActivity

    DicomWebReaderActivity
    DicomWebWriterActivity

    media

    atoms
    arPatchMedicalData


    module_ui_base
)

moduleParam(module_appXml PARAM_LIST config parameters PARAM_VALUES VRRenderBase VRRenderAppBase)
