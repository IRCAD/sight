set( NAME Tuto01Basic ) # Name of the application
set( VERSION 0.1 ) # Version of the application
set( TYPE APP ) # Type APP represent "Application"
set( DEPENDENCIES  ) # For an application we have no dependencies (libraries to link)
set( REQUIREMENTS # The bundles used by this application
    dataReg # to load the data registry
    servicesReg # to load the service registry
    gui # to load gui
    guiQt # to load qt implementation of gui
    fwlauncher # executable to run the application
    appXml # to parse the application configuration
)

# Set the configuration to use : 'tutoBasicConfig'
bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoBasicConfig) 
