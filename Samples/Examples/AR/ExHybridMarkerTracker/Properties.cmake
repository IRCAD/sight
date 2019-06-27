set(NAME ExHybridMarkerTracker)
set(VERSION 0.1)
set(TYPE APP)
set(DEPENDENCIES)
set(REQUIREMENTS
        dataReg
        arDataReg
        ctrlCamp
        ctrlCom
        gui
        guiQt
        ioCalibration
        servicesReg
        fwlauncher
        appXml
        uiPreferences
        uiTools
        visuVTK
        visuVTKQt
        visuVTKAdaptor
        visuVTKARAdaptor
        videoQt
        videoOpenCV
        videoTools
        hybridMarkerTracker
        media
        arMedia
        preferences
        )

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExHybridMarkerTrackerConfig)