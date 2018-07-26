# fw4spl-ext 17.1.0

## Refactor:

### getObject

*Replace the last 'getObject()' by 'getInOut()'.*

Replace the last two getObject() by getInout().
They were forgotten in service that already use getInout() for these data,
so the deprecated log is useless.

## Bug fixes:

### activitySelector

*Remove warnings by using seriesDB key instead of series.*

Fix series keys to seriesDB used in various configurations because it will be removed in 18.0 version of FW4SPL.

### beginnerTraining

*Fix the training samples.*

- fix the documentation for the plugin.xml generation
- remove the fwServicesRegisterMacro from the services to let cmake to generate the right one
- add getAutoConnections() for tuto03 and tuto04 SStringEditor




# fw4spl-ext 17.0.0

## Refactor:

### MesherActivity

*Refactor CGogn and VTK mesher.*

- This commit removed dependency to bundle opVTKMesh in opPOCMesher
- CGoGNMesher is now a standard ::fwServices::IOperator
- Updated MesherActivity config with new VTK/CGoGN mesher API

Refactor MesherActivity


