import QtQuick 2.0
import styleQml 1.0
import fwQml 1.0

Item {
    id: activity

    property IQmlAppManager appManager
    property var replaceMap
    property var serviceList: []

    function registerService(service){
        console.log("push " + service)
        serviceList.push(service)
    }

    Component.onCompleted: {
        appManager.replaceInputs(replaceMap)
        appManager.initialize()
        for (var service in serviceList){
            console.log("register service: " + serviceList[service]);
            appManager.onServiceCreated(serviceList[service])
        }

    }
}
