import QtQuick 2.0
import styleQml 1.0
import fwQml 1.0

Item {
    id: activity

    // AppManager associated to the application, it will be register the services
    property IQmlAppManager appManager

    // Map used to replace the object, channel or other string associated to the AppManager's inputs
    property var replaceMap

    // List of the services that will be register in the AppManager (use registerService(srv) to register a service)
    property var serviceList: []

    // register the service to the AppManager when the Activity component is completed
    function registerService(service){
        serviceList.push(service)
    }

    Component.onCompleted: {
        // Initialize the AppManager with the replaceMap and register the services
        appManager.replaceInputs(replaceMap)
        appManager.initialize()
        for (var service in serviceList){
            appManager.onServiceCreated(serviceList[service])
        }

    }
}
