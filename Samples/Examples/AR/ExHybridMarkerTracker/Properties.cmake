set(NAME ExHybridMarkerTracker)
set(VERSION 0.1)
set(TYPE APP)
set(DEPENDENCIES)
set(REQUIREMENTS
        dataReg
        arDataReg
        gui
        guiQt
        servicesReg
        fwlauncher
        appXml
        uiPreferences
        visuVTK
        visuVTKQt
        visuVTKAdaptor
        visuVTKARAdaptor
        videoQt
        videoTools
        hybridMarkerTracker
        media
        arMedia
        preferences
        )

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExHybridMarkerTrackerConfig)