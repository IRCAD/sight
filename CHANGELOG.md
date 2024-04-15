# sight 24.0.2


# sight 24.0.1


# sight 24.0.0

## Enhancement:

### build

*Update vcpkg.*

### core

*Add missing subscript operator in array_iterator class.*

*Do not restart multi_config_controller when the config does not change.*

*Improve series error management.*

*Improve series error management.*

### doc

*Update main README.md.*

### geometry

*Add unit test for module::geometry::concatenate_matrices.*

### io

*Avoid reetrance in update loops using synchronizers.*

*Add a way to set the video grabber loop mode by a generic parameter.*

Also override the new configuring(const config_t&) signature in tracker interface to allow implementations to override it as well.

### ui

*Properly align qt::parameters sliders.*

*Update icons.*

*Add a button to reset preferences settings.*

*Add an option to skip action signal emit at start.*

*Apply new QSS theme on sight_viewer.*

Some enhancements were also brought in the QSS itself.

*Add the collapsible section widget in libs and provide an example and test on it.*

*Minor UI improvements.*

Simplify reset of points/regions in freehand crop by using single slot for reset.

Correct the accordion menu icon placement.

Others minor improvements on new ircad style QSS.

*Enlarge the status bar.*

*Modernize status editor service.*

`module`::ui::qt::status``was modernized to be able to align it with toolbar buttons. In doing so, its configuration was revised to be more concise and meaningful:

```xml
   <service uid="..." type="sight::module::ui::qt::status">
       <layout>horizontal|vertical</layout>
       <labels display="under|besides">
           <name>SCP Server</name>
           <name>TCP Server</name>
       </labels>
       <red>Stopped</red>
       <green>Tracking</green>
       <orange>Started</orange>
   </service>
```

Here are the changes:
- the `count` is removed since it is useless and error-prone.
- the `form` tag is removed since now the shape is always a circle.
- if one wants only a single status, one just has to declare a single ```<name>```. No need for a shortcut.
- if one wants a status without any name, one can just declare an empty ```<name/>```.
- ```<labels display="beside|under">``` is added to allow specifying the location of the text

*Add optional popup to warn the user when changing activity.*

*Minor improvements for line edit and toolbars.*

- update color when QLineEdit is disabled
- toolbar theme update
- add optional spacing for toolbars
- remove useless property in lineLayout

*Add support for vertical sliders in SParameters.*

### viz

*Skip rendering request when an adaptor is not visible.*

*Render mode policy should always be respected.*

*Center the volume rendering around the visible area.*

*Add support for ruler and shape fiducials.*

*Add an adaptor to display a background grid.*

A new adaptor that displays the grid, with a customizable size, elevation and color is now available:

```xml
    <service uid="..." type="sight::module::viz::scene3d::adaptor::SGrid">
        <config transform="..." size="30" dashLength="2.5" color="#0000FF" elevation="200"/>
    </service>
```

## New features:

### core

*Set parameters with config slot.*

*Add a slot to reset requirements in the sequencer.*

### geometry

*Add a service to damp matrices over time.*

### io

*Get and store itk direction in sight::data::image.*

### ui

*New icons.*

*Add multiple shortcut support on toolbar.*

- disable auto repeat on all QActions of toolbar.

*Add sound support for notifier service.*

- Add three sound files .wav to the qt resources directory.
- Notifier supports three sound notifications, on for each type of notification (Success, failure and info).
- New bool is passed on notify signal to add or mute sound.
- Update ex_notifiers to display a toggle checkbox for adding sound to the displayed notification.

*Add fullscreen support with (f11, ctrl-f11).*

*Add possibility to remove fiducial distances from the current slice.*

*Uncancellable pulse_progress dialog.*

*Allow requesting values set through preferences_configuration.*

### viz

*Add slot to update color of text.*

## Bug fixes:

### build

*Fully qualify export macros to avoid conflicts.*

*Install qml plugins next to binaries on Windows.*

*Remove manual registration of QML plugins on Windows.*

The manual registration of QML plugins on Windows has no longer been required for a while since we use the standard location. It confuses Qt if the path is different when resolving filesystem links (for instance when using SUBST).

*Exclude cuda files from code coverage.*

*New vcpkg build with MSVC 2022 support.*

*Run generate_headers script only during install.*

### ci

*Use Start-SightProcess function to launch deploy tests.*

*Restore test timeout on windows.*

*Point again to the dev branch for sight-data repository.*

### core

*Add recursive mutex protection for GDCM access.*

*Missing dependency to module_ui_icons in module_ui_qt.*

*Make all modules repositories path absolute.*

*Test should use canonified paths.*

*Resolve path symlinks in core::runtime.*

All modules and libraries' paths are now correctly located when encountering symlinks.

*Single instance mode.*

*Always reset output data_ptr when stopping services.*

### doc

*Snake_case convention in doxygen documentation, missing parameters and several typos.*

### io

*Add a new function to avoid deadlock in remove_landmarks().*

Add a new function is_landmark_visible_without_lock().
Fix the remove_landmarks() function.

*Add missing data for saving fiducial ruler and included unit test.*

*Simplify bitmap service configuration.*

*Modernize tuto05.*

- add missing states
- remove the openMeshAct action and fix the way to compute mesh

*Fortify matrix_writer.*

*Do not use cuda driver api.*

*Crash when canceling selector.*

fix(io): selector canceled

### ui

*Module::ui::qt::image leaks.*

*Ui::qt::parameters vertical sliders labels are inverted.*

*Parsing of the option to hide unstarted actions.*

*Accordion_button leaks.*

*Remove useless spacing in ruler and shape fiducials.*

*Restore display and saving of calibration images in Sight Calibrator.*

*Update restrict_to_current_slice function.*

- Add missing slice reference in ruler data

*Do not keep progress dialog shared pointers.*

*Remove unwanted std::cout from status.*

*Assert the result of QMetaObject::invokeMethod.*

*Synchronize with global preferences before update.*

This fixes the internal stored preferences not being in sync with the actual preferences and fixes a widget/memory leak (QDialog without a parent). Unit tests were added for the preferences configuration service.

*Size of the slice selector index text.*

The size of the index label next to the slice selector slider now uses the current font metrics and the current maximum index to compute accurately the size of the widget.

*Add possibility to find the parent widget when it isn't active.*

*Allow transparency on SSequencer.*

*Vertical sliders are aligned.*

### viz

*Respect initial visibility state and restore it properly in negato_3d.*

*Only reset the crop mask when the image changes, not its content.*

*Dead lock when updating mask.*

*Display of triangle meshes that use per cell colors.*

*Avoid infinite loops in volume rendering box crop.*

*Make Ogre aware of GLX context switches made by Qt.*

*Grid no longer disappear when viewed from the top.*

*Memory leak when a texture is unloaded.*

*Remove unnecessary render requests.*

*Add missing slicesCross argument to Plane in SNegato2d.*

*Crash when zooming VR without autoresetcamera.*

## Refactor:

### core

*Remove obsolete get_temp_path() and get_temporary_folder().*

*Homogenise all timestamping mechanism.*

- replace occurrence creation timestamps with hires_clock functions
- use of system_clock instead of high_resolution_clock whenever possible
- use of std`::chrono::steady_clock`when more suitable

*Apply updated coding-style with clang-tidy.*

*Local and parameter variables renaming in snake_case.*

*Mass-rename of core namespace in snake_case.*

- The core namespace is now entirely in snake_case
- New() is replaced by std::make_shared<> or custom make for factory functions,
- dynamicCast() is replaced by std::dynamic_pointer_cast<>(),
- Most interfaces named in I* are simplified
- `base` namespaces renamed to __ on the filesystem and simply removed for namespaces (`ui`::base``becomes `ui`)
- Mass renaming for layout "managers" and dialogs

### ui

*Remove obsolete ui::signal, ui::starter and ui::default_action services.*


# sight 23.1.0

## New features:

### io

*Filters dicom series by sop class.*

Use the new `module`::io::dicom::SReader``configuration: `sopFilter="1.2.840.10008.5.1.4.1.1.2"`
-or- call `sight::io::dicom::Reader::setFilters({sight::data::dicom::sop::Keyword::CTImageStorage})`

*Add the Spatial Fiducials IOD read/write feature.*

These changes add the Spatial Fiducials IOD read/write feature to the existing DICOM reader/writer.

### ui

*Predefined camera enhancement, cardinalLayout area maximization and accordion menu fix.*

also:
- add a modifyLayout slot for custom layouting actions
- SAction can now trigger custom "parameter_t" signals
- Remove various unused constants which were breaking clang build
- Make the animation to take render time into account. This makes the RPM speed somewhat constant.
- Bad size for non button widget in accordion menu

*Add a slider to modify transfer function opacity.*

### viz

*Allow disabling predefined camera animation.*

*Allow moving landmarks from selected groups outside edit mode.*

`sight::module::viz::scene3dQt`::adaptor::SLandmarks``has been improved to support a `MOVE` mode when not in `EDIT` mode. The `MOVE` mode is controlled by `enableMoveMode()`, `disableMoveMode()`, `toggleMoveMode()` and `changeMoveMode(bool)` slots. The move can be restricted to specific groups when `modify="groups"` adaptor configuration is used, by setting a list of group in a new parameter of the `configureLandmarks()`. `configureLandmarks()` takes now a `sight::viz::scene3d`::LandmarksConfiguration``structure, which is shared by signals, reducing the risk of typo and unwanted API modification.

## Enhancement:

### io

*Update minizip to 4.0.1.*

### ui

*Improve SSequencer appearance.*

This improves a bit the appearance of SSequencer and above all, make it more customizable. The buttons and the fonts size can now be adjusted as well.

ExActivities was also fixed.

*Add the possibility to only allow moving landmarks from a given group.*

We added the possibility to allow moving landmarks only for a given group. It adds the configuration key `modify` for `sight::modules::viz::scene3DQt`::adaptor::SLandmarks``with the possible values `all` (the default, which allows to modify all landmarks, ignoring the current group) and `group` (which allows to modify only the landmarks belonging to the current group).

*Allow modifying the displayed columns in the series selector widget.*

Add a new configuration in `module`::io::dicom::SReader``like: `displayedColumns="PatientID,Modality,Description,Date,Time,BodyPart,PatientPosition"` to configure the displayed columns

*Password must be strong enough.*

*Add contextual mode and menu for fiducials.*

These changes introduce an editing mode for landmarks and distance. Landmarks and distances are now immutable by default, their respecting editing mode must be on in order to be created, removed, moved and renamed. Now, in order to remove a landmark or a distance, one must click on it while the editing mode is on and then click on the bin button which appears. In order to rename a landmark, one can click on the label and directly edit it. Note that, right now, the landmark groups have names, not individual landmarks; therefore, to rename a landmark will actually modify the name of its group.

### viz

*Add option to disable validation through double click for SShapeExtruder.*

*Add other slice lines on negato 2D views.*

*Update landmark/distance adaptors to work on data::Series.*

## Bug fixes:

### build

*Fix clang-15 build.*

### core

*Add missing copy operators in FiducialsSeries.*

### viz

*Display of 2D image in a 3D scene.*

*Store the VR clipping matrix in local instead of world.*

This makes this transform more portable, for instance when the image is moved/registered between the time the clipping is made and the image is rendered.

*Respect the number of samples set in SVolumeRender config.*


# sight 23.0.0

## Bug fixes:

### build

*Fix nvcc configuration.*

*Make build compatible with CUDA.*

*Protect from target coverage redefinition.*

*Build with gcc 12.*

*Provide a project or an application version number  for xml.*

We fixed and enhanced the way to specify version numbers that can be used in XML configuration:
* the `PROJECT_VERSION` variable is now correctly filled and matches the global project version number specified in the CMake `project()` macro.
* a new variable `<target>_VERSION` is now defined for each application, that matches the version used in the packaging, deduced from git tags. If a matching git tag does not exist, it fallback to the latest global tag.

*Clang-15 support and warning fixes.*

### ci

*Try to avoid crashes when a service test fails.*

*Increase timeout for tests.*

*Fix Linux build jobs not outputting test reports.*

*Fix GUI tests crashing because of dangling pointers.*

### core

*Parsing of "channel", "uid" pair for dedicated notifications channels.*

*Error in sync test.*

*Add PB missing enums.*

*Start deferred service if its objects are present or optional.*

*Crash if getMultiFrameGroupSequence is not found.*

Safe return if `getMultiFrameGroupSequence` doesn't exist in current series.

*Random crash in SSynchroniser.*

The `SSynchronizer`::synchronize``function should be run on a separate worker thread. Otherwise, stopping it properly while it runs will never be possible.

By doing that, we introduce an implicit synchronisation when we stop the worker thread, which joins and thus waits for `SSynchronizer`::synchronize``to finish. So we even don't need a more sophisticated synchronisation mechanism with mutexes, condition variables, etc...

However, we discovered three other issues:

* some data objects in the unit-tests were destroyed before the synchronizer service. We modified those unit tests accordingly,
* a deadlock could occur when a slot is destroyed during the asynchronous emission of a signal it is connected to. Now, we ensure the destruction occurs after the emission of the signal,
* a crash could occur when a timer is destroyed before its callback is called.

*Data XML parsers execution policy.*

Data-specific XML parsers are executed whenever an object is created in an XML configuration. We discovered they were also executed with referenced objects, which could lead to objects overwritten with defaults. This happened for instance with transfer functions. We no longer execute the parsers in these cases.

On the other side, objects created in activities do not benefit from this mechanism. This means that after this first change, an activity having a transfer function as a requirement with `create="true"` would create an empty transfer function. Many services do not handle that correctly.

It turns out to be desirable to also allow specifying initial object configuration for requirements. Example from the unit test, which set the value "dummy string" to the requirement `string`:

```xml
    <extension implements="sight::activity::extension::Activity">
        <id>TestBuilderObjectParser</id>
        <title>ObjectParser</title>
        <desc>Desc Test1</desc>
        <icon>Icon Test1</icon>
        <requirements>
            <requirement name="string" type="sight::data::String" minOccurs="0" maxOccurs="1" create="true">
                <config>
                    <value>dummy string</value>
                </config>
            </requirement>
        </requirements>
        <appConfig id="TestBuilderCfg1" />
    </extension>
```

*Mistmatch beween malloc and delete in Demangler.*

*Ensure that removed frame data are removed and not saved with GDCM writer.*

### io

*Prevent SReader to lose its configuraiton on start/stop.*

session`::Sreader`forgets its configuration at stop and start.
Prevent this when the dialog policy is NEVER, because it means that the file-to-read path is only specified in configuration, and should not be forgotten.
*Add openJpg support for dicom writer.*

Some dicom tests writing fail because of an internal mismatch in dicom/nvJpeg2000/openJpeg.

To summarize, for strongly randomized images, nvpeg2000 fails. This has been fixed in the 0.7 version of nvjpeg, used on linux, but not presently on windows.
So, when writing the dicom in a ut on windows, nvJpeg2000 is detected, used, fails, and a backup is used to openJpeg.

However, gdcm, the dicom writing lib, uses openJpeg intrinsecly, and does not support an the external use of openjpeg (bug ! ) which lead,s in the present case to a fail of the nvjpeg2000 backup system.

As a result, some tests fail on windows.

To fix this, a support of dicom writing in onpenjpeg has been added, to make syure that the dicom is writen in any cases

*Add tests and many fixes, optimizations and cleanup for bitmap reader/writer.*

- add read - write tests
- ensure unsupported pixel format / type will throw an exception
- support endianness correctly (libpng)
- cleanup and code factorization
- add some tests
- ensure to throw an exception if nvjpeg2000 is asked for dicom and library is not found at runtime

*Prevent ssynchronizer double lock.*

Prevent SSynchronizer to synchronize in the update in legacy mode.
There is a dedicated worker for that.
This avoids a dead lock in legacy mode.
*Ensure the parent path is created in io::session::SWriter.*

*Correct SSynchronizer test random fail.*

The SSynchronizer copy from the TL to the output vars did a scoped lock, while the buffer was used outside of it. This lead to concurent access and test fail.
The scope has been enhanced to cover the whole copy method  from the tl to the output var.

### test

*Fix GUI test not working on subprojects with Ctest.*

GUI tests didn't work using CTest on subprojects on Linux because of a bug in the `exec_gui_tests.sh` script.

### ui

*Fix bugs with Accordion Menu.*

*Enhance negato slider with window level.*

We improved the negatoscope view to allow the bottom slider to tweak the transfer function window. Before that, it was only possible to do it with a right-click mouse move.

*Replace INFO logging by DEBUG in SStatus.*

*Preferences no longer reset when app version changes.*

A regression caused the application preferences to be dependent on the commit ref. This fixes the behaviour.

*Resize overlay layout according to its children.*

*Transparent notifications with NVidia prime.*

*Typo in SNotifier.*

*Remove useless "SRecurrentSignal".*

*Notification popups doesn't move down when stacked.*

*Fully export class when derived from QObject.*

This allows to use the class as a QObject outside its library and avoid strange missing symbols on windows.

*Fix non-linear sliders value changing too much when scrolling.*

Up to now, the non-linear sliders did too big step, going from maximum to minimum. This has been enhanced to do more progressive changes in non-linear sliders.

*Non-linear slider wrong min/max values when changing values in SParameters.*

These changes fix the wrong min/max values displayed for a non-linear slider in SParameters. This was because the rangeChanged signal was sent with the wrong values, those from the internal slider, rather than those from the non-linear slider.

*First call IGuiContainer visibility slots when hide/show SSignalButton.*

Call first `IGuiContainer` enable/disable and setVisibility slots in SSignalButton.

This fix the bug where is SSignalButton was hiden in a SView, it was impossible to show it again.

*Fix labels being cut off in SParameters with the touch-friendly style.*

When the touch-friendly style is applied, the labels are cut off in SParameters if there isn't enough space. These changes fix that by applying a negative margin to the labels.

*Remove the close callback of notification and force close at stopping of SNotifier.*

### viz

*Compute clipping planes on each rendering frame.*

*Crash when switching an activity when a notification is shown.*

*Make SCamera calibrate safer.*

The SCamera adaptor requires a calibration step.
This calibration can be provided through an input to the service (CameraSet, cameraCalibration). However, for virtual camera in classical 3Dscene, no calibration is provided, and the calibration is computed based on the viewport size.
This set is only called at the service start, and, if the viewport is hidden, or deffined with visibility=false, its height and with are null or negativ.
This leads to an error which stops the application in debug.

Add a calibration verification checK.
If the calibration fails in SCamera, it stops the updating.
If the updating is called and the calibration has not been done, it is calibrated again.

*Correct several update issues with the 3D widget.*

*Shape extruder does not work with touchscreen.*

When using shape extruder on a volume in a 3D scene, and using a mousse, all works fine.
However, on the same machine, same volume, same extrusion path, when doing it with the touchscreen the extrusion takes ages, and freezes the application.

This has been fixed by the way events are handled.

shape extrusion should work perfectly on both touchscreen and mousse actions.

*Fix event managment for touch.*

* Remove `GestureRecognizer`, `EventFilter`, `GestureFilter` as we rely on default Qt one for Pinch and Pan.
* Remove Tap and long Tap, as they are no more used, but it should be easy to add them back, if needed.
* Use synthesized mouse event from Qt gesture, and synthesize the one missing (in SNegato2DCamera)
* Use float version of device pixel ratio to manage correctly fractional scaling (like 1.5 \* full HD)

*Use resize event informations to set aspect ratio.*

The aspect ratio could be broken on some initialisation paths.
Also, fixed the missing label SAxis when the axis are disabled.

*Touch event for landmarks.*

*Update of transfer function widget when image is updated.*

*Display of landmarks and distance over volume rendering.*

*Correct SLine visibility.*

The SLine 3D adaptor is displayed everytime that the line length is changed, even if its visibility is set to hidden.

To fix this, the rendering mechanism has been upgraded to display it only when it is set to visible.

## Enhancement:

### build

*Add sanitizer support.*

Using the CMake option `SIGHT_SANITIZE="sanitizer(s)"` like in `cmake .... -DSIGHT_SANITIZE="thread" ....`

The string is passed to the compiler with the option `-fsanitize=xxx`. Launching the result binaries will execute instrumented code that will check things like concurrent access, memory leak, address uses, etc. One an error is found, the code is stopped with an exception and a meaningful message is printed on the console.

### ci

*Be more permissive.*

*Cache sight-data.*

*Disable gdb to speed-up coverage build.*

*Fix caching with clang.*

*Use a shared linux runner for build jobs.*

*Use direct dependencies between jobs.*

*Use merge-requests pipelines.*

### core

*Tag output synchronized images with timestamps in SSynchronizer.*

*Add blocker support for IContainer<T>::scoped_emit.*

The scope emiter is a shortlived object which watches a IContainer object, and, once destroyed (scope variable destroyed) sends all the appropriate signals regarding what happend to the IContainer (add/remove/changed...).
However, in some cases, we might want to make use of a scope_emiter, though want to prevent it from sending a specific signal.

This is added here, through a block method which can be called

```
const auto scoped_emitter = container->scoped_emit();
scoped_emitter->block(this->slot(IService::slots::UPDATE));

container->push_back(object1);
container->erase(object2);
container->push_back(object3);
```

In this case, the update will not be sent from the container.

*Support start/stop slots connections in XML configurations.*

The `AppConfigManager` has been modified to support the connection of `start` and `stop` slots. A service started by the trigger of a signal will be automatically stopped.

```
<connect>
    <signal>service1/computed</signal>
    <slot>service2/start</slot>
</connect>

<connect>
    <signal>service1/failed</signal>
    <slot>service2/stop</slot>
</connect>
```

This deprecates the service `SStarter` that was used as an alternative so far.

### io

*Add some accessors for serialization.*

- keep the file path of the original DICOM file
- getter for archive file path
- getter for the default (de)serializer function which allow us to call them in a custom (de)serializer
- fix a typo  / coding style normalization
- update unit tests

*Forward the grabbing fps in IGrabber/SGrabberProxy.*

For performance and quality evaluation, it is useful to forward a real-time frame-grabbing frame rate.
A signal is added in the grabber proxy to forward the fps from the grabbers which support this functionality.
Connect ```<signal>GRABBER_PROXY_SRC/fpsChanged</signal>``` to get the fps when it is updated.

*Add NvJPEG2K backend when saving multiframes in DICOM.*

### test

*Update MacroSaver from change to ITest::getProfilePath.*

These changes reflect the changes of GUI test API to the MacroSaver. The main change is the support for the helper API that the code generated by MacroSaver will now use.

### ui

*Reflect changes of presets in the TF editor.*

The transfer function selector now updates the list of transfer functions properly when it changes.

*Make toolbar button icons bigger in touch-friendly apps.*

Some issues with touch-friendly apps were fixed:
* The toolbar buttons are now bigger in touch-friendly apps by default (48\*48 instead of 32\*32)
* When using SStatus with form = circle, the circles are now actually circular for any size
* The slider groove is now bigger in touch-friendly apps to make it easier to use

*Change Play/Pause/Stop shortcut.*

Changes the controls.xml config Play/Pause/Stop shortcuts to config parameters. Space is the default value for play/pause and "s" for stop, though they can be changed in the configuration.
This allows apps to use the space shortcut for their own usage.

*Add swapp support from single volume view to 3/4 view with negatos.*

The classical volume view in sightviewer is a single 3D scene with the volume. It is possible to go to a view with 4 scenes, with the 3D volume view on the top left, and the negato in all 3 directions in the other corner.
It is presently not possible to simply make one of the negato in full screen.

To make this possible, a SEvent service is added, to catch the different events, and forward a particular one.
Once added to the configuration it is used to catch double click, and hide/show the different scenes.
This makes it a lot easier to change the view, and go back.

To keep the classical button functionality, the IAction api has been enhanced to support toogleVisibility.

*Add grabber optimize channel to controls.xml configuration.*

*Prevent SVideo crash on empty image update.*

When providing an empty image (size (0,0,0) ) to a SVideo service, the service does not filter and forwards it to the textureManager which tests it and generates an error in particular in debug mode (ASSERT).

However empty images are often used as default ones, and this leads to various difficulties.

Here, an if(size\[0\] == 0 || size\[1\] == 0) test is added on the SVideo updating and return when it fails.

*Use Qt layout instead of multiple 3D viewports in SightViewer.*

### viz

*Improve distance measurement tool with a click and drag movement.*

The distance measurement tool had some limitations.
In particular, at the creation, the initial process was :

- click on add distance: a distance is created at the ends (0,0) and (width, height) of the image
- drag-drop the created distance ends to the location you want to measure

This was too tedious. The new behaviour is :

- click on add Distance: enter a dedicated mode
- click on the starting point you want to measure
- drag to the end
- drop, the distance is ready.

During your measurement, the distance is automatically updated on the screen, allowing you to visualise what you measure, and the distance between the starting point and present location.

SightViewer has been updated to support the new tool and to have the negato2D and 3D interacting when the distance is created and measured.

*Make Qt overlays compatible with the 3D scene.*

The Ogre Qt widget has been heavily reworked. This was motivated by the need to have Qt widgets as an overlay.

To make this possible, we chose to let Qt make the OpenGL compositing of the whole application. However, this required us to make a part of the composition ourselves. It was no longer possible to use a classic `Ogre`::RenderWindow``because, at some point, it will end up trying to handle its own final back buffer, while we want to render in the FBO managed by Qt. Thus, `Ogre`::RenderWindow``was banned in favour of `Ogre::RenderTarget`. This implied other significant changes to handle viewport resizes with listeners.

Other changes were brought along this rework:

- SightViewer XML configuration has been heavily reworked to use shared configurations. Three new configurations are now available in `sight::config::viz::scene3D` to display a single negatoscope, a 3D scene and four-split views with a 3D scene and 3 negatoscopes.
- Layer transparency: <layer transparency="..." /> if not specified, then the OIT is disabled (you still have transparency, but it is order dependent). This ensures that most render layers do not use an expensive transparency technique if it is not required. If you need OIT, `Default` is the best setting and is an alias for `HybridTransparency`
- The Ogre Overlay was removed because it was hard to make it compatible with the new rendering setup. On top of that, it seems pretty irrelevant to keep two systems to do the same thing. The services that used it, like `sight::module::viz::scene3d`::adaptor::SText``have been switched to Qt.
- In GUI tests, it is now possible to address the views using a `wid` in `SView`
- `SImageExtruder` can now take an optional transform matrix, in the case where the image is moved in the scene (registration for instance)

*Add reset layer camera slot.*

The reset camera slot is connected, on the SRender service, to all layouts of the service.
However, when several layouts are present, it can be expected to reset only a given layout's camera.

To do this, a dedicated slot is created at the service configuration.
The slot name contains the layoutId, and call the resetCameraCoordinates which is specified for the layoutId provided in parameter.

*Use an additional mask for volume rendering extrusion.*

This introduces a mask in the volume rendering, that is used to mark cropped regions instead of marking these regions with a special intensity value. This solves the problem of the computation of the gradient on the edges of these cropped regions and also provides smoother edges. A few extra changes were brought on the way: new toolbar icons for the cropping tool, cleaned properly the extrusions when a new image is loaded.

*Add default tf loading option in TransferFunctionWidget.*

The transferFunction editor widget (and windows) don't zllow to freely specify the a folder with f to pre-load, and the default tfs are always present.

Add two optional parameters to the configs allowing loading pre-registered tf through the path attribute of STransferFunction.

Add support to empty path (default value in the config) in the STransferFunction service.

Changes the TransferFunctionWidgetCfg and TransferFunctionWindowsCfg config id to fetch new standards (sight::config::viz::scene2d::TransferFunctionWidgetCfg)

*Enable binary alpha mixed rendering.*

Binary-transparent surface objects are now rendered properly with volume rendering.

*Add point selection feature on SLandmarks.*

The SLandmark service is a graphical widget which handles the Landmarks, which can be seen as a vector of points. It has a feature to send the coordinates of the current selected point through a signal.

This mechanism is enhanced, to treat a newly created point as a selected one.
In addition, the selected point are outputed in a ```sight`::data::Point````variable in addition to  the signal, to make it possible to treat it as a classical sight data.

closes 1040

*Correct SLIne and SText.*

Make SText input autoconnect to true
Make SLine use getOrCreateTransformNode to get its transform Node

*Add an optional transfer function presets list.*

This adds the ability to specify an optional preset of transfer functions.

## New features:

### ci

*Add a custom CMake command to run gcovr.*

This reduces redundancy across project and make it easier to use.

### core

*Introduce SMultiConfigController.*

The present config launcher allows starting a given XML configuration. SMultiConfigController allows running a configuration selected between a set of configs that share the same parameters. Somehow, it creates a simple interface, where each "subconfig" is an implementation. This can have many applications, in particular in device handling, and will help to make smaller and more generic XML configurations.

*Add API support for fiducials.*

We introduced a new class, `FiducialsSeries`, representing a "Spatial Fiducials" DICOM IOD. `ImageSeries` and `ModelSeries` can both have a `FiducialSeries`. Some modifications were done in `SeriesImpl` in order to add support to sequences of sequences with more than one element.

*Ssequencer / slandmarks fixes and enhancements.*

- Application::exit() can now be synchronous and exit immediatly
- SLandmarks: make "configureLandmarks()" a slot
- SLandmarks: allow to limit the number of landmarks
- SLandmarks: remove also landmarks no more in the data`::Landmarks`when update() is called
- crash when clicking on ok without selecting a DICOM series
- show number of frame in the series description
- SSequencer: add a validate "next" that doesn't enable next activity
- SSequencer: reset requirement that are no more valid
- SSequencer: refresh validity on previous and next activities when calling checkNext()
- cleanup
- use const_lock whenever possible

*Allow per-frame private DICOM attributes.*

*Allow log relocation.*

Add a `relocate_log()` that does exactly what its name suggests. Optionally, it also relocates the previous log files.

Additionally:

* the log singleton is now really a singleton
* `sightlog` can now be launched outside "Sight", which is needed for external unit test
* we have now access to all "binaries" paths from repositories (IE Sight/bin, XXXX/bin, ...the directories that contain executables like `sightrun` and `sightlog`)
* changing the password while using encrypted logs, will indeed also relocate the previous logs, with the new password. This is useful when you plan to submit the logs for analysis and don't want to remember all passwords you used during the work session.
* ..and some gcc-12 warnings have also been fixed.
* add also a special environment variable (try with `export SIGHT_LEGACY_COMPRESSION=1`) that force legacy zip deflate algorithm for all compression operation, instead of zstd. This allows to open archives (logs, sessions, ...) with plain old third party archiver.
* Force non-empty password. Empty password means "no password", which makes things ambiguous and complicated.

*A better temporary directory / file.*

- RAII to ensure the file / directory is deleted
- Convenient conversion / utilities operator
- Thread safe / process safe
- path lenght is minimized
- globally safer

### io

*Allow to configure the baseFolder via a slot in IWriter.*

*Allow to configure and override the transfer syntax used to write Enhanced US Volume.*

Also:

* 10% increase of nvjpeg2000 writing performance (avoiding unnecessary copy)
* fix a huge memory leak while using nvjpeg2000 for DICOM writing
* add profiling test for transfer syntax
* allow using TempDir as a simple RAII directory cleaner
* fix many memory leaks:
  * [valgrind_before.log](/sight/sight/uploads/7fd81924e7cdd842494917b0c09a3b11/valgrind_before.log)
  * [valgrind.log](/sight/sight/uploads/b84702ee5646b42910dfa3d8f679fb3d/valgrind.log)
* fix a possible crash when the window is resized while not yet ready

*Forward folder of loaded session as a signal.*

*Add a new tool to extract data from an archive.*

These changes introduce a new tool, ArchiveExtractor, to extract archives generated by Sight applications. It was designed specifically for opening archive from a real-world application, but it should be generalist enough to allow extract archives from any Sight application, as long as they're salted archives, as they aren't supported by ArchiveExtractor. This application needed a new module, module::io::zip::SExtract, which asks the user for the input file and the output path and eventually the password if needed.

*Implement bitmap readers.*

A reader and a service that allows fast bitmap reading were added.

Some code has been refactored and factorized with the bitmap writers and some bugs have been fixed:
- Image generator / randomizer that can now be called in one step, for all image formats and pixel type.
- nvjpeg2000 flaw mitigation: fallback to openJPEG if the encoded file is bigger than source instead of failing
- Reader / writer code factorization and cleanup

### test

*Add backtrace generation mechanism for GUI tests.*

These changes add helper methods to ease the creation of GUI tests. Some helpers, such as `helper`::Button``or `helper::Field`, operates at the component level, while some others, such as `helper::VideoControls`, operates at the level of a Sight XML config.

Most helper methods use a `Select` helper class to get the components they operate on. For example, one can use `Select::fromDialog("fileNameEdit")` to get the component named "fileNameEdit" inside the currently displayed dialog window. Select has an implicit constructor which take a string, which will select a component within the main window, which is the most common use case. In addition, Select has two setters, `withTimeout` and `withCondition` which allows to set, respectively, a specific timeout to wait for the component to show up, and a condition for it to meet.

A new GUI test was added for SightCalibrator, which is VideoControls. It checks whether SightCalibrator's video controls work correctly (and, incidentally, check that helper`::VideoControls`works).

### ui

*Add accordion menus.*

A new accordion menu is now available in tool bars by enclosing buttons in the `<accordion>` tag. The toolbar in SightViewer was modified so that extrusion-related buttons are shown only if the extruder is activated and distance-related buttons are shown only if the distance mode is activated. The extrusion and distance modes are mutually exclusive, if buttons related to extrusion are shown, buttons related to distance are hidden, and vice versa.

*Notification enhancements.*

- The API has been updated, the code is no more part of `IService`, but has been moved to a new interface `INotifier`. The services that need to send notifications should now inherit from that new interface. The name of the notification method and the corresponding signal has not changed (`notify()` and `notified`), but the signature now uses a `Notification` structure, instead of only the type and the message string.  For compatibility and for convenience, a `notify()` with the same parameters as before is available
- Notifications can now have individual duration, that can be "infinite". The notification will then be "permanent" and will only be closed by a double click. The duration can be set within the `Notification` structure, a parameter of the new `notify()`
- Notifications can now be "channeled", meaning they can be reused, shared and even closed by several services who share the same "channel" keys. The channel name is also passed in the `Notification` structure, but can also be mapped from XML configuration:
  ```xml
  <service uid="testNotifierSrv" type="ExNotifications::SDisplayTestNotifications">
    <notification>
      <channel key="CHANNEL1" uid="a_channel" />
      <channel key="CHANNEL2" uid="${ERROR_CHANNEL}" />
    </notification>
  </service>
  ```
- Many bug fixes / small enhancement:
  - Changing position can now be done dynamically (useful for channels), without waiting that all previous notification to be closed to have a correct placement
  - Use a button to display long text instead of single click (which disallowed closing them !)
  - Fixed long text truncation
  - Race conditions / various crashes fixes.

*Add a Speed Dial button.*

These changes add a Speed Dial button, which displays a list of other buttons when clicked with an animation. The actual "buttons" can be any kind of QWidget, typically QPushButton, but it can also be more exotic widgets such as QOpenGLWidget.

It also adds SIconSpeedDial, which is a service which allows to create a Speed Dial with a list of actions. It is the most simple and typical use case of the Speed Dial and can serve as a reference.

*Create buttonbar widget for SParam enum.*

The SParameter supports enums, though they are not simple to use, in terms of xml and ui.
A new  enum display will be added, corresponding to a bar, with buttons, image and text which allow to select a specific value.

It should have the features of enumSParam, with icons and labels.
The unselected options are gray, while the selected one is in full color.

Gui test are added, to check this feature, and a sample is added in SParameter.

*Provide a view that can be overlayed on 3D scenes.*

A new `OverlayLayoutManager` is introduced, allowing the addition of overlays to a widget. Here is an example of use:

```xml
<service uid="mainView" type="sight::module::ui::base::SView">
    <gui>
        <layout type="sight::ui::base::OverlayLayoutManager">
            <view />
            <view x="0" y="0" minWidth="55" minHeight="100" />
            <view x="-1" y="0" width="50" height="55" visible="false" />
            <view x="0" y="-1" height="35" width="100%" visible="false" />
            <view x="-50%" y="0" width="400" height="300" />
        </layout>
    </gui>
    <registry>
        <parent wid="${WID_PARENT}" />
        <view sid="scenesView" start="true" />
        <view sid="topToolbarSliderView" start="true" />
        <view sid="rightToolbarSliderView" start="true" />
        <view sid="videoSliderView" start="true" />
        <view sid="advancedQueryEditorSliderView" start="true" />
    </registry>
</service>
```
In OverlayLayoutManager, we define a list of views. The first view is the background widget, typically a 3D scene. The other ones are overlays, we can define properties such as ``x``, ``y``, ``width``, ``height`` to indicate where the overlay should be drawn and what its size should be. By default, ``x`` and ``y`` are the offset from the left and the top respectively, however, if they are negative, then it is the offset from the right and the bottom respectively. By default, they are absolute sizes in pixels, however, if ``%`` is appended, then it is relative to the size of the parent widget.

*Add a optional forward / backward button for slider.*

*Allow selecting absolute positions in negato sliders.*

*Makes password dialog customizable.*

Small modifications were brought to allow displaying an image and a custom message for the password dialog used in preferences.

Basically, it is just a couple of parameters for module_ui_base:

```cmake
    module_param(
        module_ui_base
        PARAM_LIST
            preferences_password_dialog_title
            preferences_password_dialog_message
            preferences_password_dialog_icon
        PARAM_VALUES
            "Password required"
            "  Please enter your password: "
            "sight::module::ui::qt/rename.png"
    )
```

*Add Preferences::ignoreFilesystem method.*

*Make toolbar hideable.*

Enhance the IToolBarLayoutManager with a setVisible(bool isVisible) method
Get the toggleViewAction QAction from the toolbar in the ToolBarLayoutManager and store it
Set the toggleViewAction check state and trigger it when isVisible is called
Add slots in the IToolBar to hide/show/setVisibility
add toolbar hide/show to tuto14

*Add non-linear sliders for SParameters.*

A new widget for `type=enum` parameters in SParameters is introduced, `slider`. This will use the new NonLinearSlider widget.

### viz

*Create a pre-defined camera interactor.*

New camera interactor SPredefinedCamera that takes multiple "point-of-view" in configuration. We can navigate through each point-of-view, the camera will go from one to another using a smooth animation.

Mouse track (rotation only) can be disable, if enable you can rotate the camera arround a fixing point, but will be reset at next point of view.

An optional "initial" matrix can also be set, if so camera will first use this matrix and then move to configured point-of-view according to this new position. This matrix can be updated in real time to get a "GPS-like" camera tracking an object.

Others changes:
* remove requestRender from Window,now only interactors should decide when trigger a render
* add requestRender in ClippingBoxInteractor

*Allow to specify the view distance of the landmark.*

...and many fixes on SLandmarks and one in is_less and is_greater.

*Allows to use SCamera with orthographic projection.*


# sight 22.1.0

## New features:

### core

*Add a function profiling utility.*

- fix Warning C4244
- windows pch off support

### io

*Add a DICOM Enhanced-US volume writer.*

The current Dicom Series writer has been updated to call a specialized writer when the SOP class is [Enhanced US Volume](https://dicom.nema.org/dicom/2013/output/chtml/part03/sect_A.59.html).

The new writer is for now minimalist and maybe not 100% DICOM compliant yet, as it requires, nor checks for mandatory tags to operate. However, all frames of a data`::ImageSeries`(on the Z axis) are saved losslessly in jpeg2000, and Series API allows writing/reading per-frame `Image Position Patient/Volume`, `Image Orientation Patient/Volume`, and `Frame Acquisition Date Time`. Even if the writer doesn't ensure other mandatory tags are present, it is still possible (and desirable) to write them using the Series API with the generated tag name/sop class from sight`::data::dicom::Tag`and sight::data::dicom::SopClass. Note however that the current API is limited to simple attributes, not the one located in a frame sequence like in `Shared Functional Groups Sequence`. Some adaptations are required (the same as \`Image Position Patient…). This part will be updated later when needed.

*Use correct gdcm attributes for US volume.*

- ImagePositionVolume, ImageOrientationVolume, FrameAcquisitionDateTime DICOM attribute are now stored into sequence element when using Enhenced US Volume, as before for CT/MR.
- DICOM Tags, SOPClasses, Modules definitions are generated from official documentation using python code. This allows friendlier operations (no need to remember exact tag values, or SOP uids).
- Unit tests have been updated to match changes and use generated DICOM tags, SOPClasses.
- Some bugs with equality operator have been corrected

*Allow forwarding grabber long running jobs.*

*Add optimize slot to IGrabber/SGrabberProxy.*

*Allow specifying a target camera in SGrabberProxy.*

*Nvjpeg writer implementation.*

A "fast" 2D image writer (sight::io::bitmap::Writer) and its associated service (sight::module::io::bitmap::SWriter) have been implemented. The writer uses NVidia CUDA accelerated JPEG / JPEG2000 encoding library: nvJPEG, if support has been compiled in and if a suitable GPU has been found. Alternatively, libjpeg-turbo, openJPEG, libtiff or libPNG can be used as CPU fallback. The performance should be better than VTK or even OpenCV because of direct API calls and avoided unneeded buffer copy.

> :warning: VTK SImageWriter support for 2D bitmap format has been removed. The support was anyway bugged as the image were saved upsode down.
>
> also added:
> - fix on `LocationDialog::getCurrentSelection()` that erased the first char to return an extension from a wildcard filter. Now there is a `ILocationDialog::getSelectedExtensions()` that returns a vector of extension, in case there are more than one extension from a filter.
> - some small fix for high dpi displays in the GUI tester code.

*Add zoom capability in video grabbers.*

We added the possibility to zoom on videos. Some hardware devices offers this possibility, so it is better to use this instead of zooming in the visualization pipeline.

## Enhancement:

### build

*Use /Z7 in all cases and enable ccache for windows.*

### ci

*Enable debug optimized build for GUI tester.*

Our GUI tester is very slow in the coverage build. We enabled optimizations for the tester code, especially for the image comparison functions, hoping that this will decrease the number of false positives.

*Increase test timeout on test coverage.*

### core

*Add private tag support.*

*Lower the test epsilon to be a bit more tolerant.*

*Add strict mode to require matrix presence in SFrameMatrixSynchronizer.*

*Add a simple function to set the location of series instances.*

A function was added to set both the position and orientation of a series instance at once, passing a `sight::data::Matrix4`. Besides this, `sight::data::Matrix4` has been modernized.

### filter

*Make SFrameMatrix synchroniser delay specific for each timeline.*

add a vector for the framesTl and one for the matrixTl to store the delay.
The delay is applied at the synchronization, and the frame/matrix picked is in the past regarding the most recent data, from an amount of delay.
The timelines are associated through their index.

### io

*Un-pause grabber on successive startCamera calls.*

*Skip camera restart when already started.*

### test

*Enhance global test code coverage.*

Many tests were added, especially for the most used services in our applications, that were untested so far.

*Add external project gui test support.*

- rename the gui test core folder to testCore

-change the ITest profile path handling, by geting the full path, and not using "getWorkingDir()". It will be up to the GUI tests to provide the full profile path.

- add the profile modules. The clasicall sightrun looks for the modules situated relatively to the profile path. This allows to load automatically the project modules; This will be done in the ITest too, at the test setup()

-add -B bundlesPath in the cppunit_main. Similarly to classical applications, the -B argument allows specifying additional bundles path. This will be handled in the unit test too.

- change test templates to add external libs path in the PATH variable  and bundles path

- in the eventHandling, verify that the event catched is a testEvent, and do nothing if it is not one.

-install exec_gui_test and copy it in sight-projects build bin dir

*Improve operator== tests for Data Objects.*

### ui

*Use unique slot to configure Grabbers.*

* rename some slots from SParameter from "set" to "update" for clarity
* also add a vector of string as part of the parameter_t variant to be able to update all values of enum widget

*Emit signal with value and key from SPreferenceConfiguration.*

The signal, called `preferenceChanged`, has the same signature as the signal `SParameter::parameterChanged`. This simplifies the API when the service needs to listen to a parameter that can be changed from preference or/and from a`SParameter`.

An example of usage is shown in ExSParameter.

*Use a 3 columns layout for SParameters.*

These changes modify SParameters layout to use a 3 columns system instead of the current 6 columns system, to improve the usability of the second and third spinboxes. In addition, the labels of the parameters now can wrap if they are too wide, and a bug where the second and third spinboxes wouldn't be disabled if the depend is disabled is also fixed.

*Apply SParameters' stylesheet globally.*

These changes apply the stylesheet from https://git.ircad.fr/sight/sight/-/merge_requests/795 globally. It can be enabled by setting the `"touch_friendly"` application option to `"true"` at configure time.

*Add a generic signal/slot parameter type.*

*Improve hitboxes for Q*SpinBox controls.*

*Rename toolbar.xml to controls.xml and add more signals.*

=> add some out signals
=> disconnect fileConfigured with video play
=> add documentation

### viz

*Add slot in SShapeExtruder to replace right click.*

These changes add a slot called "cancelLastClick" to cancel the last click in the shape extruder. This allows to replace the right click in context where it isn't available, such as in a touchscreen context.
In addition, this adds a button in SightViewer which triggers this slot when clicked.

*Adapt default transfer function to a reference image.*

The default transfer function can now be adapted to the image pixel type, especially for ultrasound images.

The `data`::Image``no longer holds a default transfer function. To get a default transfer function, you have to use `sight`::module::ui::qt::image::STransferFunction``and specify a reference image as input. This is no issue if you don't want the transfer function selector in the user interface. You can register the service in an invisible view. You can have a look at `Tuto07GenericScene` for instance.

Also, some bugs were fixed in SNegato2DCamera for the computation of the viewport of the axial view.

*Add gestures to scene 3D.*

These changes will allow to use gestures on the 3D scene. It allows in particular to pinch fingers to zoom and to long tap to place a landmark. Simple taps and pan gestures should intuitively map to mouse clicks and mouse drags.

## Refactor:

### build

*Use built-in CMake PCH functions.*

### core

*Introduce SSynchronizer to replace all frame/matrices sync services.*

This refactors fundamentally the synchronization between frames and matrices, by bringing a single service SSynchronization that replaces all the previous ones: `SFrameMatrixSynchronizer`, `SMatrixSynchronizer`, `SMatrixTLSynchronizer` and `SFrameUpdater`. It is much more versatile to answer all the different use cases and his behaviour is well tested.

*Inherit data::Activity from data::Composite.*

*Separate service public and private API.*

We reduced the size of the public API of the IService interface, which is undoubtedly the most used in Sight.

First, the data management responsibility has been moved to the IHasData interface and now holds the getters and setters for inputs, in/outs and outputs. Secondly, a significant part of the implementation of IService methods has been moved to a pimpl in service`::detail::Service`and service::detail::ServiceConnection.

The AppConfigManager was moved in the detail namespace as well, so now the public API only exposes IAppConfigManager.

This refactor also finishes the migration of data::ptr. Now the objects of services are only held in data::ptr, the former input, inout and output maps of IService were removed, which simplifies a lot the code. This also implies that services can no longer specify any data in the XML configuration that is not declared as data::ptr. In this repository, only two services relied on the old behaviour SConfigController and SConfigLauncher. The migration consisted in using a single declared group of objects (data::ptr_vector<data::Object>) instead of multiple undeclared objects. Changes were brought in all XML configurations.

Last, the ObjectService registry refactor was also finished. Its role has been reduced because we no longer rely on it to store the relationship between services and objects. This responsibility is filled by the services themselves. So now, this registry only holds a global list of registered services. It has been renamed accordingly. Doing so, the sight`::service::registry`namespace was removed, and the only other class that was there, sight`::service::registry::Proxy`was moved to sight::core::com::Proxy. Indeed, it has actually no dependency at all on services and is generic enough to be in the core`::com`namespace.

*Deprecate IService::getConfigTree().*

*Remove ConfigurationElement totally.*

*Remove AppManager and Qml apps based on it.*

*Replace ConfigurationElement in IService.*

*Replace ConfigurationElement in all service configurations.*

### io

*Create a widget & window configuration for pacs selector.*

* the widget config is called by the windows config
* widget can be used if the pacs selector should be integrated in a existing window

*Create macros `SIGHT_REGISTER_SERIALIZER` to ease the serialization's function registration for each data.*

### viz

*Simplify layer configuration for scene3d adaptors.*

The scene3d adaptors no longer specify their layer. Instead, it is implicitly declared in the `SRender` configuration by putting the adaptors as children of the layers tags.

```xml
<service uid="genericSceneSrv" type="sight::viz::scene3d::SRender" >
    <scene>
        <background topColor="#36393E" bottomColor="#36393E" />

        <layer id="first" order="1" />
            <adaptor uid="trackballInteractorAdp" />
        </layer>

        <layer id="second" order="2" />
            <adaptor uid="negatoAdp" />
        </layer>

    </scene>
</service>
```

*Store transfer function as 2D Nx1x1 texture.*

## Bug fixes:

### build

*Update dependency URLs.*

*Use relative paths in launcher installed scripts.*

*Only install header files when necessary.*

This prevents rebuilding a child repository each time sight is installed. To achieve this, a custom target was added to generate the library headers without `*_API` macros in the build folder, and then copy these at install. Previously, we modified the files directly in the install folders, which led to always modify the timestamp of the headers, thus triggering the rebuild.

### ci

*Use 'fetch' strategy instead of none on Windows.*

On Windows, we do not have docker runners. Thus the build folders are reused between jobs. GIT_STRATEGY='none' is faster but does not provide the clean stage that is performed with 'clone' and 'fetch'. Because of this, we may end up with artifacts from previous jobs in the deploy stages. To overcome this, we use the 'fetch' strategy. It is useless to get the sources, but, this gives us the clean stage which is impossible to do in a build script.
See https://gitlab.com/gitlab-org/gitlab/-/issues/17103

*Doxygen deployment.*

### io

*Use new bitmap writer for screenshots.*

*Ensure serializer register is unique.*

*Make loop great again.*

reset the timelines when reading process reached the end and is in loop mode
add loop mode (slot + config + mechanism) in SMatricesReader
make SMatricesReader pause system similar to SFrameGrabber one

*Serialize & deserialize correctly empty objects.*

- Data::Mesh
  * do not resize if mesh has no points / cells
  * Add VTK_EMPTY_CELLS in switch case
- Data::Array
  * do not resize if deseralized array is empty
- Data`::Image`/ ImageSeries
  * initialize Image windowCenter and windowWidth to empty vectors
- Data::TransfertFunction
  * mistake in operator==
  * also test pieces vector
  * implementation of operator !=
- Misc
  * Remove Graph, Node, Edge, Port, ReconstructionTraits, ROITraits Data
  * Use operator== to compare all data in sessionTest

### test

*Handle external modules properly in test bat and bin.*

*Improve LoadDicom GUI test sensitiveness.*

### ui

*Fix password management for preferences.*

* restore BadPassword exception
* changed inputDialog to return value and a boolean to know if user canceled it
* allow closing sight if user cancel or if wrong password have been entered 3 times
* updated ExActivities plugin configuration to force encryption and to exit on password error
* "once" passwod policy configuration has been changed to "global": "global" which means the global password will be used, if set, otherwise or if wrong, it will be asked to the user
* "default" configuration has been removed. User should simply not set any configuration if they want the default to apply

*Add a slot in SLandmarks scene 3d adaptor to take care of landmarks' group renaming.*

*Replace delete by deleteLater in QtContainer class.*

To get rid of the deletion order we use deleteLater() function on the m_container QPointer.

*Destroy the GUI elements in the reverse order in IGuiContainer.*

### viz

*Use a read lock on the image when possible in SNegato2DCamera.*

When receiving the resize event, we may end up with a deadlock if the image is already locked in writing. This may happen for instance with a writer that uses a progress bar.

*Switch current drawable when swiching OpenGL context.*


# sight 22.0.0

## Enhancement:

### build

*Update VCPKG.*

VCPKG packages has been rebuilt, and the hash has been changed. All dependency packages have kept the same version number, so it is unlikely that a change of behavior occurs.

*Remove libxml2 from the core public interface.*

*Upgrade code to support C++20, and use some new features.*

* Build with C++20 standard
* Update the code to fix the following deprecations:
  * implicit capture of ‘this’ via ‘\[=\]’ is deprecated in C++20 (see [reference](https://www.nextptr.com/tutorial/ta1430524603/capture-this-in-lambda-expression-timeline-of-change) )
* Replace usage of `boost`::ublas``by glm (the current version does not build with C++20, Debian is shipped with an old version of boost...)
* Use a new vcpkg build with a patched ITK version for C++20, and thus some minor adjustments because of new package versions (DCMTK and openCV for instance)
* Use std`::ranges`algorithms in some places just for fun :heart_eyes:

*Update VCPKG packages.*

### ci

*Disable debug dialog on Windows on the CI.*

When a unit test fails with an assertion failure in Windows Debug, a debug dialog will appear. This is useless in the CI, where physical access is required to view the dialog and know where is the problem. These changes remedy this, by disabling debug dialog and printing messages in the console when the environment variable DISABLE_ABORT_DIALOG is set.

*Disable thread information from GDB in the CI.*

*Use Sheldon's Clang-tidy hook for lint job.*

Now that Clang-tidy is added as a hook for Sheldon, we can use it instead of using
Clang-tidy directly, to reduce code redundancy.

*Check whether packaged applications are executable.*

Existing testing to prevent regressions isn't sufficient, as experience has shown. The packaged applications will now be tested automatically in the CI: SightViewer and SightCalibrator will be installed, and then executed in order to check if they start successfully or crash because of missing dependencies or various errors during installation. Also, all examples, tutorials and utilities will be launched to verify, at least, that they can start and stop.

*Add Clang-Tidy to the CI.*

*Add some metrics.*

This shows the number of warnings and deprecated declarations in the merge request metrics reports.

### core

*Create the log in user cache directory by default.*

Default log file path, when nothing is specified with --log-output in the user "cache" directory, which is given by $XDG_CACHE_HOME on Linux or simply $APPDATA on Windows. As a fallback, the default temporary directory is used. In both cases, the path is suffixed with ./sight/<profile name>. The final path is also displayed on the console.

> VTK.log and Ogre.log has been taken into account...

Some (light) adjustment and cleanup have also been made in core`::tools::Os`to manage "cache" directory and return a std`::filesystem::path`instead of a std::string.

this could be a possible breaking change as you may have to call std::filesystem::path::string() when calling getUserConfigDir() (already fixed in this MR).

getUserDataDir() name is misleading. Since it returns XDG_CONFIG_HOME it should be called getUserConfigDir(). It was hard to not change this in this MR for me, so it was done....

And last but not least, a small "fix"  in PreferencesTest to clean correctly the user config directory after the test pass. I come through it while changing the getUserDataDir() returned type to std::filesystem::path

*Remove hard-coded label value to generate mesh.*

provide a from input the value to generate mesh and implements the unit tests for the codes.

*Enable usage of lamba functions in slots.*

It is now possible to use lambda functions in slots. For instance, this declares  a slot making the sum of two integers :

```cpp
auto slotSum = core::com::newSlot( [](int a, int b){ return a + b;} );
```

*Improve camera resolution selection.*

The camera service selection now provides a new setting `<resolution>` allowing to automate the choice of the resolution. The value can be `min/max/WxH/preferences/prompt`.
- `min`: the minimum resolution is automatically chosen.
- `max`: the maximum resolution is automatically chosen.
- `WxH`: the exact resolution is set (for instance 1280x720). Beware because if the device does not support the resolution, an error message will be displayed and the camera will not be available.
- `preferences`: default value, the dialog is shown on the first time and then the choice is stored in the preferences of the application. This choice will then be used each time. An extra button is displayed next to the camera selector to change the resolution later.
- `prompt`: former behavior which makes the resolution dialog to always prompt.

*Add initializer_list constructor support for KeyConnectionsMap.*

A new constructor was added to KeyConnectionsMap allowing to shorten the implementation of `IService::GetAutoConnections()`. For instance :
```cpp
    IService`::KeyConnectionsMap`SSample::getAutoConnections() const override
    {
        return {
           {"data1", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT},
           {"data2", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT}
        };
    }
```
instead of :

```cpp
    IService`::KeyConnectionsMap`getAutoConnections() const override
    {
        KeyConnectionsMap connections;
        connections.push("data1", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data2", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

        return connections;
    }
```
The previous declaration is still possible. Few implementations were changed as example.

*Secure library loading when upgrading sight version.*

When executing sight in build tree, we may load the wrong version of libraries when upgrading. Now, we use the ."so" symlink first to be less sensitive to version change.

### geometry

*Remove charuco related codes.*

### io

*Add optional receive timeout on igtl server.*

* when using the timeout receive fonction aren't blocking
* also remove client from vector when disconnected

### navigation

*Download openvslam vocabulary only when SOpenvslam starts.*

* create a function to donwload file using curl in io_http library
* minors fixes on SOpenvslam, SFrustum & SFrustumList to match latest updates

### test

*Add tests for the HiResTimer class.*

*Improve geometry::data::Mesh::transform unit-test.*

The unit test for `geometry`::data::Mesh::transform``was pretty weak, as the test was mostly the same code as the tested code. The method is now tested with a hardcoded small mesh which is transformed, with all combinations of types possible (only points, point and point normals, point and cell normals, point and all normals).

### ui

*Add an optional scroll bar for SParameters.*

A `scrollable` property was implemented in `SParameters`, which adds a scroll bar to the right of the widgets.

*Add a new signal to notify if the next activity is ready.*

Two signals were renamed in `sight::module::ui::qt::SSequencer`:
  - `enabledNext()` -> hasNext()
  - `enabledPrevious()` -> hasPrevious()

One signal was added, which is triggered when the next activity is enabled (all requirements are satisfied):
  - `nextEnabled()`

*Simplify notification API by using single signal & slot.*

* One signal "notified" in IService containing the type & the message
* One slot "pop" in SNotifier reading type & message and displaying the corresponding notification popup

*Move the modal transfer function editor in the right panel in SightViewer.*

### viz

*Add pixel values in 2D negatoscopes.*

We added the ability to pick pixel values in 2D negatoscopes, in the same way than it was already done in 3D negatoscopes.

*Use image GPU resource sharing in all adaptors.*

*Speedup the tf upload by 40x.*

*Set SNegato2D adaptor transformable.*

*Merge ExOgreRGBDStream and ExRealSense together.*

The ExOgreRGBDStream sample was somehow useless since the only RGBD camera we do support is the IntelRealsense, which is demonstrated in ExRealSense. We added a missing feature from ExOgreRGBDStream into ExRealSense, the transfer function editor. The sample was also modernized a bit and debug optimizations were enabled in some modules to keep good performances in this build type.

*Merge SAxis and SScaleValues together.*

*Enlarge the viewport shutter to contain histogram and tf values.*

## Bug fixes:

### build

*Fix various CPPCheck warnings / errors.*

*Change windows export handling on lib with QT in the name.*

*Small fixes to support gcc / libstdc++ 12.*

*Configuration issues with GLM.*

Since GLMConfig.cmake is correct on Windows and buggy on Ubuntu, we use a different approach depending on the platform:

* Use the old-ish system with `target_include_directories` on Linux
* Use `glmConfig.cmake` on Windows (but everywhere this time)

*Clang-tidy warnings.*

Here is the complete list:
  - readability-duplicate-include
  - readability-container-data-pointer
  - cppcoreguidelines-virtual-class-destructor
  - cert-err33-c
  - readability-identifier-naming
  - cppcoreguidelines-pro-type-cstyle-cast
  - clang-analyzer-core.NonNullParamChecker
  - clang-analyzer-core.NullDereference
  - clang-analyzer-core.CallAndMessage
  - cppcoreguidelines-non-private-member-variables-in-classes
  - google-explicit-constructor,hicpp-explicit-conversions
  - bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp
  - modernize-use-using
  - cppcoreguidelines-macro-usage
  - bugprone-macro-parentheses
  - hicpp-exception-baseclass
  - clang-analyzer-security.insecureAPI.strcpy
  - bugprone-too-small-loop-variable
  - bugprone-inaccurate-erase
  - cert-flp30-c
  - bugprone-incorrect-roundings
  - bugprone-suspicious-include
  - google-build-explicit-make-pair
  - google-global-names-in-headers
  - readability-redundant-control-flow
  - bugprone-throw-keyword-missing
  - readability-static-definition-in-anonymous-namespace
  - readability-string-compare
  - hicpp-move-const-arg,performance-move-const-arg
  - cppcoreguidelines-interfaces-global-init
  - readability-misleading-indentation
  - cert-err09-cpp,cert-err61-cpp,misc-throw-by-value-catch-by-reference
  - bugprone-parent-virtual-call
  - modernize-redundant-void-arg
  - misc-unused-using-decls
  - performance-inefficient-algorithm
  - modernize-make-unique
  - readability-suspicious-call-argument
  - bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions
  - bugprone-undefined-memory-manipulation
  - google-readability-function-size,hicpp-function-size,readability-function-size
  - readability-container-size-empty
  - bugprone-integer-division
  - readability-use-anyofallof
  - cppcoreguidelines-slicing
  - modernize-raw-string-literal
  - performance-unnecessary-copy-initialization
  - boost-use-to-string
  - readability-redundant-smartptr-get
  - hicpp-use-emplace,modernize-use-emplace
  - cert-msc32-c,cert-msc51-cpp
  - cppcoreguidelines-pro-type-const-cast
  - hicpp-multiway-paths-covered
  - performance-inefficient-vector-operation
  - cert-err34-c
  - performance-faster-string-find
  - clang-analyzer-optin.cplusplus.VirtualCall
  - readability-isolate-declaration
  - readability-qualified-auto,llvm-qualified-auto
  - clang-analyzer-deadcode.DeadStores
  - google-build-using-namespace
  - readability-delete-null-pointer
  - bugprone-implicit-widening-of-multiplication-result
  - misc-unused-alias-decls
  - readability-uppercase-literal-suffix
  - bugprone-branch-clone
  - performance-inefficient-string-concatenation
  - bugprone-misplaced-widening-cast
  - cppcoreguidelines-no-malloc,hicpp-no-malloc
  - performance-no-automatic-move
  - performance-for-range-copy
  - modernize-loop-convert
  - readability-inconsistent-declaration-parameter-name
  - modernize-avoid-bind
  - bugprone-forward-declaration-namespace
  - modernize-make-shared
  - performance-trivially-destructible
  - cert-dcl21-cpp
  - modernize-use-transparent-functors
  - readability-implicit-bool-conversion
  - cppcoreguidelines-init-variables
  - hicpp-noexcept-move,performance-noexcept-move-constructor
  - modernize-pass-by-value
  - hicpp-named-parameter,readability-named-parameter
  - google-runtime-int
  - hicpp-use-equals-delete,modernize-use-equals-delete
  - cppcoreguidelines-c-copy-assignment-signature,misc-unconventional-assign-operator
  - cert-oop54-cpp
  - readability-simplify-boolean-expr
  - modify .clang-tidy configuration file
  - bugprone-sizeof-container
  - llvm-else-after-return,readability-else-after-return
  - hicpp-use-auto,modernize-use-auto
  - cppcoreguidelines-pro-type-member-init,hicpp-member-init
  - cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays
  - hicpp-deprecated-headers,modernize-deprecated-headers
  - hicpp-use-equals-default,modernize-use-equals-default
  - readability-redundant-string-init
  - readability-redundant-member-init
  - modernize-use-default-member-init
  - hicpp-use-nullptr,modernize-use-nullptr
  - cppcoreguidelines-prefer-member-initializer
  - readability-avoid-const-params-in-decls
  - llvm-namespace-comment
  - modernize-return-braced-init-list
  - readability-const-return-type
  - modernize-use-nodiscard
  - readability-make-member-function-const
  - readability-convert-member-functions-to-static,readability-static-accessed-through-instance
  - modernize-concat-nested-namespaces
  - hicpp-use-override,modernize-use-override
  - cert-dcl58-cpp

*Small corrections on filter_image to build with ITK5.*
* disable spheroidExtraction
* Add missing components

*Move Ogre plugins next to the Ogre libraries.*

*Remove warnings in modules.*

These changes remove warnings in Sight modules. These warnings were mostly harmless, but could hide potentially harmful problems.

*Remove warnings in libraries.*

These changes remove all compile warnings in Sight libraries. These warnings were mostly harmless but could hide potentially harmful problems.

*Correct windows application packaging.*

*Forward script arguments when using privilege escalation on Windows.*

### ci

*Windows SDK package made relocatable.*

Sight windows SDK package is now relocatable. All mentions of the build tree are removed with careful usage of CMake. A test has been added to the CI to prevent any regression.

*Reorder plugins loading in Tuto01DataServiceBasicCpp to avoid crash at exit.*

*Migrate artifacts download to nextcloud.*

*Remove redundant coverage report in Sight config.*

### core

*Detection of runtime library folder.*

*Reset the count before resetting BufferObject to avoid lock count assert at buffer destruction.*

Resetting the counter in the destructor **BEFORE** resetting BufferObject shared pointer is required ! Otherwise, the lock count assert in the destruction of the buffer, in `BufferManager::::unregisterBufferImpl()` will be triggered.

Also remove hack in SDistorsion service.

*Parsing of transfer function data in XML.*

The transfer function data can be initialized directly in XML configurations. Because of a regression, the parsing was broken and caused a crash at start.

*Really disable SIGHT_DEFAULT_PASSWORD when not set by user.*

*Consider multi arch folders in runtime directory detection.*

*Use priorities to unload modules loaded after profile start.*

Priorities are now used to unload all modules, even those loaded after the initial start of the profile. This allows notably to unload `appXml` first, and then modules like `module::viz::scene3d`.

To achieve this, the start and priority properties are now set directly in the `plugin.xml`. The priority is read at runtime when parsing modules. It is used both to start and to stop modules. Some code cleanup was performed, which led for instance to remove the over-designed Starter and Stopper classes. This part of the profile management should be clearer now.

The sorting of modules in the generation of the `profile.xml` is _de facto_ no longer necessary and has thus been removed to simplify the code.

*Set the log output in the main of unit-tests.*

The redirection of the log into `fwTest.log` is performed by a singleton in sight::core::utest. We link with this library in coreTest, but we do not use any symbol from it. Thus the linker strips it and so in the end, we do not initialize the log output.
To fix this, we simply initialize it in the main file of unit tests.

*Prevent race condition in serviceTest.*

*Prevent race condition when adding a new id.*

### geometry

*Update intrinsic parameters in SPoseFrom2d.*

When intrinsic parameters, such as the resolution of the camera, change, the computation of the 2D pose did not take this into account and used the parameters set at start. Now we listen the changes of the parameters and update accordingly.

### io

*Use a patched ITK header when using clang + Ubuntu 22.04 support.*

- update sight-deps and use Ubuntu 22.04 image
- vtk mesher has been fixed upstream

*Overwrite the seriesDB in the ITK reader, make *.nii the default.*

*Add patched version of buggy igtl functions.*

* Add a modify version of igtlSocket.h header, included first it allows public access to socket descriptor (needed for a proper closing function)
* Patched version of createServer to call patched version of bindSocket()
* bindSocket: remove need to define VTK_HAVE_SO_REUSEADDR to add SO_REUSEADDR option in socket

*Throw exception for common socket error (timeout, ...).*

* INetwork will now throw exceptions if timeout, network error, or size mismatching of received data instead of returning a nullptr of igtl::MessageBase.
* Create unit test to test server/client communication

*Do not restore the UUID of objects when deserializing.*

Restoring the UUID of objects implies that a serialized object is really unique. Thus if you read a file containing a object twice, you expect to have only on object in memory.
This is definitely not the behavior we want, we expect the object to be duplicated each time it is read, with its own UUID.

### navigation

*Remove vocabulary download from cmake.*

Since the download is now performed at runtime in C++

### test

*Change float comparison equality strictness of copies.*

### ui

*Deadlock when exporting transfer function.*

Because SWriter`::update`locks data and so does the function before calling this, a deadlock occurred.

*Allow to use subtoolbars in toolbars.*

This fixes two bugs preventing from using sub toolbars in toolbars:
* parsing error in `<editor wid="...">` tag attribute in the toolbar config
* wrong alignment used in child toolbars

*Block signal when enum parameter is updated in SParameter.*

* also look in both item text and data as fallback to find value of enum

*Initialize the type selection combobox with the right value.*

*SNegato2D Camera crash when a single frame is provided.*

*Run appXml code on the main worker thread.*

This fixes a regression caused by the removal of the second initialisation stage of `IPlugin`. All `initialize()` functions, notably the one from AppXml, were called from the Qt event loop. The function loading the stylesheet is posted just before on this Qt event loop, which implies it is called before the initialisation of `AppXml`. With the removal of `initialize()`, the `start` function of `AppXml` was called directly from the main thread, before the start of the Qt event loop. Thus, the function loading the stylesheet was executed after the start of the application, causing rendering issues.

It seems safer anyway to keep the previous behaviour. So, `AppXml` was slightly modified to execute the start of the application on the Qt event loop.

*Various issues when loading meshes in SightViewer.*

- random crash when the camera is not reset
- reset the camera when a model is loaded in SightViewer
- enable the hide/show model when a model is loaded
- do not enable the image buttons when a model is loaded

### viz

*Display of images with 32 bits integer pixel format.*

This restores partially the behavior prior to 1353d895 that converts 32 bits integer images into 16 bits images when uploading them on the GPU.
This fixes a regression that prevents from reading all 32 bits integer DICOMs.

*Avoid deadlock in viz::scene3d::IParameter with textures.*

*Invert Plane constructor param to fit SNegato usage.*

*Out of bounds writing in function transfer.*

An error was introduced that could fill one pixel too many, causing a crash when deleting the transfer function, potentially when exiting the application on Windows.

*Add a workaround for NVidia Prime to solve a deadlock with several 3D scenes.*

*Blend transfer function pieces proportionally to their opacity.*

*Do not allow to move TF points outside the viewport.*

*Do not reset the opacity when changing the color of a TF point.*

*Change Ogre plugin location.*

Ogre needs to locate its plugin at the start, in a similar way as Qt does. Over the years, we had different strategies to locate them which always fails in the end in some corner cases. Well, I finally propose to use the exact same strategy as we do for Qt, which is to locate the core Ogre library and then use a relative filesystem path from there. It has proved to be a winning strategy with Qt.

## New features:

### core

*Forward SParameters signals from SGrabberProxy and IGrabber.*

*Remove SeriesDB, Activity and Camera are stored in separated set.*

- rename ActivitySeries to Activity
- rename CameraSeries to CameraSet
- rework deep/shallow copy to use full inheritance and to copy parent class
- change ImageSeries inheritance to Series and Image
- remove Patient, Study, Equipment data object and use GDCM to store DICOM attributes

*Add generic container classes.*

#### Description

Implements a generic templated data container class, aka `IContainer`, that allows code factorization from all containers. STL API from the corresponding STL container classes is exposed and the inherited containers, such `Composite` and `Vector` can be seen as a real `std`::vector``and `std`::map``from outside (which they indeed are !), while keeping to be data objects, that can be used in XML, that can be serialized, etc..

Since it is a part of https://git.ircad.fr/sight/sight/-/issues/862, some containers (`ActivitySet`, `CameraSet`, `SeriesSet`) have been also implemented. Although not used yet, you can still take a look at them, as `CameraSet` uses a more complex `std::vector<std::pair<Camera::sptr, Matrix4::sptr>>` while `ActivitySet` and `SeriesSet` use a [Boost Multi-index](https://www.boost.org/doc/libs/1_79_0/libs/multi_index/doc/index.html), which allows to define a `set` which is sequenced like a `vector`.

##### Usage

The best open `Composite` or `Vector` and `IContainerTest` code, but basically, all you have to do if you need a specific data object container class, is to inherits from `sight::data::IContainer<XXX>`, with `XXX` being your STL container type (or boost STL compatible container, like Multi-index !). As an example:

```c++
#include "data/IContainer.hpp"

class DATA_CLASS_API Vector final : public IContainer<std::vector<Object::sptr> >
{
...
};
```

You can then use it like a real std::vector<Object::sptr>:

```c++
auto vector = sight::data::Vector::New();

// 99.9% of STL API is available
vector.reserve(2)
vector.push_back(sight::data::Integer::New(1));
vector.push_back(vector.front())
vector[1] = sight::data::Integer::New(2)
...

// Initializer list / assignment operators
vector = {sight::data::Integer::New(1), sight::data::Integer::New(2), sight::data::Integer::New(3)};

// iterators are supported
for(const auto& element ; *vector)
{
    std`::cout`<< element->getValue() << std::endl;
}

```

##### ScoppedEmitter

There is also a generic `ScopedEmitter` which replace various "helpers", that were used to send signals when adding / removing objects in the container. It uses RAII mechanism to send the right signals when it is deleted.

To use them, simply call the generic `scoped_emit()` function form a container to get a `ScopedEmitter` instance, and perform operation on the container. The ScopedEmitter will take a snapshot of the content and compare it with the current container upon destruction. Signals will be fired if elements have been added or removed or changed (in case of a map like container). Short example:

```c++
auto composite = sight::data::Composite::New();

{
    auto scoped_emitter = composite->scoped_emit();

    // Now modification to `composite` will be notified to whatever is connected to `composite` signals
    composite->insert({"beast", sight::data::Integer::New(666)});
    ...

    // Signals are sent when notifier is destroyed, like outside this scope...
}

```

##### Advanced usage

###### Writing specific code for specific container

Sometimes, it is useful to know if an object is a container and if yes, from which kind it is. Some template matching functions have been added to `core/tools/compare.hpp`:

```c++
#include <core/tools/compare.hpp>

template<typename T>
void my_function(const T& truc)
{
    ...
    if constexpr(core::tools::is_map_like<T>::value)
    {
        truc.insert({"maman", value});
    }
    else if constexpr(core::tools::is_container<T>::value)
    {
        truc.insert(value);
    }
    else
    {
        ....
    }
}
```

###### Generic container without inheriting from `IContainer`

If you don't want the `IContainer<XXX>` inheritance because you don't want to be a `sight::data::Object`, but still want to act like a `XXX`, you could inherit directly from `ContainerWrapper<XXX>`:

```c++
class MySimpleContainer : public ContainerWrapper<std::set<std::string>>
{
    ...
};

MySimpleContainer a;

a.insert("Le petit chaton bleu est très malade.");

...
```

### io

*Add requestSettings slot to grabber interfaces.*

*New DICOM reader implementaion.*

A new DICOM reader has been implemented to take use of the shared DICOM context, allowing to have access to all DICOM properties from the original DICOM files. The new reader is also able to read DICOM IMAGE files that were not well-supported, most notably:

* All Ultrasound Images IOD (Enhanced US Volume, Ultrasound Image, Ultrasound Multi-Frame Image)
* RGB Image with YBR color space (used in jpeg image)
* Encapsulated transfer syntax
* Unusual planar configuration (r1r2r3...g1g2g3...b1b2b3 instead of r1g1b1r2g2b2r3g3b3...)
* Monochrome 1 image

Few bugs have also been corrected, and some enhancement has been done:

* Allows selecting the series to read in case there are more than one series found
* Huge speedup: 4x faster reading
* 2x less memory used
* Fixed many bugs in the Series selection dialog:
  * crash with date/time on anonymized data (which contained unusual dates)
  * wrong data displayed
  * wrong data selected
  * added utf-8 / internationalization support (yes Ren**é** is now displayed correctly, and far east patient are no more a number of squares)
* Thread management of GUI stuff like cursors, DICOM logger dialog, etc....
* some tests were added (and also US data on sight-data)

*Update IWriter interface to automate data output.*

* Automate the output of data using Preferences and Series for the SFrameWriter, SVideoWriter and SMatrixWriter services

*Refactor io itk services and add nifti support.*

Global refactor of the itk io services and libs.

* add of the nifti support in the lib and in the modules
* refactor the lib existing read/write files to fit the global sight io naming
* refactor the services, to make a single SImageReader/SImageWriter which switch depending on the extension
* refactor the services, to have reader/writer which go together
* add unit-test
* add itk reader in sightviewer images series loader

### test

*Add a macro saver to generate GUI test skeletons.*

GUI testing is now possible, but creating GUI tests still isn't that simple, especially to get the graphic components to interact with. MacroSaver will generate a GUI test skeleton compatible with GuiTester to save that hassle. To use MacroSaver, one must call sightrun with the `--macro` flag, the generated tests will be available once the application is closed as the files "GuiTest.cpp" and "GuiTest.hpp".

*Add a GUI test library and GUI tests.*

An automated GUI test library built on top of QTest was introduced. This library can wait and get graphic components, interact with them by emulating mouse and keyboard events and check if assertions are true. In order for the graphic components to be fetched more easily, components got objectNames, so they can be easily found using `QObject::findChild()`.

Additionally, the tests will test the 3D rendering, by creating snapshots and comparing them with reference images using 6 different methods (pixel by pixel, mean square error, cosine of the matrices, histogram, Spearman's correlation,...). When a test fail, it will take a screenshot of the screen, to help in troubleshooting, especially when it runs on an non-desktop environment, such as the CI.

Last, first UI tests were introduced for SightViewer and SightCalibrator.

### ui

*Add standard embeddable video toolbar.*

*Allow to programmatically set enum values in SParameters.*

### viz

*Introduce GPU resource sharing.*

GPU resource sharing is introduced, with the first application to textures and transfer functions.
From the user point of view, we provide a new class `sight::viz::scene3d`::Texture``and we extended the existing class `sight::viz::scene3d`::TransferFunction``in order to allow GPU resource sharing. These classes only allocate Ogre resources (and thus GPU memory) when the Id of the source object changes. In addition, they "cache" the update of these resources. Thus now, when we have, for instance, 3 negato adaptors on the same image, there is only one `Ogre`::Texture``created, and only one `Ogre`::Texture``updated when the `sight`::data::Image``is modified (and similarly for the transfer functions).

*Allow applications to add Ogre plugins from CMake.*

We added the possibility to enable Ogre plugins from CMake `module_param(module_viz_scene3d ...)` calls.

## Refactor:

### build

*Use a single PCH with only external dependencies.*

To solve the issues described in #948, we now provide a single PCH file. It only contains 3rd part libraries headers. The list was extracted from the previous pchServices recursive list, by removing all sight headers. It is also built in a second version for the targets that use optimized debug builds.
During tests, we realized the build timings remain stable and even outperform sometimes the previous approach.

On Clang, we also took the opportunity to use a new feature that speedups PCH builds: http://llunak.blogspot.com/2021/04/clang-precompiled-headers-and-improving.html. The gain was measured at 10%.

### core

*Rename oldish macros.*

*Replace EConfigurationElement with boost::property_tree::ptree in all unit tests.*

*Remove visibility and transparency signals on images.*

The visibility and transparency of images are now handled in a simpler way, following the MVC pattern:
- the image is the model
- the view is the adaptor (`SNegato2D`, etc...)
- the controller is the widget (`SParameters`, `SAction`, etc...)

The view does not control the state and only applies what the controller requests.

This implied the following changes:
- Removed s_TRANSPARENCY_MODIFIED_SIG and s_VISIBILITY_MODIFIED_SIG fields from `data::image`
- Removed all associated code, including the old service `module::ui::qt::image::ImageTransparency`

*Simplify core::tools::Type and massive core::tools cleaning.*

`core`::tools::Type``was simplified for easier use. First, it was moved to the root of the library, so it is now `core::Type`. Then, it simply contains a single enum attribute that sets the supported type. All methods of the class use an internal map to answer to the capabilities of the type (size, signed, name, etc...) instead of storing them in each type instance.

Some extra cleaning was also done. `core`::tools::Type``is a very old piece of code, previous to C++11 and was linked to some pieces of dead code. Many features it used to bring are now done easily with post C++11 code. The dispatcher, for instance, used exclusively for ITK filters, relied on `type_info` just because of the use of a deprecated ITK function to get the image type. With the new function, this is no longer necessary, so a large amount of code dealing with `type_info` was removed.

*No longer use xml profile in utest, remove initialize state in IPlugin.*

### ui

*Deprecated SSlotCaller in favor of SAction.*

*Use new signals and slots of IAction everywhere.*

*Harmonize IAction signals and slots.*

*Remove deprecated services, functions and config.*

`ui`::viz::SAddPoint``is removed in favor of `module::geometry::SManagePointList`, `IAction::getActiveStateValue()` is removed in favor of `IAction`::inverted``and `ISlideViewBuilder` `align` and `size` configuration attributes are removed in favor of `halign/valign` and `width/height`.

### viz

*Remove transfer function merge copy in the data.*

We faced multiple floating precisions issues when merging the pieces of a transfer function. To avoid that, we removed the pre-merge phase of the pieces and we actually merge directly when sampling.

*Simplify transfer function management.*

This is a major rework of the transfer function data. Now the transfer function is a real piecewise function. Each piece stored in the object can be individually edited and thus is serialized. Pieces can be accessed and edited independently thanks to the `pieces()` function that returns a vector of `TransferFunctionData`. When the pieces are modified, the function `mergePieces()` must be called to mix the pieces together.

This implies that the duplicated `sight`::module::ui::qt::SMultipleTF``and `sight::module::viz::scene2d`::SMultipleTF``are removed in favor of `sight`::module::ui::qt::STransferFunction``and `sight::module::viz::scene2d::STransferFunction`.

On top of that, the former TF "pool" is no longer exposed anywhere in the services interfaces, thus from the XML. `sight`::module::ui::qt::STransferFunction``handles this "pool", renamed as "presets", internally.

Other changes were brought:
- `sight::data::TransferFunction::mergePieces()` implies the removal of `sight`::module::data::SMergeTF``service. It also brings a functional change, the pieces of the transfer function is performed with an average of colors instead of a strict additive blending.
- It is no longer possible to move a transfer function piece outside the range of the viewport (bug fix request in the comments of this ticket).
- Adjusting the window in the negatoscope is now reflected properly in the transfer function editor (partially fixes https://git.ircad.fr/sight/sight/-/issues/847)
- Moved and renamed `TransferFunctionManager` from `scene3d` to `scene2D::TransferFunctionWindow`.

*Include TransferFunctionManager from TransferFunctionManagerWindow.*

*Merge SHistogram, SHistogramCursor and SHistogramValue together.*

Since we removed SCurveHistogram, there was no good reason to keep those three adaptors separately.

Doing that, we improved the computation speed of the image histogram by a factor of 10. Computing the histogram is now so fast that it is useless to have a dedicated data for it. We removed all the ecosystem around it and the only service that uses it so far, the SHistogram adaptor, computes the histogram dynamically. On top of that, this allows to change the bins width dynamically which is very handy.

We also added an axis for the image histogram that can scale with the graph. This required a refactor of the usage of the viewport in the 2D scene. Now the scene no longer holds a viewport, but can only update its viewing rect through a dedicated function available to the adaptors. The adaptors that do use the viewport now all explicitly declare it, with the correct access type (in or inout).  Last, we scale the transfer function viewport range automatically from image min and max intensities.

Many warnings were fixed along, notably because of the coexistence of float and double variables. The floating point precision was set to double everywhere to comply with Qt API.

*Remove irrelevant SCurvedHistogram service, use SHistogram instead.*


# sight 21.1.0

## Refactor:

### build

*Apply cmake-linter rules.*

### core

*Remove last bits of data reflection based on camp.*

Data reflection based on the camp library was definitely removed from _Sight_. It consisted in removing the files used to declare data reflection and all files that used camp.

Most usages of data reflection was removed in previous merge requests, like serialisation. However, some last services still used data reflection and were modified accordingly:

#### module::ui::qt::model::SModelSeriesList

This service displays reconstructions in a tree widget. It used data reflection to display any model series attribute. However practically, it was only used to display the organ names, structures types and volumes. Those three possibilities were hard-coded instead.

#### module::ui::qt::series::SViewer

`SViewer` shows a preview of data. Normally, it takes input data, but a similar mechanism was in place to allow data extraction like in activities. This was a bit over-designed and practically only used to extract images from image series. The image preview activity was thus modified to extract the image with the new `SGetImage` service.

#### module::data::SCopy

The most annoying service was `SCopy`. Like `SExtractObj`, it was capable of using a path camp to copy sub-objects. It was used abusively in SightViewer, probably to avoid deferred objects. This was bad since model series and images are present multiple times in memory.

Nevertheless, this could not be changed easily. Two new services needed to be written.

- `module`::data::SGetVector`:`very similar to `SGetSeries`, it allows to extract elements of a `sight::data::Vector`.
- `module`::ui::qt::series::SSelectDialog`:`the intent of this service is to pop-up a dialog to let the user pick a series. Indeed in SightViewer, we may load several series, but only the first one is displayed. So for instance, if we read a CT-scan with 3 different blood times, we can only display the first one. This service also resolves the problem of the type of output. It allows to output the result to different objects according to their type: image or model. With the constraint of time for the incoming release, this service is proposed in the state of a draft. It does not prompt the dialog yet, instead, it takes the first element. This allows at least to keep the same level of functionality in SightViewer. It will be finalized in Sight 22.0

*Remove oldish SPushObject and SPushField, rename SObjFromSlot.*

We are very close to finish the rework we started years ago about clarifying object selection services.
- `SPushObject` and `SPushField` were outdated, and can be replaced easily by the `SManage` service. So we simply removed them.
- `SObjFromSlot` does not have a meaningful name. It is renamed as `SSelectObject` and no longer inherits from the also meaningless `IUpdaterSrv` interface, but rather from `IController`.

*Simplify workers management.*

Firstly, this was an intent to add a function to unregister workers, but in the end, this is a complete rework simplification of the `ActiveWorkers` registry.

Here is a summary of the changes.

No longer exposes the ActiveWorkers registry publicly. The only interest would be to have multiple registries... Mmm really? I don't think that's useful. So then if there is only one registry, as a user, I don't care that this is a singleton, I even don't care there is a registry at all. I just want to add and remove workers, right?

Now, people are supposed to add and remove the workers themselves. There is no more "global" cleaning method (formerly `cleanRegistry()` ). Symmetry for the win.

The initialisation phase has changed, which is somehow a breaking and important change. **So please read the following carefully**.

Formerly, no default worker is created by default. It was created either by calling `initRegistry()` (instantiating a `workerAsio` or by calling `setDefaultWorker` (in
concrete use cases, either a `workerAsio` or a `workerQt`). In most apps (all Qt apps of course), services, and unit tests, a worker was created. But in rare cases, no worker was set, which implies async functions such as `IService::start()`, `IService::stop()`, etc... were in fact executed synchronously.

With this MR, a `workerAsio` is created at the start and set as the default worker. If someones (actually only Qt Apps) want to set a different worker (the `WorkerQt`), it can call `setDefaultWorker()` and the default worker will thus be changed. The initial worker will be removed, but the function will raise an exception to avoid early slots registration with this worker. In clear, `setDefaultWorker()` is intended to be called at the very beginning of an application and only once. Even clearer, this is going to be called only once by the main Qt and Qml plugins. So actually, no one bothers, whereas before, everyone writing a unit-test or an executable app would have to think about it and call it.

Last a `resetDefaultWorker()` had to be implemented, just because QApp doesn't like to be destroyed during static variables cleaning, after the main exits. So this `resetDefaultWorker()` is actually called twice symmetrically with `setDefaultWorker()` in the main Qt and Qml plugins.

*Merge fieldHelper into helper/MedicalImage.*

* all functions are now static and class is replaced by namespace MedicalImage
* unused functions was removed
* unit test of MedicalImage was improved to test all helpers methods
* Update services to use only integer values instead of  pointers for slice indexes

*Rename all getNumberOf*() functions to num*().*

*Clean old deprecated array API.*

*Clean old deprecated image API.*

*Modernize mesh API.*

### io

*Remove module::io::atoms and replace it by module::io::session counterpart.*

All of module`::io::atoms`has been removed and replaced by module`::io::session`counterpart. `Patch` and `Filter` features of atoms have also been removed. io`::session`has been upgraded to support simple JSON on filesystem serialization.

Additionally, a new preference system, using Boost property tree, has been implemented:

# Preference
The class is thread safe and use RAII mechanism to load and store data from and to the preference file. In a service, the basic usage would be:
``` c++
    try
    {
        // Load
        Preferences preferences;
        std`::string`filename = preferences.get("debug.filename");
        int level = preferences.get<int>("debug.level");
        ...
        // Save
        Preferences preferences;
        preferences.put("debug.filename", "log.txt");
        preferences.put("debug.level", 2);
    }
    catch(const ui::base::PreferencesDisabled&)
    {
        // Nothing to do..
    }

```

Which will be translated into:
``` json
    debug:
        filename: "log.txt"
        level: 2
```

## Configuration
The configuration is done by static functions:

- set_enabled: Enable or disable preference management as a whoole. All functions, including constructor will throw `PreferencesDisabled` exception if used while "disabled"
- set_password: Set an harcoded password to be used. It enables defacto the encryption
- set_password_policy: Defines how and when a password is asked. @see sight`::core::crypto::PasswordKeeper::PasswordPolicy`for possible values.
  > `NEVER` will never display any message box, but if a password has been set, the resulting preference file will still be encrypted. An `BadPassword` exception will be thrown instead of diplaying a message box, asking to renter the password.
- set_encryption_policy: Define when the preferences file is encrypted: @see sight`::core::crypto::PasswordKeeper::EncryptionPolicy`for possible values.
  > `FORCE` will encrypt the file, even if no password is given. In this case a pseudo random password is used, which can be guessed if someone has access to the code. Another option is to use an harcoded password AND EncryptionPolicy::SALTED
- Module Configuration: All the above can be configured through the module ui_base parameters (@see sight::module::ui::base::Plugin)
  The preferences are enabled by default, without encryption. An example of configuration would be:

  ``` cmake
    moduleParam(
        module_ui_base
        PARAM_LIST
            preferences_enabled
            preferences_password_policy
            preferences_encryption_policy
            preferences_password
        PARAM_VALUES
            true
            once
            salted
            a_bad_hardcoded_password
    )
  ```

# Simple JSON serialization on filesystem

It is now possible to use `SReader` and `SWriter` to serialize a data object into a single JSON file (until the serializer class also store additional files like for Mesh or Image object).

## SReader
This is achieved withe the `archive` configuration node and its `format` attribute:
- `filesystem`: Reads files from the filesystem.
- `archive`: Reads files from an session archive.
- `default`: uses the builtin default behavior which is `archive`

### XML sample configuration:

```xml
<service type="sight::module::io::session::SReader">
    <inout key="data" uid="..." />
    <dialog extension=".sample" description="Sample Sight session file" policy="always"/>
    <password policy="once, encryption=salted"/>
    <archive format="filesystem"/>
</service>
```

## SWriter
This is achieved withe the `archive` configuration node and its `format` attribute:
- `filesystem`: Use the filesystem to store files.
- `compatible`: Store files in a ZIP archive, with old deflate algorithm
- `optimized`: Store files in a ZIP archive, with zstd algorithm
- `default`: uses the builtin default behavior which is "optimized"

### XML sample configuration:

```xml
<service type="sight::module::io::session::SWriter">
    <in key="data" uid="..." />
    <dialog extension=".sample" description="Sample Sight session file" policy="once"/>
    <password policy="once", encryption=salted/>
    <archive format="optimized"/>
</service>
```

### ui

*Remove obsolete SSlider, superseded by SParameters.*

## Bug fixes:

### build

*Fix application package.*

* CMake configuration has been modified to ensure that sightlog binaries are always shipped with Sight, even when building a standalone application.
* Ogre plugins install was not done because of a regression introduced when fixing Qml plugins install.
* use the .bat instead of .exe to launch app in .lnk

*Add tighter security in get_default_password access.*

Add if def security around the  get_default_password
Export the SIGHT_DEFAULT_PASSWORD var from sight

*Improve application version in package name.*

*Allow to link with io_igtl from a child project.*

*Prevent whole rebuild each time with CMake >= 3.20.*

This was caused by the new CMake policy CMP0116 about the usage of the DEPFILE argument of add_custom_command. Since we did not succeed to use the NEW behavior yet, we switch back, temporarily, to the OLD behavior.

*Qml applications on Windows.*

Several fixes were brought to fix sight applications using qml on Windows:
- determine qml plugin path at runtime instead of build time. We use the same strategy for Qt plugins.
- install qml plugins correctly when building an installer
- port deprecated QtQuick1 stuff to QtQuick2

*Enable warning as errors on sight_core.*

Warnings as errors were enabled but sight_core, but this library first builds an object library. The flags were not passed to it. This has been fixed and some warnings were fixed as well.

### ci

*Fix sheldon license date check.*

- only build on Ubuntu 21.10
- fix code coverage html/xml report generation

### core

*Replace assertion when slice index field doesn't exists.*

`getSliceIndex` returns a std`::optional`that needs to be tested to know if field is present or not.

*Object registration is overriden by XML configuration.*

Previously, there was no data prerequisite in a service. One can declare any data in the XML configuration of a service, and use it at runtime with getters. So basically you could configure a service with any data. It was even possible to change the access type dynamically.

Sight 21.0 enforced a prior declaration of data with `sight::data::ptr`. They allow specifying the key name, access type, as well as the auto-connect and the optional attributes. Normally, only declared data this way should be used in services.

However, for back compatibility purposes (especially for the very specific `SConfigController` and `SConfigLauncher` services), it is still possible to register data just from the XML configuration. We discovered that this is actually buggy and the XML configuration overrides the `sight`::data::ptr``declaration. So it is for instance possible to declare input data and finally configure it as an inout. We also realized that the default auto-connect and optional attributes values were not respected with an XML configuration. The actual default values were those of the XML parser.

This merge request first fixes the override of the data declaration with the XML configuration. It also fixes the XML parser to use the default auto-connect and optional attributes values declared in each service type. This required some extra work on group objects.

Indeed, we used to have a minimum and a maximum number of objects. These were actually not really used, and developers misused this feature most of the time. They configured `sight`::data::ptr_vector``with the same signature as `sight`::data::ptr``which led to implicitly converting a true boolean meaning "optional data", in an integer meaning "at least one object". Thus the intent of the developer was actually the opposite of the actual result. The maximum number of objects was never used in real cases and was in fact introduced to solve an internal issue with non-XML apps. This was not a real need for the developer.

In the end, `sight`::data::ptr_vector``now shares the same signature than `sight::data::ptr`. It is still possible to configure in the XML the auto-connect and optional on each member of the group, but this time the default value will be the one declared in the `sight`::data::ptr_vector``declaration.

Last, fixing all of this highlighted some misconfiguration in a few services. These were corrected accordingly.

*Update old "3DSimpleConfig2" by "3DSimpleConfig".*

* ExDicomSegmentation & ExDump impacted

*Remove dead lock and init mesh celltype properly.*

*Attributes arrays not reallocatted on pushPoint and pushCell.*

*Load of dynamic libraries when only versioned files are present.*

On a regular Linux system, libraries are installed with the version in the extension, like libsight_core.so.21.0. The symlink without the version in the extension is only present if the development package is installed. Thus we can not rely on this symlink to load the library like we did since Sight 21.0.
This commit switches back to the old way, looking for a file name that matches the library name. A cache has been added to avoid the cost of iterating over all library files each time.

*Correct needle calibration.*

Correct point pivot based nbeedle calibration algorithm
fix header to allow include in external project
misc: update coding style

*Optimize data::iterator::ImageIterator.*

*Check if service UID is used in signal/slot before collecting UID for parameter replace.*

* parse also menuItem sid="..." attribute before collecting UID for replacements

*Remove useless return after throwing exception.*

*Use sight_core module path to determine the runtime dir.*

*Speed-up image copy in SFrameMatrixSynchronizer.*

The image copy using the image iterator is slow as hell in debug, making some applications unusable. This was replaced by a simple std::memcpy.

*Add sight namespace to profiling macros.*

### doc

*Update README to match Sight 21.0 changes.*

### geometry

*Workarround to get rid of bufferObject assert still locked.*

Force destruction of original buffer object of output image after the unlock of input & output image, doing this avoid the assert of bufferObject still locked.

*Don't shift 2d points on when computing RMSE on calibration.*

* Add option in SSolvePnP to shift points or not (default off)
* Replace quad mesh plane.vtk by triangular mesh plane.vtp

### io

*Replace deprecated OFIterator in favor of OFListIterator.*

This deprecated container is removed in recent dcmtk versions.

*Remove deprecated VTK types.*

These types were deprecated and unused a long time ago. They are removed in latest VTK releases.

*Extension() getter function in reader/writer made const.*

### test

*Fix ui_qt_test not executing offscreen.*

### ui

*Reduce lock scope to avoid a deadlock.*

*Remove all deprecated QSignalMapper.*

### viz

*Do not snap landmark onto slice plane in 2D negatoscopes.*

Landmarks can now be moved properly as before with drag & drop. An extra correction was made to not snap the landmark onto the image plane in the 2D negatoscope views. This led to weird behavior when you adjust the position of the landmarks in two different orthogonal views. The landmark always snaps to the image plane, thus giving the effect of "jumping" in the other view, and preventing from adjusting the position finely.

*Replace getTransformNode by a pure getter.*

`getTransformNode` was ambiguous since it return the transform node if existing or creates it if it doesn't.

This behaviour is kept but function was rennamed into `getOrCreateTransformNode`.
A pure getter was also created `getTransformNode()` to return the transform node pointer or nullptr if it doesn't exists.

Add small unit test to test ITransformable functions

All adaptors was updated to call either `getOrCreateTransformNode` or `getTransformNode` regarding the case (creation of the node of getting the pointer). This fixes the crash due to Ogre exception "Node already exists" when trying to get a deleted node.

*Move orientation marker when splitting views.*

The orientation marker in the 3D view of SightViewer remained on the bottom right corner of the view when splitting views. Now it keeps attached to the 3D view.

*Remove extra space in R2VB program preprocessor definitions.*

Remove extra space in R2VB program preprocessor definitions. Sadly, this prevented the preprocessor defines of the R2VB geometry programs to be parsed properly, thus causing rendering issues.

*Crash in Ogre resources loading.*

The list containing the Ogre resources paths was not allocated properly.

*Set texture unit state after changing a material template.*

## Enhancement:

### build

*Update vcpkg pakage hashes to match last build.*

*Add some flags for better performance and debugging.*

*Default to warnings as errors.*

*Forbid link with a module.*

### ci

*Make the code coverage percentage more precise.*

*Use shared scripts to deploy, add SightCalibrator deployment.*

*Remove Ubuntu 20.10 Release target, replace it with Ubuntu 21.04 Debug.*

### core

*Remove redundant internal lock in the Timeline object.*

We removed internal locks in timeline functions. Now, we assume that all timeline objects are accessed by a lock() in the caller.

*Return const pointer when using getters on const data.*

Many getters in data returns shared_pointer, even if the data was const the returned pointer wasn't and underlying data was still modifiable.
This has been changed to provide const getters that returns const pointer, and non const getter to returns simple shared_pointer.
Doing so leads to some changes in part of the code that uses const data (usualy input data)

* Use of const_cast as been restricted to 3 cases:
  * When moving buffer to ITK or VTK
  * SReconstruction when moving the pointer to data`::Material`to an internal SMesh adaptor.
* SGetImage, since ImageSeries input is const, when doing a setOutput (non const) with series->getImage, a const_cast is needed to discard const qualifier.

* Changing from inout to input access:
  * SMesh: mesh is now a const input.

* Use const shared_pointer when possible

*Simplify image fields initialisation, remove unused fields.*

- `MedicalImageSrv` was simply removed.
- Instead, `data::helper::MedicalImage::check*()` functions are called when reading or creating an image. An image is "always" a medical image, but this does not hurt to have extra fields for non-medical images.
- `checkLandmarks()` was removed, the point list is now always present.
- The following used fields were removed: slices count, comment, label.

*Make dispalyCalibrationInfo more scalable.*

*Remove obsolete atoms lib.*

### io

*Move csv reader from io::dicom to io::base.*

### ui

*Enable HiDPI scaling on Qt apps.*

*Move notification stack up/down when oldest notification is closed.*

*Add option to "read more..." when notification message exceeds maximum allowed characters.*

*Upgrade default confirmation message on action.*

*Add fullpath option for camera selection in Scamera.*

style: apply sheldon

style: apply review suggestion
style: correct variable name

style: apply coding style

style: apply coding style

style: correct comment

### viz

*Make viz::scene3d::vr great again.*

#### Description

`viz::scene3D`::vr``has many feature that either don't work as intended or do not work at all.
This is mainly due to bad service-library interactions and subtle bugs / flaws in some methods.

Because of that, using this namespace as a base for subclasses requires substantial efforts to workaround those bugs / design problems.

#### Changes

##### Library

**Volume renderer:**

- `IVolumeRenderer`::update``has been introduced to solve the problem of the initial issue (calls to virtual function from constructor). This member is `public` and `virtual`, and `override`n in `RayTracingVolumeRenderer`.
- Shader source file is now specified upon construction.
- As `RayTracingVolumeRenderer`::initEntryPoints``must only be called once, there is now a check it has not been called before.
- New `struct`s wrapping attributes.
- `createRayTracingMaterial` is now `updateRayTracingMaterial`, and does not take any parameter.
- Some functions were renamed to something explaining what they actually do.
- Some integer values are now `unsigned` in order to ease some asserts.

**Volume rendering techniques:**

- RAM leaks and follow the changes in the volume renderer (types).

**Data management:**

- Before this MR, the image resources was managed by **both** the service and the library. This is not the case anymore. The service now knows *almost* nothing about the data. Consequently, what was `weak_ptr` previously in the library are now `shared_ptr` or even `unique_ptr`.
- The following are now set upon construction:
    - Raytracing fragment shader source file,
    - Ambient occlusion parameters,
    - Preintegration parameters,
    - Shadows parameters,
    - SAT parameters,
- There used to be duplicated attributes in the service and the library, which represented the same flags but where not updated simultaneously. This is not the case anymore. Actually, this was the main cause of - #756, #791 and #792).

##### Module

- Reported changes in the library (argument type changes, etc.)
- New `struct`s wrapping attributes (config, etc.).

*Improve flat shading by using fragment shader computed normals.*

Flat shading may often be broken if point normals are shared between faces. To overcome this, we chose to compute normals in the fragment shader using eye position derivatives. Doing so, the outdated and never used Gouraud shading mode was removed. This removes a shader combination, which makes maintenance easier.

## New features:

### build

*Forbid module linking, even for tests.*

 - Add a special verification step to forbid linking with a module.
 - Rename `fw_cppunit_test` to `fw_test`.

*Allow privilege escalation on Windows.*

*Allow to override the downloading of dependencies.*

*Debian package build prevents networking access.*

### ci

*Enable unit-tests report.*

To output unit-tests results in JUnit format will allow us to see the results
of the unit-tests in the GitLab web interface.

*Attach gdb to running test to have a core file.*

*Add code coverage to unit tests and Ubuntu 21.10 support in CI.*

### core

*Remove sub-object extraction from activities.*

Now, when defining a requirement in an activity, there is no need to define a corresponding `AppConfig` parameter to "replace" it with the corresponding key. In short, the same key is used for the requirement and the parameter, and there is no need to give a complicated object "path":

- Before:

```xml
    <!-- Activity used to export a model series. -->
<extension implements="sight::activity::extension::Activity">
...
    <requirements>
        <requirement name="modelSeries" type="sight::data::ModelSeries" minOccurs="1" maxOccurs="1" />
    </requirements>
    <appConfig id="xxx">
        <parameters>
            <parameter replace="model" by="@values.modelSeries" />
        </parameters>
    </appConfig
</extension>
```

- After:

```xml
<extension implements="sight::activity::extension::Activity">
...
    <requirements>
        <requirement name="modelSeries" type="sight::data::ModelSeries" minOccurs="1" maxOccurs="1" />
    </requirements>
    <appConfig id="xxx" />
</extension>
```

##### Limitation

However, this simplification implies some limitations:

1. The key must match for both requirement and `AppConfig` parameter. There is no way to "replace" them on the fly like before. We really believe this is fine as the ability to have a different name only complicate both xml (you need to specify the "mapping") and C++ code, with no real added value.
2. As we can only access to the "required" object, we cannot "walk" through the object hierarchy and extract sub-objects. For example, it is no more possible to have an `Image` as `AppConfig` parameter which is extracted from a required `ImageSeries`. This can be easily adapted with the use of the new extraction services (`SGetImage`, `SGetMesh`, ...) in `AppConfig` code.

*New service sight::module::data::SGetCamera.*

This new service extracts cameras and extrinsic matrices from a camera series.

*New service sight::module::data::SGetSeries.*

This service can be used to extract one or several series from a seriesDB.
The XML configuration is fairly simple:

```xml
<service uid="..." type="sight::module::data::SGetSeries" >
    <in key="seriesDB" uid="..."/>
    <out group="series" >
        <key index="4" uid="..." />
        <key index="2" uid="..." />
    </out>
</service>
```

where:
- seriesDB is a `sight::data::SeriesDB`
- series group data are `sight::data::Series`

*Remove core::data::ObjectLock.*

- remove core`::data::ObjectLock`call and use straight locked_ptr in place.
- rename all "dump" lock() (lock(), bufferLock(), lock_buffer(), ...) to dump_lock() to avoid confusion with "mutex" locks.
- factorize dump_lock() call in IBuffered.hpp.
- remove one deprecated TimeLine construct.

*Implement SGetImage Service.*

*Add == operators for all data object.*

#### Description
Now a data object of the same type can be compared using == like:

```c++
std::shared_ptr<sight::data::Image> image1 = ...;
std::shared_ptr<sight::data::Image> image2 = ...;

if(*image1 == *image2)
{
...
}
```

Floating point values are compared using a scaled epsilon (which gives result similar to [ULP](https://en.wikipedia.org/wiki/Unit_in_the_last_place) comparison). NaN, infinite values are also taken into account.

Templated helpers `core::tools::is_equal()` have been added to ease comparison with containers, pointers and floating point values.

*Encrypted log.*

#### Description

Our logging system is based on [boost::log](https://www.boost.org/doc/libs/1_77_0/libs/log/doc/html/index.html), which works fine, but doesn't provide a way to secure the content of the log file. We choose to use [minizip-ng](https://github.com/zlib-ng/minizip-ng) with built-in AES encryption as the backend to store the log and continue to use boost log as the frontend. We have also improved password management, to allow encryption to be used even without providing a password. The minizip backend runs in a separate and detached child process, so it will always produce a valid zip archive even if the parent process crashes.

#### Additions / Changes

##### CMake
- Enable log encryption support with new CMake build options: `SIGHT_ENABLE_ENCRYPTED_LOG` (OFF by default)
- Allow to specify a default hardcoded (but obfuscated) password at compile time, set by `SIGHT_DEFAULT_PASSWORD` CMake definition.

##### Password management
Password management has been reworked a bit to factorize and unify behaviors.

###### `SIGHT_DEFAULT_PASSWORD`
It will be used when required (log encryption, preferences, session, ..) by default, until overwritten by user when an input dialog asking the user for a password is shown. It allows to use encryption, without having to ask user for a password. BTW, this is **less** secure, as the password is still hardcoded, even if obfuscated in the binary.

###### User password
An input dialog asking for a password will be shown when:
- the log is encrypted and no `SIGHT_DEFAULT_PASSWORD` is set **or** if `sightrun` is launched with `--ask-password` argument.
- for preferences, if the appropriate `password` and `encryption` `policy` is set in module `ui_base` configuration (take a look at `libs/ui/base/Preferences.hpp`).
- for session files, if the appropriate `password` and `encryption` `policy` is set in SReader and SWriter service configuration (take a look at `modules/io/session/SReader.hpp` and `modules/io/session/SWriter.hpp`).

In all case, the entered password will be kept (obfuscated) in memory, allowing to only ask the password once, until, of course, it is explicitly configured to not do so.

### Sightlog logger (utils/sightlog/src/sightlog.cpp)
It is a simple standalone application that read stuff from standard input and write them in a [minizip-ng](https://github.com/zlib-ng/minizip-ng) archive or a raw log file. Alternatively, it can also decrypt the real log from a log archive, which can be useful if a human need to read it back. The logger is started as a standalone detached child process by `Sight` which ensure the integrity of the log archive, even if `Sight` crashes. If `SIGHT_DEFAULT_PASSWORD` password has been used, it is also embedded inside `sightlog` binary, and it should not be necessary to use `-p` option.

###### sightlog usage

```
Sightlog logger options:
  -h [ --help ]          Display this help message.
  -v [ --version ]       Display the version of the program.
  -i [ --input ] arg     Log archive to extract.
  -p [ --password ] arg  Password to use for encryption and decryption.
  -a [ --ask-password ]  Show a popup to enter the password.
  -d [ --directory ] arg Output directory when extracting a log archive.
```

For example to extract the log file from archive `/home/bozo/sight/bin/sight.log.0.zip` protected with password `w34r3th3B3st` in directory /home/bozo/logs and  we can use:

```bash
./sightlog -i /home/bozo/sight/bin/sight.log.0.zip -d /home/bozo/logs -p "w34r3th3B3st"
```

##### String "obfuscator" (libs/core/core/crypto/obfuscated_string.hpp)
Allows to define a string literal that will not appears in "clear" text in the final binary. Useful for defining an "hardcoded" password, without being to simple to find and read. This is of course not as secure as a real password entered by a real user.

##### SpyLogger class
- Doxygen comments..
- New method `start_logger()` and `start_encrypted_logger` which starts `sightlog` child process.
- New unit tests `EncryptedLogTest`
- The file paths in the log file are now trimmed again to not show the full path, but the minimal one to be able to locate a source file: (/home/bozo_the_clown/work/src/sight45/.../.../libs/core/core/spyLog.hpp -> libs/core/core/spyLog.hpp)

*Add an image parser to allow basic initialisation in xml.*

### io

*Allow custom serializers for any object, even when defined outside sight.*

### ui

*Delete sequencer data when going backward.*

Add an option to remove all the data generated when going backward with the sequencer.

### viz

*Update slice indexes when a landmarks is double clicked.*

Update SNegato3d & SNegato2d to move slices to the selected landmark.

* SLandmarks send world position when double clicked (new signal)
* SNegato3d, SNegato2d listens through a slot if slices indexes needs to be updated regarding a world position (new slot).
* Conversion between world coordinates and slice index as been implemented in scene3d`::Utils`and a unit test has been added.

Update adaptor`::SLandmarks`to handle double clicks on landmarks and also send the current world coordinates of the point

*Use the mouse wheel to scroll through slices in the negatoscope.*

Before the mouse wheel event was used to zoom in/out the image, it now allows to scroll the slices, like in other software. The Shift key speeds up scrolling.

You can still zoom in/out the image with the mouse wheel, but with the Ctrl key pressed.

*Add optional name to SAxis adaptor.*


# sight 21.0.0

## Bug fixes:

### build

*Use project_name in variable exported by sight_generate_components_list.*

Do not export SIGHT_COMPONENTS in sight_generate_components_list cmake function, use instead COMPONENTS. This avoids variable collision when using sight in subprojects, since SIGHT_COMPONENTS is exported by sightConfig.cmake.

*Sight-project installation error due to sight version.*

To fix the main problem, SOVERSION is no longer defined for executable targets. That simply prevents from creating these useless versioned binaries.

On top of that, several other fixes were brought:

* the version attribute of the `project()` CMake command is used instead of redefining a custom `SIGHT_VERSION` variable,
* code cleaning was done around this, notably to rename `FWPROJECT_NAME` into `SIGHT_TARGET`, which is more correct with the usage of this variable,
* dependencies computing now browses `OBJECT` libraries targets, like `io_session_obj` (fixes `Tuto01Basic` packaging for instance where `sight_io_vtk` library was missing),
* dependencies computing now handles cross-repositories dependencies (fixes some child projects packaging),
* components ordering was included in a higher-level function `sight_generate_component_list()` for a simpler use outside *Sight*.

*Tests are relatives to last cmake project call.*

* Use `${CMAKE_BINARY_DIR}` instead of `${PROJECT_BINARY_DIR}` to force executable to be produced in ./bin folder.
* Also testing if safe-svfb-run isn't already copied in ./bin
* early return in CppUnitConfig.cmake if CppUnit is already FOUND.

*Configure child projects fails.*

To fix the problem, we no longer export PCH targets and we no longer export modules .lib.
On top of the initial problem, we also always build `utest`, `utestData`, and `module_utest` instead of only building them when `SIGHT_BUILD_TESTS` is `ON`. Child projects may need them even if unit tests were not built in Sight.

*Generate sight component list.*

CMake components in SIGHT_COMPONENTS variable are now ordered automatically according to their dependencies. It will ease the burden to manually maintain the list.

*Explicit relative path in installted imported target library symlinks.*

When we install packages in child repositories, we copy the necessary dependencies from Sight, for instance, the libraries. On Linux, we also need to recreate the library symlinks. This was done with absolute paths, which makes packages not relocatable. This fix just creates relative symlinks instead.

*Fix package generation.*

This brings back the package generation with CPack. Both Linux and Windows are functional. Sight can be packaged as a whole (similar to the former "SDK mode") in `tar.zst` (Linux) or in `.zip` (Windows). Any application or executable target can be packaged in `tar.zst` (Linux) or in `.exe` installer (Windows).

The CI has been updated to always build the Sight and SightViewer packages on both platforms. However, the deployment on [Owncloud](https://owncloud.ircad.fr/index.php/apps/files/?dir=/IRCAD%20-%20Open/Binaries&fileid=894807) is only done on dev and master branch, or on any branch manually.
On the dev branch, the package version number is replaced by `-latest`, so that it corresponds to a "latest" build. This prevents us from having to clean our archive folder too frequently since the packages will be erased at each upload.

*Readd missing  component io_session.*

*Add missing component for sight-dependent project configurations.*

*Makes the flag WARNING_AS_ERRORS effective.*

*Add SMatricesReader export in the plugin.xml.*

*Export Qt find_package in ui_qt.*

Moves `find_package(Qt5 ...)` to Dependencies.cmake to be exported when using imported target `sight::ui_qt`

*Geometry_eigen export.*

Export also the `find_package(Eigen3 QUIET REQUIRED)` for the target geometry_eigen.

*Move cppunit_main in cmake/build folder.*

Fix the build of unit test on external projects.

* `cppunit_main.cpp` has been moved from `cmake/cppunit` to `cmake/build` folder.
* `FindOpenNI2.cmake `has been removed
* `fw*.cmake` files has been removed, contents was added in `cmake/build/macros.cmake` file in order to be retrieved from outside.

*Install executable shell scripts.*

Install missing templates files for executable target

### ci

*Small typo in SightViewer package name.*

*Use sed for regex replacement of dev packages.*

*Launch unit tests properly on Linux.*

The code with `flock` was wrong, and the test was not executed. The initial code was restored, which should be safe.

Also, there was another specific bug with `viz_scene3d` test. It crashed after destroying the first `Ogre::Root`. Indeed we chose to create and destroy it after each test. This problem is thus independent of the display number of `xvfb-run` since it does succeed to create an OpenGL context once but somehow fails to create a second. We assumed `xvfb-run` might be buggy regarding this initialization code.

As a workaround, we create and destroy the `Ogre`::Root``only once thanks to a new module `sight::module::viz::scene3d::test`.

Last, several tests in serviceTest were fixed.

### core

*Broken library path on external projects when using '.local' in installation paths.*

The deduction of the library path failed when share was already present in the main module path. The problem already occurred with module paths themselves, so the regex is now shared between these two places.

*Tuto01DataServiceBasicCpp launch.*

The source image loaded at start in Tuto01DataServiceBasicCpp was changed with the one used in Tuto02DataServiceBasic. This fixes the launch of this tutorial.

*Add case to replace uids for slide views.*

When launching a config, we substitute all `by` attributes with uids. We missed a case to handle slide views when attaching a widget.

*Add an extra LD_LIBRARY_PATH for intermediate sight projects.*

This fixes the inclusion of more than 2 sight projects.

*Fix XML configuration parsing (#3).*

Fix the parsing of objects containing sub-object (like Composite) or values (like String).
Also fix the parsing of service with an external configuration ("config" tag in the XML service definition).

*Make material resource file handling project independent.*

Make `resources.cfg` path treatment independent of the working dir.
Indeed, the present behavior uses the working dir for the absolute path generation. However, as this is done inside sight code, the prefix corresponds to the sight install path, and not the loading module-specific path.
As a result, files that are not installed in the sight install dir can not be loaded.

It is safer to rely on the module name, and get its specific path.

*ExActivities fails to launch.*

This fixes the parsing of a service configuration when a config extension was used (`config=` attribute).

*Harmonize autoConnect booleans config parsing.*

This merge requests changes all:
- `autoConnect="yes|no"` into `autoConnect="true|false"`
- `optional="yes|no"` into `optional="true|false"`

*FwServicesTest randomly fails.*

The last failing test was keyGroupTest. The problem was actually quite simple, the autoconnection with a swapped object is done after the swap. Thus, we have to wait for the object to finish the swap sequence before sending a modified signal from the data. Before we waited for the object to be present in the object map, but this is not sufficient since this only tells the object is registered, and the registration occurs before the swap.

*Restore MSVC build.*

Windows support is back! Third-part libraries are now built with vcpkg instead of Conan. We found out that vcpkg packages are much more stable and most of all, coherency is ensured between packages.

Few fixes were brought to support the newest version of these libraries. Indeed they are often more recent than Ubuntu packages.

Doing this, the GitLab CI/CD scripts were updated to use Powershell instead of cmd, as recommended by GitLab.

*Remove TransferFunctionManagerWindow include from sightViewer plugin.xml.*

*Several runtime errors after sight 21.0 upgrade.*

### doc

*Rewriting doc of CardinalLayoutManagerBase.*

### io

*DicomXplorer crashes when display mesh preview.*

This MR fixes a crash upon selecting mesh in `dicomXplorer`.
The problem was simply that old configurations used in this software were deleted.

**It does not display the mesh in an activity.** It simply fixes the crash and re-enables the preview.

*VTK readers doesn't handle color array properly.*

When converting from vtk to sight mesh format, we check if color array named "Colors" exists in polydata in order to copy vertex or cells colors. When using PLY reader from VTK (maybe other format too) color array is named "RGB" or "RGBA".

We also added a workaround to fix color rendering of first cell on Ogre 3d. Otherwise, mesh can appear black at first render.

*Igtl client is not thread safe.*

Make io_igtl`::Client`thread safe at connect / disconnect.

OpenIgtLink socket class isn't thread safe at connection due to internal function calls (details in #736).

*Dicom readers does not have the good scale along the z axis.*

*Tuto02DataServiceBasic cannot load sample data.*

Tuto02DataServiceBasic loads an image at start, but path to the image was hard-coded (`../../data/patient1.vtk`).

A sample image was provided in the resource folder to the tutorial and loaded at startup.

*Jpeg Writer (ITK) causes the application to crash upon usage.*

ITK jpeg image writer has been replaced by vtk image writer service when saving snapshots in sightviewer.

In addition, the image number of component in the vtk image writer service has been updated, as well as the lock systems.

*Remove VTK warnings when reading meshes.*

Redirect VTK messages (warnings/errors) in a VTK.log file.

*Activities saving failed.*

Saving activities was blocked due to extra "::" in the plugin.xml of ioActivity.

*Add some Dependencies.cmake.*

To build our current projects with sight 21.0, we needed to add missing `Dependencies.cmake` files, otherwise people would have to find the necessary `find_package`commands to run first.
Besides this, an unrelated change has been made to reenable the load of extra bundle paths. This is needed when you depend on more than two repositories.

### test

*ServiceTest randomly fails.*

This fixes two tests in the `serviceTest` suite. Basically, it is just about waiting for the correct condition before testing the result.

### ui

*Preferences path is broken.*

The application name in the preferences file path is deduced from the profile name attribute. During the generation, this property was not generated properly. This fixes it.

*Floating buttons are frozen when moving underlying main window.*

### viz

*Dialog deadlock with 'always' render mode and GTK.*

This removes the 'always' render mode in `viz::scene3d::SRender`, which is useless and may cause deadlocks with pop-up dialogs.

*Default SRender transparency mode is broken.*

Set the default transparency to `HybridTransparency`.
(SightViewer's transparency has been set to default instead of `DepthPeeling`)

*Cropbox reset freezes SightViewer.*

We avoid a deadlock in `SVolumeRender::updateClippingBox()` by unlocking the clipping matrix data before calling the interaction callback.

*SightViewer crashes at start.*

In `SVideo`, the SceneManager was used to get the viewport. The correct way to retrieve is to get it through the layer.

## New features:

### build

*Ubuntu 21.04 support.*

### core

*Implement SessionWriter and SessionReader and many other things.*

This is the foundation of the new serialization mechanism.

### New "crypto" package in `libs/core/crypto`

* `secure_string`: a std`::basic_string`implementation with a custom `secure` allocator which erase used memory on de-allocation. This class is mostly used to store sensitive data like password.
* `SHA256`: computes SHA256 cryptographic hashes
* `Base64`: encode/decode to/from base64 strings
* `AES256`: encrypt/decrypt to/from strings using AES with 256 bits key.
* Unit tests in `libs/core/core/test/api/crypto/CryptoTest.xxx`

### New `ArchiveReader` and `ArchiveWriter` classes in `libs/io/zip`

* Fixed some nasty bugs on ressource deallocation with WriteZipArchive / ReadZipArchive: Handle from files opened inside archive were not closed
* Implements some thread safety measures: Due to minizip implementation, to avoid strange corruption problems, an archive could only be opened either in `read` or in `write` mode. In the same way, only one file in a archive should be opened at once. With old classes, no restrictions was applied, Yolo mode by default.
* Allows to specify, as the zip format allows us to do, one compression method, one compression level, and one encryption key by file. This let us, for example, adapt the compression level, when the stored file is already strongly compressed, like for a mp4 file.
* Use RAII mechanism to close file handle inside archive, global zip handle, etc. It means that opening the same archive, the same file inside an archive in the same scope, or even simply when the ostream/istream is alive, will dead lock (it's a feature, not a bug...)
* Unit tests in `libs/io/zip/test/tu/ArchiveTest.xxx`

### Refactor UUID

Not planned, but since `core::tools::Object::getUUID()` was not `const`, it requires some attention. Basically, only the generator has been kept in `core`::tools::UUID``and all code went in `core::tools::Object`.

* Removed double mutex, strange double indirection, etc.. while retaining most of the feature (especially the `lazy` getter/generator, which a really doubt it is useful, but I did not Benchmarked it, so I decided to keep it)
* `core::tools::Object::getUUID()` is now const at the operation doesn't modify external and even internal class state.
* Unit tests in `libs/core/core/test/api/tools/UUIDTest.xxx`

### Implementation of SessionWriter, SessionReader, ... All in `libs/io/session`

#### New `PasswordKeeper` password management system

Designed to hold passwords, in the case you don't want the user to retype every 3 seconds its password.

* Replace the less secure and more `handcrafted` solution in `libs/ui/base/preferences/helper.xxx`
* Store the password in an AES256 encrypted `secure_string`. The key is computed in a deterministic way (no surprise), but should not be so easy to guess. Once the password is no more needed, or when the application quit, it will be erased from memory.
* Allows to store a "global" password along several "instance" passwords.
* `libs/ui/base/preferences/helper.xxx` has been a bit refactored to use the new `PasswordKeeper`
* No need to say, a password should never be serialized on disk or in a database. Use only password hash for that.
* Unit tests in `libs/io/session/test/tu/PasswordKeeperTest.xxx`

#### New `SessionWriter`

It's an implementation of a base::reader::IObjectWriter. It's purpose is to be used in a service to "write" an object to a session archive. It is the public API interface for writing a session archive. Basic usage would be something like:

```cpp
auto sessionWriter = io::session::SessionWriter::New();
sessionWriter->setObject(myActivitySeries);
sessionWriter->setFile("session.zip");
sessionWriter->setPassword("123")
sessionWriter->write();
```

#### New `SessionReader`

It's an implementation of a base::reader::IObjectReader. It's purpose is to be used in a service to "read" an object from a session archive. It is the public API interface for reading a session archive. Basic usage would be something like:

```cpp
auto sessionReader = io::session::SessionReader::New();
sessionReader->setFile("session.zip");
sessionReader->setPassword("123")
sessionWriter->read();

auto myActivitySeries = sessionReader->getObject();
```

> You may have noticed the slight change in classical `IObjectReader` API usage. It is no more needed to guess, before reading, the type of object to deserialize. Instead of "setting" and empty object, we simply "get" it back, once the "read" done

#### New `SessionSerializer`

This class contains the the main serialization algorithm. It also contains a specialized serializer (one for each kind of data object) registry, so it can delegate specific serialization tasks. The serialization is done on two medium: one boost ptree and, for big binary file, directly into the archive. The final ptree will also be stored in the archive as the index.json.

The algorithm is really straightforward, not to say "basic":

1. Extract the UUID of the input object and the class name.
1. With the UUID, look into an object cache. If the object is already serialized, put a reference on the current ptree node, and stop.
1. With the classname, find a suitable serializer and call it. The specific serializer will update the current ptree node and maybe store binary file to the archive.
1. The serializer from step (3) will eventually return also a list of "children" object (this is the case for composite objects). Recall recursively the step (1) on them.
1. If the object contains `fields`, recall recursively the step (1) on them.

> this MR text will be my base for README.md which I will write once everything are reviewed. Don't panic if you don't see it now, a new MR will be issued later.

#### New `SessionDeserializer`

This class is the counterpart of `SessionSerializer`. Instead of a specialized serializer registry, we have a specialized deserializer registry.

The algorithm is a bit more complex, but still straightforward:

1. Extract the UUID and the class name of the current object stored in the ptree index.
1. With the UUID, look into an object cache. If the object is already deserialized return it, and stop.
1. With the UUID, look into the global UUID object registry. Use it to avoid unneeded object instantiation. It also allow us to safely deserialize children which have direct reference on parent objects.
1. With the classname, find a suitable deserializer.
1. First, deserialize the child objects by recalling recursively the step (1) on them.
1. Deserialize current object, passing the deserialized child objects from step (5)
1. If the object contains `fields`, recall recursively the step (1) on them.

#### Specific object serializer/deserializer

For now, only a very small subset is implemented. This subset should however cover most cases encountered while writing a serializer for a new kind of data object:

* ActivitySeries\[De\]serializer:
  * Demonstrate how to serialize object with a child `Composite` reference and how to use another serializer to mange inheritance (with `Series`).
* Composite\[De\]serializer
* Equipment\[De\]serializer
* Generic\[De\]serializer:
  * Demonstrate how to serialize "generic" object (Integer, Float, Boolean,String)
* Mesh\[De\]serializer
  * Demonstrate how to serialize big binary data to archive
* Patient\[De\]serializer
  * Demonstrate how to encrypt sensitive data, regardless of the encryption status of the archive
* Series\[De\]serializer
* String\[De\]serializer
  * This serialize String to and from Base64. Since boost ptree have serious flaws with strings with special characters in it, encoding to base64 is a suitable workaround, but still a bit overkill..
* Study\[De\]serializer

#### Unit tests

all in `libs/io/session/test/tu/SessionTest.hpp`

*Promotes code from internal to open-source.*

* Create `SRecurrentSignal` service in modules/ui/base/com. This service emits a signal at a defined frequency.
* Create `Mesh` lib in libs/geometry/vtk and the corresponding unit test. This library computes the center of mass of a mesh using vtk.

### graphics

*Add a material transparency editor.*

A new service `SMaterialOpacityEditor` was added, which allows tweaking the opacity of a `sight`::data::Mesh``via  `sight`::data::Material``with a slider.

*Quad mesh integration in fwRenderQt3D.*

### io

*Drop in replacement for SReader and SWriter to serialize an application session.*

### Main feature

Two services `sight`::module::io::session::SReader``and `sight`::module::io::session::SWriter``were implemented. They read/write a root data object from/to a session file on a filesystem.

The session file is indeed a standard "ZIP" archive, while the compression algorithm for files inside the session archive is ZSTD. A standard archive reader could open a session file, if it is able to handle ZIP archive with ZSTD compression.

The archive can be password protected using AES256 algorithm and the compression level is set individually, depending of the type of data to serialize.

The service configuration includes specifying the file extension and the password policy for encryption.

Configuration example:

```xml
<service type="sight::module::io::session::SReader">
    <inout key="data" uid="..." />
    <dialog extension=".sample" description="Sample Sight session file" policy=always/>
    <password policy="once" encryption="salted"/>
</service>
```

The dialog policy specifies when the open dialog is shown:

* **never**: never show the open dialog
* **once**: show only once, store the location as long as the service is started
* **always**: always show the location dialog
* **default**: default behavior, which is "always"

The password policy defines if we should protect the session file using a password and when to ask for it:

* **never**: a password will never be asked and the session file will never be encrypted.
* **once**: a password will be asked only once and stored in the memory for subsequent uses. The session file will be encrypted.
* **always**: a password will always be asked. The session file will be encrypted.
* **default**: uses the builtin default behavior which is "never".

The encryption policy defines if we uses the password as is or with salt. It can also be used to encrypt without password:

* **password**: Use the given password for encryption.
* **salted**: Use the given password with salt for encryption.
* **forced**: Force encryption with a pseudo random hidden password (if no password are provided).
* **default**: uses the builtin default behavior which is "password".

### General improvement:

* `ExActivities` has been modified to use the new session services instead of atoms
* new `TemporaryFile` class in `core::tools::System` that uses RAII to delete the associated file as soon as the `TemporaryFile` class is destroyed.
* `core::tools::System::RobustRename()` now have an optional parameter to force renaming, even if the target already exists (it will be first deleted)
* `ui::base::Cursor` improvement: `BusyCursor`, `WaitCursor`, `CrossCursor` classes that use RAII to restore back "default" cursor, even if an exception occurs
* `ui`::xxx::dialog::InputDialog``improvement: add a "bullet" mode for password field.
* `ui`::xxx::dialog::MessageDialog``improvement: add a "retry" button.

*Serialize most objects.*

The serialization is done through two classes: SessionSerializer, SessionsDeserializer, and many .hpp with two functions: `serialize()` and `deserialize`, depending of the type of data object to serialize.

For example, the serializers for meshes and images are coded respectively in `Mesh.hpp` and `Image.hpp`. They are good samples to demonstrate how it is possible to use a well known format to serialize objects. The Sight images / meshes are converted into VTK format using Sight helpers and are then saved with the now "standard" VTK way using `vtkXMLImageDataWriter` for image and `vtkXMLPolyDataWriter` for meshes.

As a side notes, since the files are stored in a zstd compressed zip file, and since VTK doesn't provide any way to use an output streams, the VTK writers are configured as such (image and mesh are equivalent):

```cpp
vtkWriter->SetCompressorTypeToNone();
vtkWriter->SetDataModeToBinary();
vtkWriter->WriteToOutputStringOn();
vtkWriter->SetInputData(vtkImage);
```

This allow us to compress only one time and use fast zstd. Since the compression level can be set independently, some test need to be done to find the best efficiency. For now it is the "default" mode that is used, but better compression ratio at the expense of compression speed (not decompression!) is also possible.

The drawback for using `WriteToOutputStringOn()` is that the complete data need to be written in memory before being able to serialize it. Shame on VTK for not providing an easy way to use c++ streams...

Most serializers are far more simple as we only write/read values into a Boost property tree, a bit like before, but without the complexity of "atoms" tree. The version management is also quite simple, we write a integer in the tree and read it back. It is up to the user to add the `if(version < 666)...`

## Enhancement:

### build

*Enable support of PCH in unit tests.*

Sight CMake targets of type TEST now build with precompiled headers, which speed-ups a bit the global build time.

### core

*Wrap notification signal emission in a function.*

Wraps emission of "notification" signals into `IService`::notify``with an enum class NotificationType (SUCCESS, INFO, FAILURE) and the message.

*Add helper function to ease error handling in boost::property_tree.*

This adds a new function to ease error handling in `boost::property_tree`.

```cpp
core::runtime::get_ptree_value(config, "test.true", false); // returns true
core::runtime::get_ptree_value(config, "test.yes", false);  // throws
core::runtime::get_ptree_value(config, "test.foo", false);  // returns false
core::runtime::get_ptree_value(config, "test.foo", true);  // returns true
core::runtime::get_ptree_value(config, "test.true", 42);  // throws because it implicitly request an integer
```

*Move TransferFunctionManagerWindow configuration to config folder.*

- Move `TransferFunctionManagerWindow.xml` to `configs/viz/scene3d`.
- Add a link in `sightViewer` application to use it.

### doc

*Add a README.md for each target.*

We now provide a short documentation file for each target. The goal of this file is to give an overview of the content of the library or the module to the developer. It is complementary with the doxygen and sight-doc but it should not overlap with them.

### graphics

*Support keeping the original image size and bilinear filtering in SVideo.*

* Add support for switching between nearest and bilinear filtering for the texture
* Add support to scale or not the image to the size of the viewport

### io

*Make realsense support optional.*

*Add missing description for pdf writer label.*

add description in plugin.xml of modules/io/document

### ui

*Send value when released slider in SParameters.*

Adds an option to emit value of SParameter sliders only when releasing the slider, this avoids multiple sending when moving the slider.

*Add option to fix the size of QToolButton on a toolbar.*

Adds an option on Toolbar layout to resize buttons to the wider one.

This ensures to keep same size between each button and fix some glitches when larger button was hide / show.

### viz

*Enhance tracked us display.*

- change UsUltrasoundMesh default us format, and allow xml configuration of this format
- add SMatrixViewer significant number for display

## Refactor:

### build

*Remove obsolete activities.*

The following activities were removed:
- blendActivity
- registrationActivity
- 3DVisualizationActivity
- volumeRenderingActivity

Only 2DVisualization is kept, and renamed to `sight::activity::viz::negato`. Indeed some configurations of this activity are used in other activities and in **dicomxplorer**.

The following activities were fixed:
- DicomPacsReader
- DicomPacsWriter
- DicomFiltering
- DicomWebReader
- DicomWebWriter

Also, the loading of DICOM images from the filesystem or a PACS in SightViewer should also be fixed.

### core

*Use data::ptr everywhere.*

This is the final merge request that generalizes the usage of data`::ptr`instead of IService::get*/getLocked*/getWeak*.

The deprecated `DynamicType` class was also removed, which really helps to clear the build log from many warnings.

Last, Ogre runtime deprecation warnings were also processed, which implied to refactor a bit the material and shader code.

*Move remaining misplaced files.*

- Move several services interfaces from `sight::service`:
  - `IGrabber` -> io
  - `IRGBDGrabber` -> io
  - `ICalibration` -> geometry_vision
- Rename `IOperator` as `IFilter` to match the new naming scheme and avoid synonyms
- `IParametersService` is renamed into `IHasParameters`, moved in `ui_base` and is no longer a service. Thus any specialized service can inherit this interface using multiple inheritances.
- Rename `pchServices/pchServicesOmp` into `pchService/pchServiceOmp` (we have used the singular everywhere for `service`)
- Renamed and moved `sight`::module::ui::dicom::SSeriesDBMerger``into  `sight::module::ui::series::SPushSelection`, since it pushes a selection (vector) of series into a `SeriesDB`.
- Removed duplicated `module`::geometry::generator::SUltrasoundMesh``and moved `module`::geometry::generator::SNeedle``into the same module than the duplicate of `SUltrasoundMesh`, i.e. in `module::filter::mesh::generator`.

*New pointer types to manage data in services.*

Two new pointer types are introduced, which the only aim is to be used as service class members:
- `sight`::data::ptr`:`single data
- `sight`::data::ptr_vector`:`group of data

For instance, they can be declared this way in a class declaration:

```cpp
class STest : public IService
{
    ...
private:
    data::ptr<data::Image, data::Access::in> m_input {this, "image"};
    data::ptr_vector<data::Mesh, data::Access::inout> m_inoutGroup {this, "meshes", true, 2};
    data::ptr<data::Object, data::Access::out> m_output {this, "genericOutput", false, true};
};
```

`this` must be passed as the first argument, with a class instance inheriting from `IHasData`. So far, only `IService` inherits from this interface, but other cases might appear later. It is used to register the pointers in the OSR and get/set their content automatically, mainly with the `AppManager` (Xml based and C++ based). This prevents calling `registerObject()` for each data in the service constructor (it was almost never done because this only breaks C++-based apps, but normally it should have been done everywhere). Actually, this registration method was removed from the public interface of `IService` so you can no longer call it and there is no risk of conflict. All occurrences were refactored to use these new pointer types.

To retrieve the data, it is simple as using a `data::mt::weak_ptr`, so you can simply call

```cpp
    auto input = m_input.lock(); // returns a data::mt::locked_ptr<data::Image>
    const auto data = *input;  // returns a data::Image&
    const auto data_shared = input.get_shared(); // returns a data::Image::csptr

    // Access data in a group using indexes
    for(int i = 0; i < m_inoutGroup.size(); ++i)
    {
       auto data = m_inoutGroup[i].lock();
       ...
    }

    // Access data in a group using for range loop - this gives access to the underlying map,
    // that's why '.second' should be specified, while '.first' gives the index
    for(const auto& data : m_inoutGroup)
    {
        auto data = data.second.lock();
        ...
    }

    // Alternative using structured binding
    for(const auto&[index, data] : m_inoutGroup)
    {
       auto lockedData = data.lock();
       ...
    }
```

*Booleans attributes configuration parsing.*

Harmonize XML configuration of services code by replacing the use of "yes"/"no" with "true"/"false".

*Replace getPathDifference() by std::filesystem::relative().*

*Reorganize all targets and rework the build system.*

Sight 21.0 brings a major update on the naming scheme. We renamed almost every target, according a new naming scheme, and we reduced drastically the scope of categories we use to sort targets. Many targets were merged together. We lost some modularity but we gained in simplicity.
Also the build system was partially rewritten. Now it relies only on standard CMakeLists.txt, and no longer on our custom Properties.cmake. We no longer support injecting external repositories into a Sight build. To build other repositories, we use what we called before the SDK mode, which is actually the classic way to link some code to another in C++.
Last we introduced a global `sight`::``namespace, which is also reflected on the filesystem. This makes extensibility easier by avoiding naming conflicts.


# sight 20.3.0

## New features:

### core

*Add services for echography navigation and simulation.*

Add various services that help to build applications based on echography.

* create `cvSegmentation` module
    * add `SUltrasoundImage`:  segments an ultrasound image from an echography video
    * move `SColourImageMasking` service from `colourSegmentation` module
* add several services in `maths`:
  * `SMatrixList`: manages a list of matrices
  * `STransformLandMark`: applies a matrix transform to a landmark
  * `STransformPickedPoint`: applies a matrix transform to a picked point in a scene
  * `SPointToLandmarkDistance`: compute the distance between a point and a landmark
  * `SPointToLandmarkVector`: compute a direction vector between a point and a landmark
* create `opUltrasound` module
  * `SUltrasoundMesh`: generates a 3D mesh to display an echographic plane in a 3D scene

### graphics

*Add adaptor to show the patient orientation in Ogre scenes.*

* Add a new SOrientationMarker Ogre adaptor service, that allows to display a mesh showing the orientation of the scene at the bottom right of the screen (equivalent to the VTK SOrientationMarker)
* Use SOrientationMarker service in OgreViewer

*Synchronize 2D negatos when picking.*

* Synchronize 2D negatos when cliking on the image.
* Add a parameter in `SVoxelPicker` to enable the update of image slices indexes. Send ``::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG``with the picked indexes.

*Allow to delete the last extruded mesh.*

Add a slot to delete the last mesh generated by SShapeExtruder.

### ui

*Allow to delete series from the selection view from a button.*

## Bug fixes:

### core

*Fix SLandmarks by setting an optional input for the key "matrix".*

*Save preferences when they are changed.*

Save preferences when updated from GUI.

*OgreViewer doesn't read properly json with modelSeries.*

Connects seriesDB modified signal to the extractImage/Mesh update slots in OgreViewer app.

*FwServicesTest  randomly fails.*

Fix the fwServices`::ut::LockTest::testDumpLock`by adapting the test to asynchronous unlocking.

In real world application, the bug should not be present, although the randomly failing test are annoying.

### doc

*Update licenses.*

### graphics

*Default visibility of modelSeries adaptor is not taken into account.*

Take into account default visibility in `visuOgreAdaptor::SModelSeries`.

*Mesh normals aren't computed each time.*

Compute normals each time a mesh is updated (if mesh has no normals). Before it was only computed if mesh needs a reallocation (number of vertices greater than the previous mesh).

### io

*Mesh attributes created after getInput aren't locked.*

Some attributes of ``::fwData::Mesh``may be created later (aka after the New()), like points/cells normals/colors/texture coordinates.

But this can lead to some unpredicted unlock issues on internal arrays of `::fwData::Mesh`, when using new RAII methods like `getLockedInput` if the mesh hasn't point normals when getting the locked input the corresponding array will not be locked (because it will be equal to nullptr), so if you want to set normals afterwards the corresponding array will still be in an unlocked state.

We now initialize all attributes (colors/normals/texture) at mesh creation.
A binary mask corresponding to which attributes are currently used in mesh can be checked. We don't rely anymore on the `nullptr` of internal arrays.

We also modify mesh iterator to remove the lock it performs, so we need systematically call `mesh->lock()` to lock internal array only if we don't use RAII methods (`getLocked*`) from services.

Along with previous modifications, we also modify internal structure of CellsData arrays, previously we store `std::uint64_t` values, we change values to `std::uint32_t` much more adaptable on both 32bits and 64bits systems.

To deal with all previous modifications we also updated data mesh version (version 4), fwData version (V15) & arData version (V17AR). We provide structural patches to load/save previous version of data.

Some of our unit test has also be updated to handle all previous modifications.

This could break the API.

### ui

*Fix visuOgre/SNegato3D::setvisible() freeze in ExRegistration.*

*Adaptor SLandmarks deadlock.*

*Show distances button in OgreViewer bugs.*

### vision

*Fix SNeedleGenerator colors.*

## Enhancement:

### core

*Add boolean record slot to SFrameWriter.*

Add boolean `record` slot to SFrameWriter and SVideoWriter

*Improve SPreferencesConfiguration to prevent setting wrong parameters.*

### graphics

*Allow to interact with Ogre landmarks.*

Allow mouse interaction on Ogre landmarks

*Add default visibility configuration for SLandmarks adaptor.*

Adds a tag to configure default visibility in `SLandmarks`.
Enables the possibility to hide the adaptor by default in the xml configuration.

*Add default visibility configuration for SNegato2D.*

Uses the tag from IAdaptor to configure default visibility in `SNegato2D`.
Enables the possibility to hide the adaptor by default in the xml configuration

### io

*Use igtl::serverSocket to get real port number.*

Allow the use of port 0, setting port = 0 will ask socket to find the first available port number.
Thus we need to modify the way the port is stored in igtlNetwork::Server, and use igtl`::ServerSocker::GetServerPort`to have the real port number.

*Make igtlProtocol::MessageFactory  extendable from other libraries.*

Make the MessageFactory public, to be used from outside of `igtlProtocol`.

This is helpful to registrar new type of IGTL Messages that are not necessary implemented in `igtlProtocol` library.

## Refactor:

### core

*Add services for ITK and VTK operator modules.*

### graphics

*Move all material to fwRenderOgre and deprecate the `material` module.*

*Remove all usage of the deprecated API in Ogre modules and libraries.*

* Remove deprecated functions in Ogre libs/modules.

* Use the new RAII system in Ogre modules.

### io

*Remove videoOrbbec.*

* Remove videoOrbbec from sources.


# sight 20.2.0

## New features:

### graphics

*Add a new picker to really pick the current slice image.*

- When using an Ogre picker on a medical image, the picked position does not correspond to a voxel position of the image. In fact, the Ogre negato display the image between \[0;size\] and the picker needs to pick between \[0;size\].

*Add Qt3D SMaterial adaptor.*

Implement qt3d material service:

* `::fwRenderQt3D`::data::Material``object handling a qt3d material
* `::fwRenderQt3D`::techniques::Lighting``object handling a qt3d technique with shader programs to compute lighting mode and rendering options such as point/line/surface rendering or normals visualization
* `::fwRenderQt3D`::SMaterial``adaptor used to create a qt3d material from sight data and attach it to the render service

### io

*Read tfp TF from directory with ::uiTF::SMultipleTF.*

*Add setNotifyInterval method to the Grabber interface.*

Adds an option in the videoQt Player to change the notifyInterval option. This is useful to require frame through "setPosition".

*Add birth date request field for PACS query editor.*

### ui

*Allow to manage opacity of the TF in TF editors.*

Allow to manage a whole TF opacity at the same time from the TF editor.

## Refactor:

### core

*SpyLog rework.*

SpyLog has been unnecessarily complex for a long time. In this rework, we propose to:
- deprecate the loglevel `TRACE`. Only `SLM_TRACE_FUNC()` remains, but its occurrences should never be committed.
- deprecate `OSLM_*` macros in favor of `SLM_*` macros, which now take stringstreams as input (no performance penalty)
- all loglevels are now always compiled, which means that the big bloat of `SPYLOG_*` CMake variables were removed.
- occurrences of `OSLM_*` macros were replaced by `SLM_*` macros
- occurrences of `O?SLM_TRACE*` macros were removed or replaced by higher levels logging macros
- the default displayed log level is now `WARN`
- the path of files displayed in the output was shortened to keep the minimum information needed: namespace(s), source fil

*Deprecate configureWithIHM.*

Deprecate `configureWithIHM` and use `openLocationDialog`, backward compatibility is kept until sight 22.0

*Deprecate *dataReg modules and move *dataCamp content to *data.*

This deprecates the usage of dataReg and arDataReg modules, which were so far mandatory as `REQUIREMENTS` for any XML configuration using data. This was done in several steps:

1. a function was added in `fwRuntime` to allow the loading of any regular shared library (we could only load libraries from modules before).
2. `AppConfigManager` was modified to guess the library name when parsing configuration, and load the library accordingly
3. `dataReg` and `arDataReg` were emptied from the hacky symbols, and deprecated
4. the only real useful code, i.e. the XML data parsers from `dataReg` were moved to `fwServices`, so that we can remove the whole module later.
5. `dataReg` and `arDataReg` were replaced by `fwData`, `fwMedData` and `arData` in `Properties.cmake` files
6. `dataReg` and `arDataReg` were removed from all `plugin.xml` requirements.
7. all `*DataCamp` libraries were deprecated and most of their content imported in the corresponding `*Data` libraries. They were linked in an application thanks to the dataReg modules and hacky symbols. I could have loaded the modules manually like data libraries but I think it is simpler to gather everything related to a data into a single library. There is no real use case in my mind where we want only the data without any introspection. And we do not need to plan for a coexisting alternative to perform the introspection, we are anyway too tight with Camp.

This is not a breaking change and it is still possible to keep the deprecated modules.

### graphics

*Deprecate VTK.*

* Deprecates VTk generic scene
* Updates all samples to use Ogre
* Cleans all sample files (xml/hpp/cpp/cmake)
* Fixes some errors in our samples
* Adds a new module `visuBasic` for basic generics scenes
* Rename samples

### io

*Deprecate VLC.*

Deprecation of `videoVLC` since VLC package will no longer be available with new conan system (sight 21.0).

### vision

*Deprecate the bundle.*

Deprecate the bundle `uiHandEye`

## Enhancement:

### core

*Complete meshFunction baricentric  API.*

Add a method `isInsideTetrahedron( const ::glm::dvec4 barycentricCoord);` in the MeshFunction.

*Add [[nodiscard]] attribute for weak/shared_ptr::lock().*

Adds `[[nodiscard]]` attributes in weak_ptr and shared_ptr lock() function. This avoids using lock() as lock function and force user to use returned lock_ptr object

*Update conan.cmake file to v0.15".*

Update conan.cmake file to v0.15.

### graphics

*Improve scene resetting.*

### io

*Increase the DICOM services log level.*

Add detailed logs for our communications with the PACS.

*Improve PACS research fields.*

* Make research fields of ioPacs case insensitive.
* Improve date research.
* Avoid to read unreadable modalities with the series puller.

*Execute PACS queries from a worker.*

``::ioPacs::SeriesPuller``now retrieves series in a worker to avoid the app to freeze.

*Improve ioPacs queries.*

Adds a PACS series selection to `OgreViewer`.

* `SPacsConfigurationEditor`: send notifications.
* `NotificationDialog`: use the height of the notification instead of the width for computation.
* `SSelector`: remove margin.
* `SNegato2D` and `SNegato3D`: avoid a division by 0.
* `Utils`: sometime the default texture as the same size than the image, so there is never initialized.

### ui

*Rename bad named functions.*

* Deprecated `::fwGui::dialog::MessageDialog::showNotificationDialog()` and propose a new `::fwGui::dialog::MessageDialog::show()`
* Deprecated `::fwGui::dialog::NotificationDialog::showNotificationDialog()` and propose a new `::fwGui::dialog::NotificationDialog::show()`
* Renamed all occurrences.

## Bug fixes:

### build

*Add deps between targets and their PCH for Unix Makefiles.*

This fixes errors when building using the Unix Makefiles generator and multiple processes.

### core

*Load library is broken when launching activity using wizard.*

Adds `loadLibrary` method in `new.cpp` file in order to load libraries before starting `AppConfigManager` or `ActivityWizard`

### io

*Cannot save json on mounted disk.*

Create a custom `rename` function that will first try a `std`::filesystem::rename``and then fallback to a basic copy-remove scenario.

Using this avoid the `Invalid cross-device link` error, when saving a json(z) file on a another disk/volume.

*Tf names without extension are not saved.*

### ui

*Enable the current activity of the sequencer if no data are required.*

* Enable the current activity of the sequencer if it's needed.
* Avoid double start of the config launcher.

*The TF editor suppresses points.*

### vision

*Display left and right view vertically.*

Until a better fix, display left and right camera vertically in extrinsic calibration. Doing this provide a better width view of video.


# sight 20.1.0

## Bug fixes:

### build

*Remove ".sh" extension from launcher scripts.*

Both build and install targets are impacted.

Adds `.bin` extension to executable target such as utilities/tests or fwlauncher to distinguish executable targets from shell scripts launcher

*Ninja install issue on linux.*

Test if the install directory ( `CMAKE_INSTALL_PREFIX`) is empty, if not print a warning at configure time.

### ci

*Build on master branch on Windows.*

Fix quoting of branch name while cloning

### core

*Crashes when closing OgreViewer.*

OgreViewer crash at closing, this crash happens because the action `openSeriesDBAct` is working on a thread.

Now we wait until services are properly stopped after `service->stop()` in XXRegistrar class.

*Replace misleading error logs in SlotBase.*

When trying to call/run a slot, an error log was displayed if the signatures don't match, but it is allowed to call a slot with less parameters.
Now, an improved information message is displayed if the slot cannot be launched with the given parameters, then we try to call the slot removing the last parameter. The error is only raised if the slot cannot be called without parameter.

### graphics

*Fix screen selection for Ogre scene.*

Enable the full screen of Ogre scene on the right monitor.

For more informations: https://stackoverflow.com/questions/3203095/display-window-full-screen-on-secondary-monitor-using-qt.

*Slice outline colors are inverted between axial and sagittal in OgreViewer.*

Use blue outline color for Ogre's sagittal view, and red outline color the axial one.

### io

*Change the overwrite method when saving using ioAtoms.*

Change the overwrite method of `ioAtoms::SWriter`. Previously, when a file was overwritten, it was deleted then saved. But if the saving failed, the old one was still deleted.

Now, the file is saved with a temporary name, then the old one is deleted and the temporary name is renamed with the real one.

*Realsense doesn't work on latest 5.4 linux kernel.*

Update the version of librealsense package to latest (2.35.2). This fix issue of using realsense camera on linux kernel > 5.3

*Remove useless requirement in ogreConfig.*

Remove `ioVTK` module from `ogreConfig`. No reader, writer or selector was used in the configurations.

*Change realsense camera timestamp to milliseconds.*

Correct the Realsense grabber timestamp from microseconds to milliseconds

*Allow to re-start reading a csv without re-selecting the file path.*

Update 'SMatricesReader' to properly close the file stream when calling `stopReading` slot and clear the timeline.
Properly stop the worker to avoid a crash when closing the application.
Also remove the default delay of 1s when using one shot mode.

*Remove hard-coded codec and only offer mp4 as extension available in SVideoWriter.*

* Limits extension selection to only `.mp4` for now.
* Limits FOURCC codec to `avc1`. This codec is also linked to the `.mp4` extension.

*Fix a crash with OpenCV grabber.*

- Start/stop the worker of OpenCV grabber when service start/stop

### ui

*Notifications crashes when no active window was found.*

Avoid crash of app when notifications were displayed without active window (focus lost).
If no active window is found, print an error message and discard the current notification.

*Prevent dead lock when opening organ manager.*

Block Qt signals in SModelSeriesList when changing the state of the "hide all organs" checkbox.

## Enhancement:

### ci

*Re-enable slow tests and remove ITKRegistrationTest.*

### core

*Add empty constructor to Array iterator.*

* Add empty constructor to Array BaseIterator, to make it compatible with std::minmax_element

### doc

*Improve IService doxygen about output management.*

Describe how to properly remove the service's outputs and why and when we should do it.

Add a check when the service is unregistered and destroyed to verify if it owns an output that will also be destroyed (if the output pointer is only maintained by this service). Currently only an error log is displayed, but it may be replaced by an assert later. Other services may work on this object but didn't maintained the pointer (they used a weak pointer).

*Update the screenshots in README.md.*

Update README.md with more up-to-date screenshots/animations

### graphics

*Use ogre to display videos in ARCalibration.*

* Use Ogre for `ARCalibration`.
* Properly clean ogre resources before root deletion.

*Manage default visibility of `SNegato3D` from XML configuration.*

Call `setVisible` method at the end of the starting method.

### io

*Improve ioPacs module.*

* Improves connection/disconnection management.
* Improves the UI and request forms.
* Use new disconnection method of dcmtk.
* Fills new `medData` attributes.

### ui

*Add goTo channel in ActivityLauncher.*

Add `goToChannel`in `ActivityLauncher.xml` in order to call `goTo` slot from an activity.

*Replace Ircad and Ihu logos by the unique Ircad/Ihu logo in the sequencer view.*

Modify ActivityLauncher configuration to replace Ircad and Ihu logos by the new version of Ircad/Ihu logo.

*Improve assert messages in fwGui.*

Add as much information as possible in the assertion messages: service uid, why the error occurs, ...

*Add check/uncheck slot in SSignalButton.*

These two new slots allow to easily check/uncheck the button without using a boolean parameter. They can be connected to a SSlotCaller or any signal.

Update TutoGui to show an example with these two new slots.

*Allow to delete all reconstructions or one from a right click.*

- Allow to delete all reconstructions from `SModelSeriesList`
- Delete specific reconstruction from a right click.

*Improve Qt sequencer colors management.*

Allow to manage more colors in the Qt sequencer.

## New features:

### core

*Implements copy for mesh iterators.*

Implement `operator=()` and `operator==()` on mesh iterator's internal structs (PointInfo and CellInfo).
It allows to use `std`::copy``(or other std algorithms) with Mesh iterators.
Update Mesh unit tests to check using `std::copy`, `std`::equal``and `std::fill`.

### graphics

*Add new experimental Qt3D renderer.*

A new experimental renderer based on Qt3D has been integrated, providing basic support for mesh visualization.

Three samples are provided to show how Qt3D could be integrated into a classic XML application, a pure c++ application
or a QML application:
* TutoSceneQt3D
* TutoSceneQt3DCpp
* TutoSceneQt3DQml

The following classes and services have been introduced:
* `::fwRenderQt3D`::GenericScene``object handling a qt3d scene
* `::fwRenderQt3D`::FrameGraph``object to attach to a GenericScene, allowing to define custom qt3d framegraphs
* `::fwRenderQt3D`::SRender``service used to define a GenericScene object within Sight context and attach adaptors to it
* `::fwRenderQt3D`::Mesh``object creating a mesh from sight data using qt3d geometry renderer
* `::fwRenderQt3D`::IAdaptor``class providing base functionalities for Qt3D adaptors.
* `::visuQt3DAdaptor`::SMesh``adaptor used to create a qt3d mesh from sight data and attach it to the render service

*Improve Ogre's SLandmarks for 2D scenes.*

Improve Ogre resources management in '::visuOgreAdaptor::SLandmark' and display the landmarks in 2D negato scenes only on the slice of the landmark.

*Add Ircad-IHU logo overlay in Ogre Scene.*

- 8 positions are supported: left-top, left-center, left-bottom, center-top, center-bottom, right-top, right-center and right-bottom.

### io

*Allow to save and load multiple TF with TF editor.*

* Add new buttons to export and import a TF pool to `SMultipleTF`
* Use the new API in `SMultipleTF`.

## Refactor:

### core

*Manage pixel format in FrameTL.*

Manage pixel formats: Gray Scale, RGB, BGR, RGBA and BGRA.
Now, to initialize the frame timeline, the format must be specified, the previous initialization method (with number of components)is deprecated.
Also improve the unit tests to check timeline initialization and copy.

Update `SFrameMatrixSynchronizer`: use the new pixel format to initialize the image, but still support the old API when the format is undefined. Also use the new service's API with locked and weak pointers.

### graphics

*Rename the layer "depth" to "order".*

Replace `depth` attribute of Ogre layer by `order`.

### ui

*Factorize visibility slots in Ogre3D renderer.*

- Factorise `updateVisibility`, `toggleVisibility`, `show` and `hide`. `setVisible` is the only method to reimplement in subclasses.


# sight 20.0.0

## Bug fixes:

### .gitlab

*Improve sheldon stage.*

Use sheldon and build the project on the merge result instead of the working branche.

*Improve sheldon stage.*

### activities

*Split bundle to separate gui services from core services.*

Separate service using Qt Widget from other services in order to use generic service with a Qml application, because Qt widgets cannot be instantiated in a Qml application.

A new bundle `uiActivitiesQt` was created where `SCreateActivity` and `SActivityLauncher` has been moved. The old `SCreateActivity` and `SActivityLauncher` services has been kept in activities but set as deprecated. A fatal would be raised if you use them in a Qml application.

`ENABLE_QML_APPLICATION` variable has been added in the main CMakeList to define if there is a Qml application that will be launched. It is temporary because we need to keep the old API until sight 21.0 and allow to remove the dependency to Qt Widgets in activities bundle.

### ActivityLauncher

*Fix reader/writer configuration.*

Fix the reader and writer configurations used by `ActivityLauncher` config: use the configuration given in parameters instead of the default one.

### AppConfig

*Fix XML variable regex.*

The regex is used to know if the xml attribute contains a variable (`${...}`). But in a few cases, for example in signal/slot connections, the variable is followed by other chars. For these cases, the regex should not be limited to `${...}`but to `${...}....`.
The unit tests have been updated to check this error.

### AppConfigManager

*Fix useless variable.*

Remove wrong `#ifdef` around `FwCoreNotUsedMacro(ret);`

### Array

*Implement missing copy constructor in Array iterator.*

Implement copy constructor for const iterator and assignment operator.

### build

*Fix build on linux with gcc7 in release mode.*

- fixed sheldon coding style and define in fwCom
- fixed unused variable (release mode) in fwServices

### CalibrationInfoReader

*Wrong image channel order.*

* Convert to the sight's default color format when reading calibration inputs.
* Sort the calibration input by their filename.
* Update the related tests.

### CI

*Disable slow tests until they all pass.*

*Sheldon-mr job doesn't work.*

According to #402, the sheldon-mr job seems to fail when merge commit are present in-between dev and the current mr branch.

Here the `git merge-base` function has been replaced by a diff of revision list between the MR branch and dev, and then keep the oldest common ancestor.

**Explanations**

* `git rev-list --first-parent origin/${CI_COMMIT_REF_NAME}`: revision list of the MR branch
* `git rev-list --first-parent origin/dev`: revision list of dev (/!\\ we need to check is there is a limitation on number)
* `diff -u <(git rev-list --first-parent origin/${CI_COMMIT_REF_NAME}) <(git rev-list --first-parent dev)` : basic diff between the two list
* `| sed -ne 's/^ //p'`: removes the diff line ("+" "-", etc), to keep only the common revisions
* `| head -1`: keep the first form common revisions.

This is, indeed, a bit over-complicated, and can be simplified by a simple merge on the CI side, and then run sheldon, but this avoid any potential merge conflicts checking beforehand.

*Use CI_JOB_TOKEN when cloning sight-data.*

### CMake

*Disable RelWithDebInfo build type.*

*Resolve Qt plugins path in SDK mode.*

Do not install the qt.conf from our Conan package that points to a bad plugin location. This overrides the location set in `WorkerQt.cpp`, preventing Qt application built with the SDK from launching.

Besides this, I also fixed file permissions of installed files (executable were not actually executable) and I corrected a warning at the end of a CMake run because of a wrong usage of `PARENT_SCOPE` in the macro `generic_install()`.

*Resolve missing export when using SDK mode.*

The systematic build of the object library broke the SDK build, because the export was missing for the object library. However, this means users would have to link for instance against `fwCore_SHARED_LIB` instead of `fwCore` which is the object library. This is counter-intuitive and really not desirable.

We chose to invert the logic. `fwCore` is now the shared library and `fwCore_obj` is the object library. This solves the problem for external users but not for sight developers who would need to modify all `CMakeLists.txt` to link fwCore_obj with 3rd party libraries instead of fwCore. We found a middle-ground by building the object library **only** when the variable GENERATE_OBJECT_LIB is set. This variable was set for `fwRuntime` to enable the build of `fwRuntimeDetailTest`. Its `CMakeLists.txt` was last modified to link against `fwRuntime_obj` instead of `fwRuntime`.

On top of that, some corrections were made because of the rework of `fwRuntime` and the usage of `std::filesystem`.

*Conan cmake script not correctly downloaded.*

- Add check hash of `conan.cmake` file for:
  * avoid downloading if the file already exists and is valid
  * download the file if it's corrupted

*Add cmake 3.17.0 support.*

Add cmake 3.17.0 support

*Fix the plugin config command.*

Include the appropriate headers in generated `registerService` files.

*Strip cmake compilation flags on Windows.*

Avoids cmake adding a space at the end of compilation flags

*Fix executable installer.*

This enables the packaging of executable programs under windows.

By the way, the call of generic_install() is now done automatically for all APP and EXECUTABLE targets, thus enabling packaging automatically. A warning is displayed if the macro is called in the CMakeLists.txt of the application.

### conan-deps

*Update hybrid_marker_tracker version.*

Update hybrid_marker_tracker library version that fixes memory leak.

### ConfigLauncher

*Add existing deferred objects to configuration.*

* Notify xml sub-configurations that optional objects already exist.
* Deprecate `IAppConfigManager`.
* Improve the documentation of modified classes.

### dataReg

*Add missing include to compile with latest VS2019 16.6.0.*

### Dispatcher

*Fix wrong mapping for uint8 type.*

`isMapping` method is used by Dispatcher to find the right type for calling template functor.

### doxygen

*Doxygen uses the correct README.md to generate the main page.*

Update Doxyfile to fix the path of the main README.md. Now the doxygen is generated using the main README file as the main page.

### ExIgtl

*Add auto-connections INetworkSender.*

* Add missing auto-connections method in INetworkSender
* Fix ExIgtl example
* Add JetBrains IDEs (CLion) project folder in git ignore file

### ExImageMix

*Fix image voxel information.*

Remove the useless hack that skipped some events in VTK picker interactor, so the information was not up-to-date.

### ExSimpleARCVOgre

*Add missing module_ui_media bundle.*

This solves the crash of ExSimpleARCVOgre at start. The bundle module_ui_media was missing.

### fwData/Image

*Deep copy.*

* Fixes an assert raised in debug when the image is locked and deep copied.
* Don't let the data array expire.
* Add a test to ensure this doesn't happen again.
* Fix ExDump.

### fwGdcmIOTest

*Test fails.*

Fixes fwGdcmIOTest by using correct type when setting Tags Values.

* ConsultingPhysicianName is a list
* Patient Size/Weight/MassIndex are double
* Attributes was missing when copying a Study

### fwGuiQt

*Allow notifications to work with ogre scenes.*

Fixes notifications for app that used Ogre scenes.
Moves the dialog according to the parent.

### fwLauncher

*Allow to exit when receiving signals.*

- Use regular exit() to stop program execution when receiving SIGINT, SIGTERM, SIGQUIT signals.

### fwMath

*Use glm vector when computing barycenters and fix mistake.*

* Replace all `fwVec3` structure by `::glm::dvec3`, along with glm maths functions.
* Fix mistake in dot product when computing barycentric coordinates.
* Update unit-test

### fwRenderOgre

*Fix query flags and ray mask.*

* Sets all pickable adaptors query flags to a default value ('::Ogre::SceneManager::ENTITY_TYPE_MASK').
* Sets pickers query mask to a default value ('::Ogre::SceneManager::ENTITY_TYPE_MASK').
* Query flags on adaptors are now sets, (previously, they was added).
* Updates documentations on updated files.

It allows pickers to pick all adaptors by default. This can be adjusted in xml by modifying the query mask of the picker, and the query flags of adaptors

### fwVtkIO

*Fix warnings (as error) when building with Clang.*

### gitlab

*Improve sheldon stage.*

Fix the build on dev, master and tags

### HybridMarkerTracking

*Fix tracking issues with strong distortion.*

Removing re-definition of member variable and undistort displayed image allows to correctly re-project tracking position on videos with strong distortions.

### IActivitySequencer

*Retrieve more data on update.*

The sequencer allows to store all data related to all activities in a member called `requirements`. When the sequencer checks an activity for the first time, it'll create it and add all it's needed data that already exist in the `requirement` list into the activity composite.

If we have two activities `A1` and `A2`, `A1` needs an image called `CT` and `A2` needs the same image, and also a matrix `MA` created by `A1`.

You launch A1 after adding its needed data in the wizard (`CT`), at this time, the sequencer stores `CT` in its `requirements` list.

So A1 is open, the sequencer will check if needed data for A2 have been loaded (Allows to enable a button in the view). To do that, `A2` is created and the sequencer adds the `CT` into the composite of `A2`, but `MA` is missing, nothing is launched.

Now `A1` has created the matrix `MA` and asks the sequencer to open the next activity, `A2`. The sequencer checks needed data, but `A2` is created, and the sequencer only updates data that are contained in the composite of `A2`.

* These modifications allow to update data and also to add potential new data generated by a previous activity (`IActivitySequencer::getActivity`).
* Now, before checking a next activity, the sequencer stores data of the current one (the current one has maybe created a new data).
* `IActivitySequencer`::parseActivities``returns the last valid activity. Before modifications, the next one was launched.

### Image

*Fix the iterator when using a different type than the image.*

The image iterator is now computed based on the format of the iterator instead of using the image type. You can now use Iterator on int16 image. It can be useful to fill the image with zeros. You can also parse int32 image with int64 iterator to gain performance.
`SLM_WARN` have been added when you use an iterator on a different type.

### ioDcmtk

*Fix the series DB reader and the CI last modification time of file.*

### ioIGTL

*Fix the module loading on windows.*

- Moves the module `ioNetwork` to SrcLib, it only contains interface. Furthermore some modules like `ioIGTL` links with it, and link between modules are prohibited.

### ioVTK

*Vtk Mesh readers and writers only support VTK Legacy files format.*

Add more Mesh format in VTK readers/writers.

* VTK Polydata (\*.vtp)
* Wavefront OBJ (\*.obj)
* Stereo lithography format (\*.stl)
* Polygonal File Format (\*.ply)

This was added to improve compatibility with other VTK software (paraview for example), it seems that the \*.vtk format is a legacy format and we should use \*.vtp when dealing with polydata (our ::fwData::Mesh).

**Note:** when using OBJ/PLY/STL format you may loose some informations (point normals/point color/...), often it saves only global geometry of the data (points, edges, ...).

### IParameter

*Add missing makeCurrent in IParameter.*

Add missing `makeCurrent` in `fwRenderOgre/IParameter`

### ModelSeries

*Add const in model series helper.*

Update `::fwMedDataTools::ModelSeries`to add const on shared pointer parameters. It allows to call the helper with const sptr.

### OgreVolumeRendering

*Fix the ambient occlusion.*

* Fix a crash in `SummedAreaTable` due to bad storage (when `m sat Size[0]` is 256, the old `std:uint8_t` stores 0).
* Fix a crash when the software is closed.

### openVSlamIOTest

*Add missing dependencies for link.*

This adds ffmpeg and Qt as explicit dependencies. There is clearly a bug in the OpenCV target but until this is fixed upstream, we can use this solution.

### openvslamTracker

*Do not download the vocabulary file each time.*

Check the file hash to prevent it to be downloaded on each CMake configuring step.

### RayTracingVolumeRenderer

*Correctly blend the volume with the scene behind it.*

* Fix alpha blending between the volume and the scene behind it.
* Fix z-fighting between the negato picking widget and the negato plane when OIT is enabled.
* Fix z-fighting between the volume and the negato planes.

### SActivitySequencer

*Qml import path is not properly defined.*

Check if a `qml` folder exist on the application root folder (for installed application) and use the conan path otherwise.

### SArucoTracker

*Doesn't display marker if image components < 4.*

Test the number of components of the input image before doing OpenCV conversion.

With this modification we can now handle both 3 and 4 components images.

### SCalibrationInfoReader

*Don't convert the image if loading failed.*

Fix a crash when loading a folder containing files not readable as images.

### SeriesDBReader

*Skip structured report and improve the activity data view.*

When you load a DICOM folder that contains a `SR` modality with the reader ``::vtk`Gdcm`::Series`FBReader`, it throws an exception. Now it's skipped with a message.

The activity data view is also improved in this MR.

### SFrameMatrixSynchronizer

*Use new image API.*

* Fix the issue when the number of components are not properly reset (#423)
* Add a missing write lock on matrices.
* Use the new image API

### SFrameWriter

*Add missing else statement.*

### Sight

*Fail to compile with Clang.*

This adds the AES flag for Clang, allowing to build fwZip successfully.

Initially, the MR was opened to fix this issue but actually many targets failed to compile with Clang, thus other minor fixes are included.

### SMatricesReader

*Fix default path.*

Fix the default path in `::ioTimeline::SMatricesReader`. The default path must be a folder, so we use the parent path of the selected file to set the default reader path. It allows to re-open the file dialog in the same folder.

### SMatrixWriter

*Add fixed floating point format and set float precision.*

Set floating precision fixed to 7 digits.

### SMesh

*Fix recursive read lock.*

Update ``::visuOgreAdaptor::SMesh``to remove a read lock in an internal method and only call it in the slots. It fixes a random freeze when starting the adaptor in a large application.

*Fix auto reset in mesh adaptor.*

Fix auto reset camera in `::visuOgreAdaptor::SMesh`:

* call auto reset when the vertices are modified
* call request render when the adaptor is started

### SNegato2DCamera

*Modify camera position in function of the image.*

Modify the AutoReset camera method to reset the camera position in function of the image and not the world position.
If there is other adaptors than SNegato2D, the scene will be autoreset with the position of the image. It avoids some strange scaling due to other adaptors.

### SNegato3D

*Properly use the ITransformable interface.*

Properly use the ITransformable interface in SNegato3D.

### spylog

*Reduce usage of FW_DEPRECATED Macro.*

Small cleanup of usage of `FW_DEPRECATED` macros.

* Macros was removed from widely used functions
* keyword `[[deprecated]]` was added where it was missing

### SText

*Fix text configuration.*

The `text` configuration was wrong when parsing xml file.

### STransferFunction

*Avoid assert when icons path are not initialized.*

- Fix crash when icons path are not initialized in `StransferFunction` service.
- When icons path were not defined, we have this assert: `Assertion 'path.is_relative()' failed. Path should be relative`

### STransform

*Remove or add the node instead of change the visibility.*

Show or hide the transform node directly impact attached entities and services like `::visuOgreAdaptor::SMesh`, that can't manage its visibility since STransform change it any time.
Now, the node is just removed or added from/to its parent to avoid a visibility error.

### STransformEditor

*Add missing lock.*

Add missing mutex lock in `uiVisuQt::STransformEditor`

### SVector

*Fix the visibility update of SVector.*
* Add two slots to show and hide the vector

### SVideoWriter

*Computes video framerate.*

* Removes the default framerate value hardcoded to 30 fps in SVideoWriter
* Computes framerate using the timestamps of the first frames in the timeline

### SVolumeRender

*Fix usage of transfer functions.*

* Create a new service `uiTF`::STransferFunction``instead of `uiTF`::TransferFunctionEditor``(it also fix the swapping method and lock data).
* Improve the TF usage in `visuOgreAdaptor::SVolumeRender`.
* Set the sampling rate at starting of `visuOgreAdaptor::SVolumeRender`.
* Fix the `GridProxyGeometry` by avoid uniforms sharing between programs.
* Fix the `makeCurrent` method.

*Fix a crash with the volume render.*

*Double lock when updating the volume sampling.*

### test

*IoVTKTest randomly crashes.*

Fix ioVTKTest random crash. Sometimes writer doesn't load the reconstructions in the same order than the reader saved it.

We need to force writer to load file in a specific order:

* prefix reconstruction filename by its index in reconstructionDB (like "0_Liver")
* sort filenames by alphabetical order, using the previous prefixed index, to ensure that reconstructions are loaded in the same order than the generated ones.
* add messages in CPPUNIT assert marco to help to find failing tests.

### TutoTrianConverterCtrl

*Console application cannot be launched with parameters.*

Forward arguments of .bat/.sh scripts to fwLauncher.

### videoVLC

*Fix VLC plugins dir.*

* use current working path to find VLC plugins folder in `videoVLC` for installed applications
* fix cmake install target for apps using libvlc:
  * on Windows (with MSVC2019) VLC is now manually installed to avoid `fatal error LNK1328: missing string table` in fixup_bundle
  * on linux vlc plugins path is now analysed by the fixup_bundle script to find required dependencies
* update VLC conan package to the version 3.0.6-r5

### visuOgreAdaptor

*Properly generate the r2vb in SMesh.*

### visuOgreQt

*Intel mesa regressions.*

* Fix regressions when using intel mesa GPU following the context handling MR (!249)
* Handle buffer swapping manually instead of relying on ogre.

*Remove unused variables.*

* Fixes a warning on the CI preventing sight from building.
* Removes an unused parameter in `visuOgreQt::Window`.

*Always force rendering when the window is exposed.*

* Fix the call to `renderNow` in `visuOgreQt/Window`
* Request render when changing visibility parameters in `ExSimpleARCVOgre`

*Send mouse modifiers as keyboard events.*

* Take mouse modifiers into account for mouse events.
* This is a quick fix and should be refactored later on.

## Documentation:

### fwData

*Improve doxygen of Image, Mesh and Array.*

* add examples, fix some mistakes.
* improve `Image::at(x, y, z, c)` to add the component parameter, it allows to retrieves the value of an element of a image with components

### README

*Remove broken install links.*

Remove broken links pointing to old documentation pages.

### visuOgreQt

*Clean the bundle.*

Cleans the `visuOgreQt` bundle.

## Refactor:

### Boost

*Replace boost use by C++17 new features.*

Replace usage of Boost by **standard library** versions of:
- `boost::filesystem`
- `boost::make_unique`
- `boost::any`
- `boost`::assign``(most of them, some were left because of specific boost containers such as bimaps)

Also, `fwQtTest` failed a lot during testing, so a fix has been proposed. It should no longer fail now.

### CMake

*Rename THOROUGH_DEBUG option.*

Rename `THOROUGH_DEBUG` into `SIGHT_ENABLE_FULL_DEBUG`

### deprecated

*Remove last remaining deprecated classes and methods for sight 20.0.*

* Remove deprecated class Bookmarks
* Remove deprecated class AttachmentSeries and the associated reader and converter class (atom patches have been kept to support the loading of old data).
  * it implies the increment of MedicalData version to V14 and V16AR
* Remove the stop of the worker in its destructor
  * it implies to properly stop the workers before to clear the worker registry (::fwThread::ActiveWorkers). In fact, the workers should be stopped by the classes that created them, but as the API to remove a worker from the registry does not exist, it will be done in #521.
  * it also implies to stop manually the workers used in the unit tests and not managed by the registry

*Remove the use of deprecated Image, Mesh and Array API.*

* Remove the last remaining usage of the deprecated API of Image, Array and Mesh outside of fwData and fwDataTools: add a `2` after get/setSize, get/setOrigin, get/SetSpacing.
* Fix dump lock on Image and Array iterators: lock must be called before accessing the buffer.
* Improve the documentation about the dump lock.

*Use new Image, Mesh and Array API.*

Refactor some services and libraries to use new Image, Mesh and Array API:

* refactor `igtlProtocol` library to use new API
* refactor `fwCommand` library to use new API
* remove useless helpers includes
* fix MeshIterator: set 'operator=()' as virtual and remove redundant cast

### filetree

*Rename root folders.*

All root folders were renamed, and first of all, Bundles into modules. We also chose to stick to lower-case only and so we renamed all existing folders to their lower counterpart. Besides this SrcLib was shortened into libs and Utilities in utils.

Here was the old file tree:

| Apps
| Bundles
| CMake
| fwlauncher
| Samples
 \ Examples
  | PoC
  | Tutorials
| SrcLib
| Utilities

And here is the new one:

| apps
| cmake
| fwlauncher
| libs
| modules
| samples
 \ examples
  | poc
  | tutorials
| utils

### fwData

*Clean fwData and fwTest with new API of Image, Mesh and Array.*

* Update `config.hpp.in` to add `<PROJECT_NAME>_DEPRECATED_CLASS_API`, it allows to set the `deprecated` attribute on a class (display a compilation warning).
* Update `fwTest` and `fwData` libraries to remove the dependency to `fwDataTools`
* Clean `fwDataTools` to remove the use of deprecated helpers, but the helpers are still here.
* fix SImagesBlend adaptor: fix size, spacing and origin comparison

*Use new Image, Mesh and Array API in uiXXX Bundles.*

* Use new Image, Mesh and Array API in uiXXX bundles
* Improve `ioVTK` and `fwVTKIO` unit tests
* Fix `fwTest` Image generator (number of components was not properly set)
* Fix `::fwData::Image`, add a missing const on `getPixelAsString` method

*Improve the API of `::fwData::Array`.*

Simplifies Array API by moving the methods from the helper to the data itself:

* deprecate `::fwDataTools::helper::Array`
* integrate the useful methods from the helper to `::fwData::Array`
* deprecate the component attribute
* add `Iterator<Type>` and `ConstIterator<Type>` to iterate through the array buffer

### fwDataTools

*Use the new Mesh API.*

* Refactor Mesh iterator to access values on a const variable.
* Use the new Mesh API in ``::fwDataTools::Mesh``to compute normals and improve unit tests.
  * Also fix the normals: when the cell contains more than 3 points, only the last 3 points were used to compute the normal.
* Improve Image ans Array documentation.

### fwRenderOgre

*Use the new API for Mesh, Image and Array.*

Refactor `fwRenderOgre` and `uiVisuOgre` to use new Image, Array and Mesh API.

*Clean the module_ui_media folder.*

Sorts glsl, metarial and compostior.

### fwRuntime

*Remove usage of deprecated methods.*

* This removes the usage of all deprecated methods by !282
* This removes the usage of the term `bundle` in local variables, private functions, comments.

*Rename 'Bundle' into 'Module'".*

The first intent of this MR is to rename the term 'Bundle' into 'Module'. As stated in #404, 'Bundle' is not a term widely spread in software for the meaning we give to it. More often, software use either the term 'Module' or 'Plugin'. Late in summer 2019, we decided to choose 'Module'.

However we did not want that change to be a breaking change. So as usual, during two major versions, 'Bundle' and 'Module' terms are likely to coexist and the old API should be deprecated but maintained. Most code mentioning 'Bundle' lies in `fwRuntime`. I realized a lot of code present in that library was never used outside. It would have been useless to double every method and class for non-public code.

I've been keeping saying we do not clearly separate public and private API in our libraries. I decided so to try to achieve that in fwRuntime. After doing this, the only deprecation of the public API will be faster.

To separate the private and the public API in _Sight_ **libraries**, I propose to add `detail` folders both in `include` and `src` folders. Symbols in `detail` folders will be hidden/not exported, thus reducing the size of the shared library and speeding up debugging (when doing this at a large scale of course). This has many advantages in terms of readability of code, maintenance (like this deprecation), etc...

However, one drawback is that since symbols are not exported, it was no longer possible to unit-test private classes and methods. To overcome this, I propose to compile libraries in two steps. One `OBJECT_LIBRARY`, then the usual `SHARED_LIBRARY`, that uses the previous as input of course. This way, the unit-test can build directly with the `OBJECT_LIBRARY` (so the precompiled .obj) directly, removing the need of export, and allowing the test of private classes and methods.

**But**, this was not that simple. In our tests that are often more functional tests than unit-tests, we face two issues:
- we have circular dependencies between libraries, so the test may try to link against both the OBJECT_LIBRARY and the SHARED_LIBRARY, causing awful runtime errors,
- we use lots of static singletons to register types, factories, etc... The same singleton may be instanced both in the OBJECT_LIBRARY and the SHARED_LIBRARY, ending up with a duplicate and not a singleton.

The first issue can be tackled, I tried at some point, but it was quite hard, especially as soon as we load modules. The second is even harder sometimes. At the end I chose an alternative. I propose to split tests in two. One for the public API (ex: `fwRuntimeTest`) and one for the implementation (ex: `fwRuntimeImplTest`). The implementation test should be much more minimal and should not require many dependencies, thus reducing the possibility of these two problems to occur. At least that's my hope. :candle:

### fwServices

*Remove the old API from fwServices.*

Remove all the deprecated methods from IService and its associated helpers and registries.
Remove IService default object.

**Note**: the default auto-connection on the first service's object to the 'modified' signal is removed. Check your log to know if your auto-connections are connect.

### guiQt

*Improve SParameters.*

* Fix a bad call to `m_illum`.
* Allow to avoid the automatic update of `SParameters`.
* Add option in `SParameters` to hide the reset button.

### Image

*Improve the API of ::fwData::Image.*

Improve the API of `::fwData::Image`:

* Add the pixel format information: GRAY_SCALE, RGB, RGBA, BGR, BGRA
  * Add an iterator for the different formats
  * the format involve the number of components
* Prevent to use more than 3 dimensions: the size, spacing and origin are defined in a `std::array`
  * 4D images are no longer allowed
* The ``::fwData::Array``is no longer accessible from the image
* Update Dispatcher to use ``::fwTools::Type``instead of ``::fwTools::DynamicType``and deprecate DynamicType.
* **Most** of the old API is still supported with deprecated warning:
  * the new getter/setter for size, origin and spacing are temporary post-fixed by `2` (ex: getSize2())
* Increase the version of Medical Data to serialize the image format (V12, V14AR, V16RD)
* Update `WARNINGS_AS_ERRORS` macro to allow deprecated warnings.

Example:

```cpp
`::fwData::Image::sptr`img = ::fwData::Image::New();

img->resize({1920, 1080}, ::fwTools::Type::s_UINT8, ::fwData::Image::PixelFormat::RGBA);

typedef `::fwData::Image::RGBAIteration`RGBAIteration;

auto lock    = img->lock(); // to prevent buffer dumping
auto iter    = img->begin<RGBAIteration>();
const auto iterEnd = img->end<RGBAIteration>();

for (; iter != iterEnd; ++iter)
{
    iter->r = static_cast<std::uint8_t>(rand()%256);
    iter->g = static_cast<std::uint8_t>(rand()%256);
    iter->b = static_cast<std::uint8_t>(rand()%256);
    iter->a = static_cast<std::uint8_t>(rand()%256);
}
```

### IO

*Use the new Image, Mesh and Array API.*

- Use the new Image, Array and Mesh API in readers and writers.
- Fix Mesh iterators to properly access the values, even if the variable is const.
- Update Image to add the missing API for lazy readers : it requires access to the buffer object to read streams.

*Use the new Image, Mesh and Array API.*

- Refactor fwVtkIO, fwItkIO, igtlProtocol and associated bundles to use the new Image, Mesh and Array API.
- Fix Image iterator: the end of the const iterator was not correct.
- Improve mesh iterator to allow point and cell colors with 3 components (RGB).

### iterator

*Improve iterator to access values.*

- Update Image and Array iterators to allow to access values on const iterators.
- Refactor Image iterator to remove complicated format and only use simple struct.
  - removed warnings when getting an iterator on a type different from the array type. It allows to iterate through a multiple value structure at the same time.

Now, to iterate through an Array or an Image, you can use a struct like:

```cpp

struct RGBA{
    std::uint8t r;
    std::uint8t g;
    std::uint8t b;
    std::uint8t a;
}
```

Then:

```cpp

`::fwData::Image::sptr`image = ::fwData::Image::New();
image->resize(125, 125, 12, ::fwTools::Type::s_UINT8, ::fwData::Image::RGBA);

auto itr = image->begin< RGBA >();
const auto itrEnd = image->end< RGBA >();

for (; itr != itrEnd ; ++itr)
{
    itr->r = 12.0;
    itr->g = 12.0;
    itr->b = 12.0;
    itr->a = 12.0;
}
```

### module_ui_media

*Fuse all module_ui_media bundles.*

* Deprecate `arMedia` bundle.
* Copy all `arMedia` content into `module_ui_media`.
* Fixes xml files that used these bundles.

### Mesh

*Improve the API of ::fwData::Mesh.*

Refactor the mesh API:
- deprecate the access to the points, cells and other arrays
- rename allocate method to reserve
   - allow to allocate the color, normal and texture arrays in the same reserve method
- add resize method to allocate the memory and set the number of points and cells
- add iterator to iterate through the points and cells

**Allocation**:
The two methods `reserve()` and `resize()` allow to allocate the mesh arrays. The difference between the two methods is
that resize modify the number of points and cells.

- `pushPoint()` and `pushCell()` methods allow to add new points or cells, it increments the number of points and allocate the memory if needed.
- `setPoint()` and `setCell()` methods allow to change the value in a given index.

Example with `resize()`, `setPoint()` and `setCell()`:

```cpp
    `::fwData::Mesh::sptr`mesh = ::fwData::Mesh::New();

    mesh->resize(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock();

    for (size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val                                            = static_cast<uint8_t>(i);
        const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
        const float floatVal                                              = static_cast<float>(i);
        const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
        const size_t value                                                = 3*i;
        mesh->setPoint(i, static_cast<float>(value), static_cast<float>(value+1), static_cast<float>(value+2));
        mesh->setPointColor(i, color);
        mesh->setPointNormal(i, normal);
        mesh->setPointTexCoord(i, texCoords);
    }

    for (size_t i = 0; i < NB_CELLS; ++i)
    {
        mesh->setCell(i, i, i+1, i+2);

        const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
        const float floatVal                                              = static_cast<float>(i);
        const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
        const size_t value                                                = 3*i;
        mesh->setCellColor(i, color);
        mesh->setCellNormal(i, normal);
        mesh->setCellTexCoord(i, texCoords);
    }
```

Example with `reseve()`, `pushPoint()` and `pushCell()`

```cpp
    `::fwData::Mesh::sptr`mesh = ::fwData::Mesh::New();

    mesh->reserve(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock();

    for (size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val                                            = static_cast<uint8_t>(i);
        const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
        const float floatVal                                              = static_cast<float>(i);
        const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
        const size_t value                                                = 3*i;
        const size_t value                                   = 3*i;
        const auto id =
            mesh->pushPoint(static_cast<float>(value), static_cast<float>(value+1), static_cast<float>(value+2));
        mesh->setPointColor(id, color);
        mesh->setPointNormal(id, normal);
        mesh->setPointTexCoord(id, texCoords);
    }

    for (size_t i = 0; i < NB_CELLS; ++i)
    {
        const auto id = mesh->pushCell(i, i+1, i+2);

        const `::fwData::Mesh::ColorValueType`val                          = static_cast< `::fwData::Mesh::ColorValueType`>(i);
        const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
        const float floatVal                                              = static_cast<float>(i);
        const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
        const size_t value                                                = 3*i;
        mesh->setCellColor(id, color);
        mesh->setCellNormal(id, normal);
        mesh->setCellTexCoord(id, texCoords);
    }
```

**Iterators**

To access the mesh points and cells, you should uses the following iterators:
- `::fwData::iterator::PointIterator:`to iterate through mesh points
- `::fwData::iterator::ConstPointIterator:`to iterate through mesh points read-only
- `::fwData::iterator::CellIterator:`to iterate through mesh cells
- `::fwData::iterator::ConstCellIterator:`to iterate through mesh cells read-only

Example to iterate through points:

```cpp
    `::fwData::Mesh::sptr`mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, ::fwData::Mesh::TRIANGLE);
    auto iter    = mesh->begin< `::fwData::iterator::PointIterator`>();
    const auto iterEnd = mesh->end< `::fwData::iterator::PointIterator`>();
    float p[3] = {12.f, 16.f, 18.f};

   for (; iter != iterEnd; ++iter)
   {
       iter->point->x = p[0];
       iter->point->y = p[1];
       iter->point->z = p[2];
   }
```

Example to iterate through cells:

```cpp
    `::fwData::Mesh::sptr`mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, ::fwData::Mesh::TRIANGLE);
    auto iter         = mesh->begin< `::fwData::iterator::ConstCellIterator`>();
    const auto endItr = mesh->end< `::fwData::iterator::ConstCellIterator`>();

    auto itrPt = mesh->begin< `::fwData::iterator::ConstPointIterator`>();
    float p[3];

    for(; iter != endItr; ++iter)
    {
        const auto nbPoints = iter->nbPoints;

        for(size_t i = 0 ; i < nbPoints ; ++i)
        {
            auto pIdx = static_cast< `::fwData::iterator::ConstCellIterator::difference_type`>(iter->pointIdx[i]);

            `::fwData::iterator::ConstPointIterator`pointItr(itrPt + pIdx);
            p[0] = pointItr->point->x;
            p[1] = pointItr->point->y;
            p[2] = pointItr->point->z;
        }
    }
```

`pushCell()` and `setCell()` may not be very efficient, you can use `CellIterator` to define the cell. But take care to
properly define all the cell attribute.

Example of defining cells using iterators

```cpp
    `::fwData::Mesh::sptr`mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, ::fwData::Mesh::QUAD);
    auto it          = mesh->begin< `::fwData::iterator::CellIterator`>();
    const auto itEnd = mesh->end< `::fwData::iterator::CellIterator`>();

    const auto cellType = ::fwData::Mesh::QUAD;
    const size_t nbPointPerCell = 4;

    size_t count = 0;
    for (; it != itEnd; ++it)
    {
        // define the cell type and cell offset
        (*it->type)   = cellType;
        (*it->offset) = nbPointPerCell*count;

        // /!\ define the next offset to be able to iterate through point indices
        if (it != itEnd-1)
        {
            (*(it+1)->offset) = nbPointPerCell*(count+1);
        }

        // define the point indices
        for (size_t i = 0; i < 4; ++i)
        {
            `::fwData::Mesh::CellValueType`ptIdx = val;
            it->pointIdx[i] = ptIdx;
        }
    }
```

### MeshIterator

*Cast cell type into CellType enum instead of using std::uint8_t.*

When iterating through mesh cells, the iterator return the type as a CellType enum instead of a std::uint8_t. It allow to avoid a static_cast to compare the type to the enum.

### OgreViewer

*Recreate the whole application.*

* Cleans `visuOgreAdaptor`.
  * Cleans the documentations of all adaptors and fix it for some of them.
  * Draws border of negatos and allow to enable/disable them from the xml.
* Creates a new `OgreViewer`.
* Adds a missing documentation in `LineLayoutManagerBase`.
* Creates a xml color parser.
* Fixes the fragment info adaptor by listening the viewport instead of the layer.
* Improves the slide view builder

### RayTracingVolumeRenderer

*Separate ray marching, sampling, compositing, and lighting code.*

* Simplifies the volume ray tracing shader to make it more understandable.
* Reduces macro definition combinations.

### resource

*Fuse all module_ui_media bundles.*

* Adds `arMedia` and `module_ui_media` into a folder `resource`.
* Adds a new bundle `flatIcon` with flat theme icons.

### SNegato2DCamera

*Improve 2D negato interactions.*

* Add a new adaptor to replace the `Negato2D` interactor style.
* Precisely zoom on a voxel using the mouse cursor.
* Deprecate `::visuOgreAdaptor::SInteractorStyle`.
* Update OgreViewer to use the new adaptor.
* Decouple camera management from the negato adaptor.
* Fix interactors to work within a viewport smaller than the whole render window.
* Fix the camera's aspect ratio when the viewport width and height ratios are not the same.

### STrackballCamera

*Move the trackball interaction to a new adaptor.*

* Decouples the trackball interactor from the layer.
* Moves trackball interactions to a new service.
* Makes 'fixed' interactions the default.
* Deprecate parts of the `SInteractorStyle`.

### Tuto

*Use new API of Image, Mesh and Array.*

Refactor some services and libraries to use new Image, Mesh and Array API:

* Refactor Tuto14 and Tuto16 specific algorithms to use the new API
* Refactor `scene` to use new API and remove some warnings
* Refactor `visuVTKAdaptor` to use new API
* Refactor `itkRegistrationOp` library to use new API

### video

*Video use new data api.*

Refactor some services and libraries to use new Image, Mesh and Array API:

* Refactor `videoCalibration`
* Refactor trackers, frame grabbers
* Refactor `cvIO`: conversion between openCV and sight

### visuVTKAdaptor

*Use the new Image, Mesh and Array API.*

* update `fwDataTools` to remove the last uses of the deprecated API except the helpers
* refactor `visuVTKAdaptor` `vtkSimpleNegato`and `vtkSimpleMesh` to remove the deprecated API
* refactor `opImageFilter` and the associated library to remove the deprecated API
* fix `::scene2D`::SComputeHistogram``due to a missing include and remove the deprecated API
* fix ``::fwRenderOgre::Utils``due to a missing include and remove the deprecated API for the image conversion

### VRWidgetsInteractor

*Move the clipping interaction to a separate class.*

* Refactors vr widget interaction. Splits `VRWidgetsInteractor` into `TrackballInteractor` and `ClippingBoxInteractor` and deprecates it.
* Adds right mouse button interaction to the trackball.
* Renames `VRWidget` to `ClippingBox`.

## New features:

### ActivityLauncher

*Update the activityLauncher configuration to change icons paths and readers configuration.*

Allow to customize the activity wizard used by the sequencer. You can use different icons and/or define the readers to use.

* Add a parameter in ActivityLauncher configuration `WIZARD_CONFIG` to define the custom configuration.
* Update ExActivities sample to customize the wizard.

### ARCalibration

*Load calibration input image folders.*

* Add a service to load image folders with calibration inputs.
* Move the detection method to the `calibration3d` library.

*Live reprojection for intrinsic calibration.*

* Compute the reprojection error for each new frame following the calibration.
* Display the reprojected points and the reprojection error.
* Display the detected points.
* Add the ability to undistort the video images once the calibration is computed.
* Modify SSolvePnP to always update the camera parameters.

### build

*Embed PDB file when installing Sight in debug mode on Windows platform.*

Add `install(FILES $<TARGET_PDB_FILE:${FWPROJECT_NAME}> DESTINATION ${CMAKE_INSTALL_BINDIR}/${FW_INSTALL_PATH_SUFFIX} OPTIONAL)`

### ci

*Build sight-sdk-sample with the SDK.*

* Refactor a bit linux release and debug jobs to build in SDK mode and install/package the SDK. The SDK is an artifact of the job and passed to other dependent jobs
* Add jobs that use the SDK artifact and unpack and use it to build sight-sample-sdk
* The SDK artifact can be used to manually deploy the SDK on artifactory

*Use LFS repo for sight-data.*

This replaces the usage of sight-data with a new repository with LFS support. This speed-ups the download time because git LFS can parallelize batch downloads. This is particularly useful for the CI:
- Previously we used `curl` to retrieve the archive then `tar` to decompress the archive, **the whole process took around 170s**.
- The new `git clone` commands, **this takes only 45s**.

For some ci tasks like deploying issues and merge requests templates, we will use `GIT_LFS_SKIP_SMUDGE=1` which allows to skip completely the download of binaries.

*Remove macos support.*

* remove macos jobs in gitlab-ci script
* add warning message to inform users that macos is no longer supported

### cmake

*Conan update and compilation flag sharing.*

* Update ogre to patched version 1.12.2
  ** Match shader input/ouput attribute names in vertex/geometry/fragment shaders.
  ** Fix varying parsing for the volume proxy bricks shaders.
  ** Replace the deprecated scene node iterator.
  ** Remove the debug plugin configuration on windows.
  ** Explicitly convert quaternions to matrices.
  ** Fix normal rendering.

* Add Visual studio 2019 support with conan package available on artifactory. All conan packages have been updated

* Share "optimized" debug compiler flags across all conan package and Sight. Some previous work on array API (and in many place in our code or our dependencies) showed very bad performance in debug. To mitigate this, we want to use optimized debug build with `-Og -g` (unix) or `/Ox /Oy- /Ob1 /Z7 /MDd` (windows) which will effectively make the speed almost reach release build, while being "debuggable".

  The drawbacks are:
  ** the build will take a bit longer (max 10% longer on gcc/clang, 30% on MSVC)
  ** some lines may be "optimized" out
  ** we loose /RTC1 on windows

  To allow "full" debugging on Sight, we plan to add a special option to use regular flags.

A new option `THOROUGH_DEBUG` has been added to allow full debugging without optimization, if needed. On windows it also add /sdl /RTC1 which performs additional security checks.

Also be sure to have the latest version of conan and that you use the latest `settings.yml` on Windows or macOS (sometimes there is a `settings.yml.new` and you need to rename it `settings.yml`). On Linux, Sight automatically download an updated `settings.yml`, but you can still update conan.

To update conan: `pip3 install --upgrade conan`

*C++17 support.*

This updates our Sight build to support C++17 standard.

Some modifications were also needed from some of our dependencies:
- Camp (patched upstream)
- ITK (updated to 5.0.1)
- Flann (imported patch from upstream)
- Sofa (updated to 19.06.01 and patched locally)

### conan

*Update Qt, PCL, Opencv & Eigen.*

*Update conan package to support CUDA 7.5 arch.*

This is mainly for supporting correctly NVIDIA RTX GPU

### conan-deps

*Support Visual compiler 16 for Visual Studio 2019.*

Now Sight can be compiled with the latest Visual compiler 16 provided
with Visual Studio 2019.

### core

*Use RAII mechanism and weak_prt / shared_ptr pattern to protect fwData against race condition and memory dumping.*

Three class has been added: `weak_ptr`, `locked_ptr`, `shared_ptr`. Each of them will "store" the pointer to the real data object, but in a different manner and with a different way to access it.

You will receive a `weak_ptr` when calling `IServices::getWeak[Input|Inout|Output]()` and a a `locked_ptr` when calling `IServices::getLocked[Input|Inout|Output]()`

weak_ptr will use a hidden `std`::weak_ptr``to hold the real data, but it can only be accessed by "locking" it through a `locked_ptr`. `locked_ptr` will hold a `std::shared_ptr`, but also a mutex lock to guard against concurrent access and a buffer lock to prevent dumping on disk when the data object have an array object. RAII mechanism will ensure that everything is unlocked once the `locked_ptr` is destroyed. The underlying lock mutex will be a **write** mutex locker if the pointer is **NOT const**, a **read** mutex locker if the pointer **is const**. Using an `Input` will anyway force you to have const pointer, forcing you to use a **read** mutex locker. You can simply get the original data object through a std`::shared_ptr`by calling `locked_ptr::getShared()`

*Simplifies macros in fwCore and enables WARNINGS_AS_ERRORS in some core libs.*

* Warnings in Sight core was mainly due to the call of macro with fewer parameters than required.
* We have now a macro `fwCoreClassMacro` with three versions:
  * fwCoreClassMacro(_class);
  * fwCoreClassMacro(_class, _parentClass);
  * fwCoreClassMacro(_class, _parentClass, _factory);
* The macro `fwCoreServiceClassDefinitionsMacro` used in services has been simplified and replaced by `fwCoreServiceMacro(_class, _parentClass);`
* All old deprecated macros have been placed in the `macros-legacy.hpp` file (still included by `macros.hpp` for now - until **Sight 22.0**)
* List of new projects that no longer generate warnings during compilation (new cmake option `WARNINGS_AS_ERRORS` enabled):
  * arData
  * arDataTest
  * fwCom
  * fwComTest
  * fwCore
  * fwCoreTest
  * fwData
  * fwDataTest
  * fwMedData
  * fwMedDataTest
  * fwRuntime
  * fwRuntimeTest
  * fwServices
  * fwServicesTest
  * fwTools
  * fwToolsTest
  * fwRenderOgre
  * fwRenderOgreTest

### ctrlPicking

*Add a new bundle for picking operation services.*

* merge `::uiVisuOgre::SAddPoint`, ``::echoSimulation::STransformPickedPoint``and ``::uiMeasurement::SManageLandmarks``into `::ctrlPicking::SManagePoint`
* Allow to have a maximum number of points in a pointlist via `::opPicking::SAddPoint`.
* Allow to avoid points to be removed in `::ctrlPicking::SManagePoint`.
* Deprecate `::uiVisuOgre::SAddPoint`.
* Deprecate `::uiMeasurement::SManageLandmarks`.

### debian

*Build sight on Debian-med.*

Added several patches to build sight on Debian-med

* fix launcher library path
* fix version getter: in debian workflow, Sight version should be passed from the build parameters and not from the git repository
* do not install conan deps since we use system libraries
* add missing copy constructor for fwData`::ImageIterator`(new warning in gcc9)
* fix support of VTK 7
* remove redundant move in return statement in fwCom \[-Werror=redundant-move\]
* fix build flag for project using system lib
* add a new debian gitlab-ci build job
* fix the source path of dcmtk scp config file

### ExActivitiesQml

*Implement activities for Qml applications.*

Create a Qml sample to launch activities: `ExActivitiesQml`

* Create base class for activity launcher and sequencer service to share the code between qml and qt services.
* Move qml style from `guiQml` to `style` bundles, it is required because some bundles require it and we don't want to start guiQml bundle before them.
* Improve `AppManager` to manage input parameters and to generate unique identifier
* Create an Qml object `ActivityLauncher` to help launching activities in a Qml application

### flatdark

*Update flatdark.qss to add style for persia.*

Modify `flatdark.qss` to add specific style for `Persia` application.

### fwData

*Allow locked_ptr to work with nullptr.*

- If a data is optional, it can be null. But the AppConfig manager try to get a locked_ptr of the data, and the locked_ptr try to lock a nullptr, so an exception is thrown. Now, locked_ptr allows nullptr.

- Fix a crash when clicking on `Reinitialize` or `Delete` in the Volume TF editor.

*Get/set a 4x4 Matrix from TranformationMatrix3D.*

- Add new functions to get/set a TransformationMatrix3D from/to a 4x4 Matrix (array of array). This ensure the row major order and avoid linear to 2d conversions before setting coefficients.

### fwGui

*Manage more gui aspects.*

* Manage label of SCamera.
* Remove a "pading" warning in flatdark.qss.
* Add tooltips on button in the WindowLevel.
* Change the color of disabled QLabel.
* Allow to set the size of each borders in layouts, and manage spacing between widget in the same layout.
* Fix the icons aliasing in the wizard.
* Fix the opacity in `fwGuiQt`

*Manage inverse actions.*

Handle the inverse actions behavior.

When the inverse is set, it means that the behavior of toolbar button (or menu item) is inverted. It is like if it's internal state return false when it is checked and true when unchecked.

*Add parameter to specify the tool button style in ToolBar.*

Enable to display the text beside or under the icons in a toolBar.

### fwGuiQt

*Allow to set a background color on toolbars, menus and layouts.*

### fwPreferences

*Add password management capabilities.*

This MR allow to specify a password, retrieve it from memory (it is stored in a scrambled form) and compare its sha256 hash to the one stored in preferences. This password is then used by SWriter and SReader to write and read encrypted data.

The code rely on fwPreferences helper and should be accessible from anywhere as static variable is used to hold the password in memory. This functions should be thread safe.

### fwRenderOgre

*Simplify fullscreen toggling.*

* Make fullscreen toggling easier and more efficient.
* Add slots to enable/disable fullscreen on the render service.
* Add an action service to select a screen for fullscreen rendering.
* Add a shortcut to return to windowed mode.
* Fix bundle linking in `uiVisuOgre`.

*Configurable viewports.*

* Configure viewports for each layer.
* Refactor the overlay system to enable overlays per viewport (layer).
* Fix adaptors to render text using the new overlay system.

*Allow pickers to pick both side of meshes.*

Take into account the culling mode of entities for the picking.
It allows to know which sides of the triangle to pick.

See Sight/sight#373

### fwRenderOgre/Text

*Crisp font rendering.*

* Improve font rendering by taking into account the dpi and rendering text at the font size.
* Update the dpi on text objects when switching to a screen with a different DPI.
* Handle vertical alignment for texts.
* Update all adaptors to use this new font rendering method.

### fwServices

*Raise an exception when IService::getLockedInput() return a NULL locked_ptr.*

Throw an exception when `IService::getLockedXXX()` retrieve a NULL data object.

### fwZip

*Add "encryption" support when reading and writing jsonz archives.*

- add encryption and update internal minizip to version "1.2".

### gui

*Add notification popups in sight applications.*

* Notification can be displayed at 7 fixed positions (from top-left to bottom right including top & bottom centers).
* 3 Types of notifications: INFO, SUCCESS, FAILURE (background color changes).
* Notifications appears / disappears with a fade-in/out effect on opacity.
* Two ways of displaying notifications:
  * Centralized by the new `SNotifier` service:
    * Using new IService signals `infoNotified`, `successNotified` and `failureNotified` connected to SNotifier slots `popInfo`, `popSuccess`, `popfailure`.
    * `SNotifier` can queue multiple notifications, if queue is full the oldest one is removed.
    * `SNotifier` handles also position of the notification per application/config (ex: always at TOP_RIGHT).
  * Or notification can be displayed by calling directly `::fwGui::NotificationDialog::show()`, loosing the advantages of the centralized system (position, queue, ...).
* Add `ExNotifications` sample & `SDisplayTestNotification` service to test and to show how it can work.

### ImageSeries

*Add more attributes to our medical data.*

* Adds more attributes to our medical data folder.
* Creates patch related to these data.
* Add tests related to these new data versions.

### MeshFunctions

*Add function to convert point coordinates to barycentric one in tetrahedron.*

Add functions and test to convert a points coordinates to barycentric coordinates to its barycentric coordinate inside of a tetrahedron. Add corresponding unit test.

*Add function to convert world coordinates to barycentric.*

* Add functions and test to convert from world coordinates to barycentric coordinates if a point belongs to a triangle.
* Add corresponding unit test.

### MeshPickerInteractor

*New service for mesh picking interactions.*

### OpenVSlam

*Integrate OpenVSLAM in sight.*

* openvslamIO is a library that contains some conversion classes & functions in order to easily convert from/to sight to/from openvslam structures. OpenvslamIO is unit tested.

* openvslamTracker contains SOpenvslam service to interface with openvslam process
  * start/stop/pause/resume tracking
  * Get camera pose for each frames given to the service
  * Get the map as a `::fwData::Mesh`pointcloud
  * Save/Load the map.
  * Save Trajectories as .txt files (matrix or vector/quaternions format).

* ExOpenvslam is an example that show all possibilities that offer the openvlsam integration in sight.

**Note**: this is a preliminary integration so for now openvslam runs only on linux and with monocular perspectives cameras (very classical models).

### SActivitySequencer

*Add new editor to display an activity stepper.*

The list of the activities are displayed in a toolbar, the buttons are enabled only if the activity can be launched (if all its parameters are present). The sequencer uses the data from the previously launched activity to create the current one.

Create a new `ActivityLauncher` configuration to simplify the launch of an activity sequencer.

Improve `ExActivities` sample to use `ActivitySequencer` configuration and add volume rendering activity

### SAxis

*Add a configurable marker.*

Add a configurable marker on `visuOgreAdaptor::SAxis`

### SFrameMatrixSynchronizer

*Add new signals to be consistent with SMatrixTLSynchronizer.*

  -  Add slots to be compatible with the behaviour of `SMatrixTLSynchronizer` in order to connect with a `SStatus` service. The `ExStereoARCV` example is updated to show its usage.

  -  The configuration of `SMatrixTLSynchronizer` is updated to take into account the wish to send the status of some matrices in timelines in `SMatrixSynchronizer`.

  -  Now, it's possible to add the tag `sendStatus` in a `<key uid="matrixX" sendStatus="true|false"/>`.

  -  In this way, if you have multiple timelines and multiple matrices, you can choose which one you want to send its status like:

```xml
<inout group="matrices0">
   <key uid="matrix0" sendStatus="true"/>
   <key uid="matrix1" /> <!-- sendStatus is set to false by default -->
   <key uid="matrix2" />
</inout>
<inout group="matrices1">
   <key uid="matrix3" sendStatus="true"/>
   <key uid="matrix4" />
</inout>
```

### SImage

*Add editor to display an image.*

Create an editor ``::guiQt::editor::SImage``to display an image in a view.

### SImageMultiDistance

*Improve interactions and resources management.*

* Fixe `::visuOgreadaptor::SImageMultiDistance`.
* Add auto-snap on distances.
* Update distance on multiple scene.
* Improve resources management.
* Use our new interactor API.

### SLight

*Manage point light.*

* Adds point light in addition of directional light.
* Allows to manage point light position with `SLightEditor`.
* Fixes the specular color computation in `Lighting.glsl`.
* Allows to manage ambient color of materials in a new service `SOrganMaterialEditor`.
* Deprecates the service `OrganMaterialEditor`.

### SMesh

*Add default visibility configuration.*

* Add default visibility state in configuration of a mesh.
* Add a usage example of this feature in `ExSimpleARCVOgre`

### SMultipleTF

*Add new adaptor to manage TF composite.*

Creates a new adaptor to display a composite of TF and interact with them.

The following actions are available:

* Left mouse click: selects a new current TF or move the current clicked TF point.
* Left mouse double click: adds a new TF point to the current TF or open a color dialog to change the current clicked TF point.
* Middle mouse click: adjusts the transfer function window/level by moving the mouse left/right and up/down respectively.
* Right mouse click: remove the current clicked TF point or open a context menu to manage multiple actions which are 'delete', 'add ramp', 'clamp' or 'linear'.

### SNegato2D

*Take the slide position into account.*

* Take into account the slice position to have the right picking information.
* Retrieve the viewport size in the event method for `SNegato2D`.

### SParameters

*Manage dependencies.*

Adds dependencies system on `SParameters`.

### SPointList

*Add slots to update and toggle visibility.*

* Add a `toggleVisibility` slot in `SPointList`
* Implement this feature in `ExOpenvslam`

### SPreferencesConfiguration

*Add the possibility to specify a file in the preferences.*

### SRGBDImageMasking

*New service to perform depth image masking.*

### SSeriesSignal

*Listen the modified signal.*

### SSquare

*Deferred position update and mouse interaction toggling.*

- Implement deferred position update, via the "autoRefresh" xml attribute
- Allow to enable/disable mouse interaction, via the "interaction" xml attribute

### SsquareAdaptor

*Add slots on scene2d SSquare.*

Add a slot to the scene2D Ssquare adaptor which allows the change the configuration parameters of the square. This will allow the movement of the square by changing the values of `X` and `Y` on the widget slider.

### STextStatus

*Add input string.*

Add an input ``::fwData::String``and display it.

### Stransform

*Check if the matrix is null.*

Check if the given matrix of `visuOgreAdaptor`::STransform``is null to avoid a computation error with Ogre's nodes.

### style

*Add QLabel error style.*

Add a style in the qss to make a QLabel red and bold for errors.

*Add service and preference to switch from one theme to another.*

- Selected style is saved in preference file (if it exists), and then reloaded when you re-launch the app.

### SVector

*Add parameter to configure the vector visibility on start.*

Add `visible` parameter in `SVector` configuration to manage the visibility of the vector on start.

### SVideo

*Add update visibility on SVideo Ogre adaptor.*

Add a `updateVisibility` slot in `visuOgreAdaptor::SVideo`

### SVolumeRender

*Buffer the input image in a background thread.*

* Add a new option to load input images into textures in another thread.
* Add a new worker type able to handle graphical resources in parallel.

### Tuto06Filter

*Update the tutorial to follow the API changes (fwData and RAII).*

* Improve Tuto06Filter documentation according to sight-doc.
* Use new service API in SThreshold
* Remove useless dump locks in SThreshold
* Use SSlotCaller action instead of calling the operator service in the menu
* Add some comments

### ui

*Improve gui aesthetic.*

* Improve the background management in 'fwGuiQt'.
* Disable parameters in 'uiMeasurement::SLandmarks' instead of hide them.
* Allow to configure the style of 'SActivityWizard'.
* Properly use the 'hideActions' mode on toolbars.
* Properly set bounding boxes on 'visuOgreAdaptor::SAxis'.
* Add a flatdark theme.

### uiMeasurementQT

*Allow to remove landmarks with SLandmarks.*

* Allows to add or remove landmarks from picking information with the service 'uiMeasurementQt::editor::SLandmarks'.
* Updates the documentation.

### visuOgreAdaptor

*Improve the negato camera.*

Compute the camera ratio relatively to the viewport size

*Add a punch tool to the volume rendering.*

Adds a punch tool, a new adaptor to creates extruded meshes from a lasso tools, and a service to remove image voxels that are in meshes.

* The service `SShapeExtruder` works as follows:
  * The drawn shape is stored as a points list belonging to a 2D plane in the near of the camera, and a second one is stored in the far of the camera.
  * Once the shape is closed, a triangulation is done on the two points list with a constrained Bowyer-Watson algorithm.
  * Then, for each segment of the shape, two triangles are created between the segment at the near plane and the far plane.

*New adaptor to output renderTarget as an image.*

Add a new adaptor `SFragmentsInfo` that takes informations of the configured layer like, informations can be color, depth or primitive ID.

Some minors updates are also pushed in this MR:

1. SMaterial can now be configured with a `representationMode` (SURFACE/POINT/EDGE/WIREFRAME).
1. Add new conversion function in `fwRenderOgre`::helper::Camera``to convert from screen space to NDC or viewspace. deprecates the old ones.

*Directional light editing.*

Adds a visual feedback to light adaptors.

*Allow to manage TF windowing with SNegato2DCamera.*

* Add TF windowing management with SNegato2DCamera.
* Fix a configuring error from SPicker.

*Add new adaptors to resize a viewport.*

* Add a new compositor to draw borders in viewports.
* Add an adaptor to resize viewports

*Interact with the 3D negato.*

* Add the same interactions on the negato as in VTK
* Add a priority to the image interactors
* Add the ability to cancel interactions
* Deprecate the old selection interactor API

*Add a new adaptor to display a vector.*

Add `SVector`, a new adaptor that displays a vector.

### visuOgreQt

*Properly compute cameras aspect ratio.*

If an Ogre layer has a ratio different of 1:1, the camera aspect ratio of the layer is badly computed. It's due to the resize event that retrieves the wrong viewport ratio.

*Use qt to create the OpenGL contexts for ogre.*

* Add a new class to create OpenGL contexts for ogre.
* Move the offscreen rendering manager to `visuOgreQt` and instantiate it through a factory.
* Fix material management for the VR widgets.

## Performances:

### videoCalibration

*Speedup chessboard detection.*

* Add a scaling factor to the input to run the detection algorithm on a downscaled image.
* Const-overload `::cvIO::moveToCV`.
* Add a new preference parameter to configure the scaling in ARCalibration.


# sight 19.0.0

## Bug fixes:

### qml

*Use of a QML Engine to recreate dialog and apply a new style.*

* fwQt has been change to add a callback function that is called to create the application using QApplication or QGuiApplication.

* created libraries:
  * guiQml: start all services needed for a Qml Application and contain element to customise Material style.
  * fwGuiQml: contain all visual Component for Qml, like dialogs.

* guiQml
  * Plugin: call getQtWorker to setup services and launch the Application. Then set the style to Material.
  * Material.qml: Singleton to set the color of the theme globally in each Window.
  * qmldir: regroup all Control.qml in rc to override the theme from QtQuick.Controls and QtQuick.Controls.Material

* fwGuiQml
  * model: regroup ListModel and TableModel that are generic model to use inside Qml
  * dialog: recreate all Qt dialog in Qml

### vlc

*Conan package depends on testing dependencies.*

Update VLC to 3.0.6-r2 to depends from Qt and FFMpeg of the testing branch.

### fwServicesTest

*Increase timeout in fwTestWaitMacro.*

- The timeout of `fwTestWaitMacro` was increased to give more time when the runner is heavily loaded.
- An optional parameter to specify the duration of the timeout was added. This is useful when we want for instance a shorter duration when we want to verify that a condition did not occurred.

### SWriter

*Automatically parse file name when saving an activity.*

* Extension is added automatically if no extension is provided in filename
* if an extension is provided in filename it will overwrite extension selected in dialog, a error pop if extension is unknown.

### SHybridMarkerTracker

*Update hybrid_marker_track to version 1.1 to improve tracking speed.*

### fwRenderOgre

*Use std::isinf() instead of isinf() from math.h.*

### fwRenderOgre

*Remove Courier.ttf font file, not allowed for redistribution.*

The font `Courier.ttf` was removed from `fwRenderOgre` and replaced by `DejaVuSans.ttf`, which was already present in the bundle `material`. `Courier.ttf` was only used for the unit-tests, so the test was modified to reflect this change.

### VRRender

*Activities creation crash.*

Add parameters `APP_NAME` and `PROGRESS_CHANNEL` in `SActivityLauncher` service in VRRender `sdb.xml`.
Modify `SActivityWizard` to check if no tab was created because the selected activity did not have any input parameters.

### ExSolvePnP

*Crash due to wrong data inout key name.*

This MR fixes the issue Sight/sight#354. It edits the INOUT key name for the meshReader service in ExSolvePnP

### calibration

*Add missing locks in SSolvePnP and SChessBoardDetector.*

Without these locks, it is possible to find a configuration where the objects are used in the same time and the application crashed.

### CMake

*Set policy CMP0072 to NEW to avoid warning about findOpenGL on Linux.*

Please refer to `cmake --help-policy CMP0072` for details

### VideoRealSense

*Only set the output when the first frame has been grabbed.*

 -  Wait until the first frame has been grabbed to set the output. Doing so, prevent the use of an allocated but randomly filled buffer to be processed by other services, thus making them crash. See #333 for detail.

 -  Additionally, the pause mode has been fixed to not consume all cpu power.

### VRRender

*Crash at exit.*

This prevents VRRender to crash when exiting in all DICOM related activities.

Actually the crash occurred in the ioDicom plugin destruction. The usual linking hack, preventing the linker to strip symbols, consists in instancing one of the class of the bundle. It was placed in the destructor which is a bad idea because it creates an allocation of an UUID while the application is being destroyed.

### SSolvePnP

*Add missing locks.*

Without these locks, it is possible to find a configuration where the objects are used in the same time and the application crashed.

### TutoEditorQml

*Fix the crash when the application is launched.*

- The `startService` in `onServiceCreated()` is removed. `AppManager` base class has been updated to automatically start the services when they are added (if `startServices()` has been called once).

### core

*Add missing readlock.*

Some read locks are missing in central services like SCopy. Adding them may lead to slower but better, safer code

### SShaderParameter

*Force parameter update when the update slot is called.*

  -  Reset the dirty flag when calling the update. Not doing so prevented textures from being updated when modified.

### cmake

*Check CMAKE_BUILD_TYPE value.*

* on Windows, if the user doesn't specify a value for 'CMAKE_BUILD_TYPE', it's automatically initialized to 'Debug' after 'project()' cmake command.
* it's annoying because if we wanted to build in release, we have to clean the cmake cache and define `CMAKE_BUILD_TYPE` to `Release` (just redefining `CMAKE_BUILD_TYPE` isn't enough and is "dangerous")
* so now, we check `CMAKE_BUILD_TYPE` before cmake command 'project()', and if the user forgets to define `CMAKE_BUILD_TYPE` before configuring, configuration is stopped and display an error message

### RayTracingVolumeRenderer

*Allow to work with derived classes.*

### visuOgreAdaptor

*Missing color buffer when rendering point billboards.*

Add a new material to handle the case when the rendered mesh has no color buffer

### SFrameUpdater

*Change image dimension to 2D.*

Modify image creation in SFrameUpdater to change dimension from 3D to 2D

### visuOgreQt

*Fix crash in ogreResize().*

  -  Add a test to check if the ogre render windows is initialized before using it in the `ogreResize()` method

### visuOgreAdaptor

*Properly release resources.*

Releases Ogre texture properly

### ogre

*Fix infinite loop and graphic corruption in ogre shader code.*

-  Fix infinite loop and graphic corruption in ogre shader code by replacing a 'for' loop with its 'reverse' equivalent. It is very strange, and looks like a glsl compiler bug, but the workaround seems to work.

-  Some missing `makeCurrent()` were added

### visuOgreQt

*Fix hidpi mode for various mouse event.*

-  Computes correctly the mouse and window coordinate by taking into account HiDPI (retina) display, on each events, instead of only once. This should fix problems when switching display with a different resolution. Still a manual resize may be needed to force relayouting and thus to have correct size computation.

### SGrabberProxy

*Remove spaces when parsing camera Type Tags.*

### fwRenderOgre

*Prevent alpha from leaking onto the rendering window.*

* Disable alpha writing when blending layers with the background.
* Fix the transparent widget bug.

### SAxis

*Fix the SAxis node visibility.*

`SAxis` visibility is defined by changing the visibility of its node, which can be used by other adapters. The visibility is now sets with another method.

### fwVTKQml

*Fix QML Tutos with VTK Scene.*

The vtk OpenGLRenderWindow was changed from an external to a generic one which handle his own OpenGL context. So in each render at the beginning we init the state of the OpenGL by calling OpenGLInitState. QVTKOpenGLWidget was changed by QVTKOpenGLNativeWidget because in the code source of VTK it was written: `QVTKOpenGLNativeWidget is intended to be a replacement for QVTKWidget when using Qt 5`.

### fwCore

*Fix HisResClockTest::getTimeTest.*

We use now the `std::chrono::high_resolution_clock::now()` instead of `std::chrono::system_clock::now()`.
Although it may be different, especially on macos where system_clock tends to go too fast (!!!), it seems that on most platform it is indeed the same. This need to be validated on a long test period, and may require a revert..

### OgreVolumeRendering

*Pepper artifacts when the clipping box exceeds the image size.*

Clamp clipping box coordinates to not exceed the volume size to fix some artifacts when the user sets a clipping box bigger than the rendered image.

### fwServices

*Crash when swapping the inputs of stopped services.*

### Qml

*Set conan Qt/Qml directory in QML_IMPORT_PATH".*

Allow to use QtCreator to edit Qml files with the qt package from conan.

### ARCalibration

*Reverse the model pointlist coordinates.*

Reverse x and y coordinates on the calibration model according to openCV tutorial (https://docs.opencv.org/3.4/d4/d94/tutorial_camera_calibration.html). It does not seem to change the result.

### AppManager

*Fix auto-start services in AppManager.*

Fixes `AppManager` to start automatically services when all associated objects are available.

### OgreVolumeRendering

*Fix proxy geometry generation.*

A bug occurs when the volume rendering display a tiny image (like a 10\*10).

### visuOgreAdaptor

*Fix camera ratio in Ogre SFrustumList.*

Camera ratio was wrong in `visuOgreAdaptor`::SFrustumList``because it's not the same than `visuOgreAdaptor::SFrustum`.

Now, Both adaptors have the same behavior and display the frustum at the same size and the same place.

### videoVLC

*Vlc SFrameGrabber continuously pushing while paused.*

### AppConfigTest

*Add a missing wait condition in startStopTest.*

### fwDataTest

*Fix MTLockTest random failure.*

The test was not written correctly. This could lead to race conditions. We rewrote it in the following way.

The test try to lock the data to write two different string sequences in a `fwData::String`. It launches two asynchronous methods that lock the data to write, wait 2 ms between each char insertion and wait 5ms after the unlock. Then, it ensures that the letters from the two sequences are not mixed in the string.

### ObjectService

*Add missing mutex.*

There was a missing mutex lock in ObjectService::unregisterServiceOutput().
This should solves the random fail in AppConfigTest::startStopTest(), and possibly other random failures in the rest of the tests.

### ARCalibration

*Fix extrinsic calibration synchronisation.*

Now, we ensure that the chessboard is detected in the two cameras before adding the calibration information. It prevents a wrong synchronization of the calibration information.
By the way, an action was also added in the extrinsic view to save the calibration images.

Fixes #292

### fwDicomIOFilter

*Limit memory usage of fwDicomIOFilterTest & fwDcmtkIOTest.*

This replaces the usage of deepCopy in two Dcmtk filters. We used to copy the whole DicomSeries and then remove the internal buffer. This is both inefficient and memory expensive. We propose here to use shallowCopy() instead, which does not copy the buffer. The container is still cleared out, but only the pointers, so the source buffer is not destroyed.

### Ogre

*Set the origin of the SNegato2D to a corner.*

The origin of the `::visuOgreAdaptor::SNegato2D` representation is moved on the lower left corner of the image. This change is necessary because the distance system (which was designed for the VTK backend) returns values from the lower left corner of the image. To get this working properly with Ogre, we need to change the origin that currently lies in the middle of the image. The method used to change the orientation of the negato was also modified accordingly.

### videoRealSense

*Fix grabber to properly update the cameraSeries.*

Prevent to add multiple times the same camera in the cameraSeries when the `startCamera` method is called by checking if the cameraSeries is calibrated.
Fix ExRealSense configuration to set the proper parameters for the service.

### unit-tests

*Fix random failures.*

- **fwServicesTest**: add missing wait. Also, one second might not be enough when the system is under heavy load, so the waiting time has been increased to 2500ms instead of 1000ms. Remember this is just the worst case and usually the function returns in a shorter time.
- **ioAtomsTest**: this divides the test image size by 2 and reduces the number of reconstructions from 15 to 5 in the test model series. This should help a bit to reduce the execution time, see results below.
- **igtlProtocolTest**: actually fix a lot of the mesh conversion code. It crashed randomly, but only because the UTest was very poor. If done properly, it should have shown how bad the code was, since apart from positions and cells copy, all the rest of the array copies were broken.
- **All tests**:
  - when destroying a worker, we may ask it to join twice the thread if someone else is calling `stop()`. On top of that, hen stopping a worker, we decide whether we need to join the thread by testing the state of the io_service. However this does not seem to be a reliable way, so we now test the thread state itself,
  - when launching two unit-tests in parallel, they may stop at the same time. Both processes will try to remove temporary folders simultaneously, so we must handle failures properly with exceptions,
  - use temporary folders for writing instead of folders in the build tree,
  - last, we realized it is a bad practice to rely on stopping a worker with the auto-destruction (i.e. using shared pointers), because this could lead the `std`::thread``to be destroyed from its own execution scope. So we deprecated the call of `stop()` in the `WorkerAsio` and now we advise people to call `stop()` from the callee thread (most often the main thread).

All of this solves a lot of random errors when launching all unit-tests. We can also launch unit-tests in parallel now, making the CI jobs faster.

### SDK

*Remove relative paths in 3rd part libraries.*

All paths relative to the build host should now be removed from the include and library paths. The absolute paths are stripped away.
Some cleaning has also been done to include only the needed modules for VTK and PCL.

### qml

*VisuVTKAdaptor crashes when using QML.*

Registers imageSeries and tf objects in the SImageSeries constructor.

### Qml

*Fix SIOSelector when using Qml.*

Change the `AccessType` of the registered object from INPUT to INOUT when we are in writer_mode.

### Ogre

*Correct GLSL shaders compile errors on Intel chipsets.*

### SGrabberProxy

*Fix SGrabberProxy configuration selection.*

### SScan

*Convert point cloud positions to millimeter.*

RealSense camera return the point cloud in meters, but we need a point cloud in millimeters.
Thus we convert point positions to millimeter (multiply the values by 1000).
We also add a test if the camera is already started to avoid a crash.

## New features:

### calibrationActivity

*Improve widget layout.*

* Display all widgets other than the scenes in a panel on the left side.
* Fix the naming convention for service uids.

### cmake

*Add option to build a project with warnings as errors.*

* Added a cmake option `WARNINGS_AS_ERRORS` in sight to build a project in warning as error (`/Wx` on MSVC or `-Werror` on gcc/clang)
* Removed warning in sight project `fwlauncher`
* Enabled option `WARNINGS_AS_ERRORS` in `fwlauncher` project
* Changed warning level to `w4` on Windows (to see unreferenced local variable)

### cmake

*Improve package version number generation.*

* Added cmake helper script `get_git_rev.cmake` (from [sight-deps](https://git.ircad.fr/Sight/sight-deps/blob/dev/cmake/utils/get_git_rev.cmake)) to find git tags, branch name, version...
* Updated cmake script to generate a SDK filename using the latest tag (or git revision, see command [git describe --tags](https://git-scm.com/docs/git-describe))
* Updated cmake script to add Sight version in app packages

### cmake

*Add warning for links between bundles.*

Add a test in CMakeLists.txt to display a warning message if there are links between bundles

### fwRenderOgre

*Add a depth technique to the VR.*

Add a new technique to the volume rendering in order to display the depth of the volume.

* RayTracedVolumeDepth_FP: new fragment shader that displays the volume depth.
* OffScreenWindowInteractor: fix the rendering by calling the specific target instead of all targets.

Remove a double connection in `::visuOgreAdaptor::SInteractorStyle`.

### CMake

*Allow utilities to be launched from Visual Studio.*

Generate the `vcxproj.user` for utilities.

### OgreDynamicImageTest

*Speed-up copyNegatoImage() and updateImage().*

- Improve copyNegatoImage using parallel omp

### videoRealSense

*Align pointcloud on RGB frame for AR.*

Align streams to the desired frame coordinate system (Depth, Infrared, Color).
This allow us to have each stream in the same coordinate system, no need to apply transforms in xml configurations.

New option is available trough ExRealSense: "Align frames to"; this allow user to choose target coordinate frame where all frames should be align to.

Add "visible" option in configuration of `::visuOgreAdaptor::SPointList`

### SHybridMarkerTracker

*Extract tag position and orientation in camera.*

* Modify `SHybridMarkerTracker` to extract tag position and orientation and take into account an `::arData::Camera`.
* Configuration file was replaced by parameters that could be set through `SParameters`.

### hybridMarkerTracker

*Add service to track cylindrical hybrid marker.*

- Add SHybridMarkerTracker service for tracking a cylindrical hybrid marker
- Add associated ExHybridMarkerTracker example

### visuOgreAdaptor

*Addition of a distance measurement editor.*

  -  Addition of a distance measurement adaptor in the Ogre 2D negato. You can create a new distance, remove a specific distance or hide/show the distance.

### visuOgreAdaptor

*Support landmarks visibility.*

Added landmark visibility support in `::visuOgreAdaptor::SLandmarks`

### conan

*Update all package to allow sharing of C flags.*

  -  The goal of this code is to share C flags across all our conan packages to ensure compatible code generation as some compiler settings can lead to strange bugs, hard to debug (especially floating point mode like `-mfpmath=sse`, please see the associated issue https://git.ircad.fr/Sight/sight/issues/188).

  -  We choose to write the flags and compiler settings inside a python file packaged as a conan package (https://git.ircad.fr/conan/conan-common/tree/stable/1.0.0). For now, you will have acces to:

  -  `get_[c,cxx]_flags()`, `get_[c,cxx]_flags_[release,debug,relwithdebinfo]()`, `get_cuda_version()`, `get_cuda_arch()` and some utility functions for conanfile.py like `fix_conan_path(conanfile, root, wildcard)` which allows to fix path in .cmake files.

### SFrameMatrixSynchronizer

*Handle time shift delay.*

Handle synchronization issues in our application by applying a time shift value.

### fwRenderOgre

*Update `MeshPickerInteractor` to send `PickingInfo` over signals.*

Send ``::fwDataTools::PickingInfo``with `MeshPickerInteractor`to be more generic.

### SLandmarks

*Configurable text size.*

Use an xml configuration to set the text size of `::visuOgreAdaptor::SLandmarks`.

### visuOgreAdaptor

*Addition of a dashed line SLine.*

Ability to draw a dashed line and choose the distance between the dots. Configuration is done in XML (SLine service)

### videoRealSense

*Implement the record and playback from realsense library.*

Adding record/playback functionalities in ::videoRealSense::SScan.

* Record function can records all streams synchronously in one file in rosbag format (.bag)
* Playback function can replay recording and emulate a realsense device (filters can be applied, and camera parameters are the same as the real device).

Also add color on Ogre's SPointList adaptor.

### visuOgreAdaptor

*Adaptor displaying text.*

Display text along the borders or in the center of an OGRE window.

### videoRealSense

*Add filters on depth frame.*

Allow to filter the depth frame with the three filters provided by the RealSense SDK: spacial, temporal and hole filling. The filters can be enabled and configured with a `SParameter`.

The three filters are:
* **Spatial Edge-Preserving filter**: it performs a series of 1D horizontal and vertical passes or iterations, to enhance the smoothness of the reconstructed data.
* **Temporal filter**: it is intended to improve the depth data persistency by manipulating per-pixel values based on previous frames. The filter performs a single pass on the data, adjusting the depth values while also updating the tracking history. In cases where the pixel data is missing or invalid the filter uses a user-defined persistency mode to decide whether the missing value should be rectified with stored data. Note that due to its reliance on historic data the filter may introduce visible blurring/smearing artifacts, and therefore is best-suited for static scenes.
* **Holes Filling filter**: the filter implements several methods to rectify missing data in the resulting image. The filter obtains the four immediate pixel "neighbors" (up, down ,left, right), and selects one of them according to a user-defined rule.

Update `ExRealSense` to add widgets to change the filters parameters.

Add a service configuration in ARCalibration to play the RealSense infrared frame without the emitter.

Fix `SPointCloudFromDepthMap` to emit less signals. It emitted 'modified' signal on the mesh on each update which was causing the entire refresh of the VTK mesh (very slow). But only the vertex and the point colors are modified, so now, only 'vertexModified' and 'pointColorModied' signal are emitted.

### Conan

*Update many conan packages.*

Update conan package to more up-to-date versions and refactor sight a bit:

* boost --> 1.69
* eigen --> 3.3.7
* Qt --> 5.12.2
* pcl -->1.9.1
* vtk --> 8.2.0
* itk --> 4.13.2
* opencv --> 3.4.5
* ogre --> 1.11.5
* glm --> 0.9.9.5
* gdcm --> 2.8.9
* dcmtk --> 3.6.4
* odil --> 0.10.0
* cryptopp --> 8.1
* bullet --> 2.88

Details:

* **boost** related 
### fwIO

*Implement a fail/success notification on all readers and writers.*

Adds a new member status boolean to `IReader` and `IWriter`and all of their inherited implementations.

The following implementation of those interfaces have been modified :

Readers :

* ioAtoms/SReader
* ioCalibration/SOpenCVReader
* ioData/SAttachmentSeriesReader
* ioData/STrianMeshReader
* ioData/TransformationMatrix3DReaderService
* ioDcmtk/SSeriesDBReader
* ioGdcm/SDicomSeriesDBReader
* ioGdcm/SSeriesDBReader
* ioITK/InrImageReaderService
* ioITK/SInrSeriesDBReader
* ioITK/SImageReader
* ioVTK/SModelSeriesReader
* ioVTK/SSeriesDBReader
* ioVTKGdcm/SSeriesDBLazyReader
* ioVTKGdcm/SSeriesDBReader

Writers :

* ioAtoms/SWriter
* ioCalibration/SCalibrationImagesWriter
* ioCalibration/SOpenCVWriter
* ioData/MeshWriterService
* ioData/TransformationMatrix3DWriterService (great name btw).
* ioGdcm/SDicomSeriesWriter
* ioGdcm/SSeriesDBWriter
* ioGdcm/SSurfaceSegmentationWriter
* ioITK/InrImageWriterService
* ioITK/JpgImageWriterService
* ioITK/SJpgImageSeriesWriter
* ioITK/SImageSeriesWriter
* ioQt/SPdfWriter
* ioVTK/SImageSeriesWriter
* ioVTK/SImageWriter
* ioVTK/SMeshWriter
* ioVTK/SModelSeriesObjWriter
* ioVTK/SModelSeriesWriter
* ioVTKGdcm/SImageSeriesWriter

The following implementation of those interfaces have **not** been modified : Note: by default we consider that a non modified reader always return success...

Readers:

* ioTimeline/SMatricesRead : this service is to complex for a simple muggle like me.
* ioZMQ/SAtomNetworkReader : one of those that do a bit more than reading... :rolling_eyes:
* ioZMQ/SImageNetworkReader : same here

Writers:

* ioTimeline/SMatrixWriter : see above
* ioZMQ/SAtomNetworkWriter : idem
* ioZMQ/SImageNetworkWriter : idem
* videoOpenCV/SFrameWriter : not the actual behaviour we expect.

### videoVLC

*Implement RTP video streaming with libvlc.*

- Adds a new service ̀SFrameStreamer in the videoVLC bundle. Which allows to stream frames pushed into a FrameTL.
- Adds a new sample ExVLCVideoStreaming that grabs and streams a given video (either passed through a file or directly a stream).
- Adds and increments videoVLC conan package version dependency.

### conan

*Stores all used versions of conan packages in a single file.*

To simplify the process, all versions packages of conan are now merged into a simple conan-deps.cmake file

### GUI

*Add tooltips on views.*

A tooltip can now be set on any view of a cardinal or a line layout manager, optionally of course :

```xml
            <service uid="multiView" type="::gui::view::SDefaultView">
                <gui>
                    <layout type="::fwGui::LineLayoutManager">
                        <orientation value="horizontal" />
                        <view proportion="0" minWidth="30" toolTip="Amazing view" />
                        <view proportion="0" minWidth="50" />
                        <view proportion="1" toolTip="Please click me"/>
                    </layout>
                </gui>
                <registry>
                    <view sid="subview" start="yes" />
                    <view sid="button" start="yes" />
                    <view sid="button2" start="yes" />
                </registry>
            </service>
```

If the editor inside the view already has a tooltip, it will be not be overridden by this configuration, thus we do not break the existing tooltips.

### gitlab-ci

*Check the compilation with SPYLOG_LEVEL=trace.*

Update gitlab-ci to compile debug configuration with SPYLOG_LEVEL=trace to check wrong log message.
Fix a wrong debug message.

### SGrabberProxy

*Forward IGrabber's signals.*

### FrameLayoutManager

*Add visibility parameter.*

### opencv

*Use our own ffmpeg and rework opencv dependencies.*

### calibration

*Write calibration input images to a folder.*

Adds a feature to write calibration input images to a folder.
This can be useful for debugging or to compare our results with those of third-party programs.

Calibration info image writer.

### CI

*Add a new stage to build CI on linux to build the sdk.*

Besides this this also embeds some fixes for the sdk packaging on windows.

### test

*Use always slow tests skipping.*

Resolve "ease CI for slow test"

### fwTest

*Add a template version of randomizeArray.*

Add a template version of randomizeArray in fwTest generators.
It is useful for floating arrays because with the existing method some 'nan' values could appear, and the comparison of 'nan' return always false.

### TutoSimpleAR

*Promote ExSimpleARCV into a real tutorial.*

ExSimpleARCV is promoted to a real tutorial named TutoSimpleAR. The configuration has been cleaned a bit, comments have been added everywhere and the sample data are now downloaded at configuration time to help the beginners.
This sample is also a demonstration of the new design to synchronize AR rendering efficiently.
As most of the tutorials, it is documented in the official documentation.

## Refactor:

### deprecated

*Remove deprecated code related to sight 19.0.*

* Remove deprecated methods in `fwDcmtkIO`
* Remove outdated and unused bundle `uiNetwork`

### SActivitySequencer

*Emit a signal when the activity requires additional data.*

The service emits a signal 'dataRequired' if the activity can not be launch because some data are missing.
It allows to connect it to a SActivityWizard to select new data.

### conan

*Remove sight-deps support, conan is used by default.*

Conan is now the official way to retrieve 3rd party libraries in Sight.
Since it would be a pain to keep maintaining both sight-deps and Conan together,
we agreed to skip the normal deprecation phase.

* Remove option `USE_CONAN` in sight and make it the default.
* Clean all parts of the build system that refer to `EXTERNAL_LIBRARIES`.
* Remove support of Eclipse project

### fwPreferences

*Add getValue helper.*

The method `getPreferenceKey(...)` returns associated value saved in the preferences (if delimited with the character `%`) or simply returns the variable.
This helper is defined (copied/pasted) 12 times in Sight.
To avoid this, we have centralized all these versions in `fwPreferences` with the new method `getValue(const std::string& var, const char delimiter = '%')`.

### deprecated

*Remove deprecated code related to sight 19.0.*

### trackingCalibration

*Remove hand-eye reprojection service.*

SChessboardReprojection was unused in sight.

Remove the hand-eye reprojection service

### videoCalibration

*Rewrite chessboard detection.*

Modifies the SChessboardDetection service to take images as inputs instead of timelines.
Makes synchronization easier.

### RayTracedVolume_FP.glsl

*Remove IDVR referencies.*

## Performances:

### fwVtkIO

*Improve mesh conversion.*

The methods updating the mesh points, normals and texture to VTK were very slow, checking the allocated memory on each value. It is replaced by using the `SetArray()` methods with a copy of the data mesh array. This increases the speed from ~23ms to ~0.7ms for the point color update.


# sight 18.1.0

## New features:

### MarkedSphereHandleRepresentation

*Add ComputeInteractionState method.*

### videoRealSense

*Update realsense grabber:.*
- grab also pointcloud
- live loading of presets (in /rc/presets)
- enable/disable IR emitter
- switch between color/infrared frame
- live modification of min/max range
- speed-up the grabbing function
- brand-new ExRealSense using Ogre backend

Resolve "Output pointcloud from realsense grabber"

### ci

*Add ci-jobs to build sight on windows and macos.*

- Add some jobs to build sight on Linux, Macos and Windows
- Launch unit-test
- Use ccache to reduce build time

Resolve "Add windows and macos as gitlab CI target"

### CMake

*Add source packaging with CPack.*

It is now possible to create a source package for a specific application.
To do so, you have to:

- Choose an app you'd like to package. (e.g. VRRender, OgreViewer,...) This needs to be an **installable** app make sure it calls `generic_install` in its `CMakeLists.txt`
- Set it (and only it) as the `PROJECTS_TO_BUILD` in your cmake config.
- Run cmake `cmake .`
- Build the source archive, if building with ninja you can run `ninja package_source` else run `cpack --config CPackSourceConfig.cmake`
- Retrieve the source archive (e.g. VRRender-0.9-Source.tar.gz if you chose VRRender).

## Some results

A source package example : [PoCRegistration-0.1-Source.tar.gz](/uploads/a4fdd05b1681b7f01c1c553903f0f072/PoCRegistration-0.1-Source.tar.gz)

## Additional tests to run

This should better be tested on all platforms.

- [x] Windows
- [ ] ~~macOS~~ :arrow\_right: won't work see #248
- [x] GNU/Linux

### SDecomposeMatrix

*New service to decompose a matrix and associated tutorial.*

Add a new service to decompose a matrix into a rotation matrix, a translation matrix and a scale matrix.
Add a new sample using this service.

### TabLayoutManagerBase

*Add a border option.*
- add also Parameters_Gray.svg icon.

Resolve "Improve GUI esthetic"

### SParameters

*Add new index based enumeration slot.*

Add a slot which changes enumeration state based on the index and not on the label.

### video

*Add slot that plays or pauses according to current state.*

- Add new slot to IGrabber that plays or pauses.
- Add two state booleans on IGrabber.
- Add declaration to start, stop and pause on IGrabber.
- Add parent call to all derived grabbers for start, stop, pause.

### conan

*Find conan packages associated with installed cuda version.*

- Use conan option 'cuda' to find package associated with cuda version installed on the machine
- Available cuda versions: "9.2", "10.0", "None"

### SFlip

*Add service to flip images.*

- Add a service to flip images in the three main axes (can handle up to 3D images)
- Implement the SFlip service in Tuto06Filter

### vscode

*Add vs code generator.*

- add VS Code support to build and debug Sight
- to use VSCode with sight: run CMake configure with option SIGHT_GENERATE_VSCODE_WS, and open the workspace file "sight.code-workspace" (in build dir)

### conan

*Add linuxmint19 support.*

- Support new settings 'os.distro' in cmake-conan script into sight for linux.
- Available values in your packages for settings 'os.distro' are:
  - linuxmint18
  - linuxmint19

### idvr

*Draw the IDVR's depth lines from the outside of the countersink.*

### visuOgreAdaptor::SPointList

*Addition of a labeled point list.*

If the option "displayLabel" is set to true on the XML configuration, it will add a label on a point when the point is created.
You can also choose the character height and the color of the label on the XML configuration.

### PoCMergePointcloud

*Add a new PoC to merge pointclouds from 2 RGBD Cameras (Orbbec astras).*

Move opDepthMap & depthMapOp from internal repository to opensource.

Resolve "Sample to merge two RGBD Cameras"

### Calibration

*Export extrinsic matrix in .trf format.*

Resolve "Export extrinsic matrix in .trf in calibration activity"

### OGRE

*Configure stereo cameras using camera series.*

- Computes each projection matrix using intrinsic and extrinsic calibrations

### Ogre

*Improve stereo rendering management.*

- Adds an action to enable/disable stereo in an ogre layer.
- Refactors the stereo mode to be handled by the core compositor.
- Fixes some adaptors that crashed when restarting.

### SImageWriter

*Handles timestamps and bitmap images".*

### Player

*Add getter of video duration.*

Adds a getter for QMediaPlayer::duration() this is useful for knowing how much time is left on your video and especially this ticket for a video timeline editor which basically uses the `::fwVideoQt::Player`to get specific frames at specific positions.

## Bug fixes:

### template.sh

*Add quotes in linux sh template (build & install).*

Resolve "Wrong bundledir path in application scripts automatically generated by CMake"

### OgreViewer

*Correct user interface bugs.*

The compositor uniforms are now properly shown in the compositor selector.
The application no longer crashes at exit when all lights are removed.

### SMaterial

*Use a lookup table to correctly display 16 bits texture.*

### videoOpenCV

*Duration calculation & loop mode.*
* Correct the duration calculation.
* Use total number of frame and current frame index to loop the video.

Resolve "OpenCV grabber loop mode & duration"

### guiQt

*Copy the styles plugin on install.*

Installed and packaged apps looked bad because we forgot to ship the qt styles plugin.

### fwRuntime

*Remove windows.h in Library.hpp.*

- avoid exposing of windows.h in fwRuntime`::dl::Library`header (via dl::Win32.hpp)
- use abstract class fwRuntime`::dl::Native`(as a pointer) in the hpp and instantiate the specific implementation in the cpp
- update vlc conan package used in videoVLC to fix problems related to windows.h

### Flipper

*Move the implementation to the correct path.*

Resolve "Move Flipper.cpp from imageFlipperOp to imageFilterOp"

### videoOrbbec

*Allow to use the grabber in RGB mode.*

Fixes a crash in videoOrbbec`::SScan`if no depth timeline was given. Now, we can use this grabber to get only the RGB frames.

### ci

*Fix job to deploy doxygen on the sight gitlab page.*

- Doxygen pages can no longer be deployed on the CI due to an error on the gitlab-ci script.
- There is an error because the git configuration has been defined globally for all jobs, and the deployment jobs use a docker image that doesn't contain 'git'.
- To fix this, we'll use the same docker image for all linux jobs (DOCKER_ENVDEV_MINT19)

### guiQtTest

*Add missing profile.xml and gui requirement.*

### fwDicomIOFilter

*Fix unit tests.*

### SStereoToggler

*Remove fwServicesRegisterMacro macro.*

Fix compilation without PCH by removing the fwServicesRegisterMacro

Resolve "Missing header in SStereoToggler.cpp"

### utests

*Various fixes for fwJobsTest, fwGdcmIOTest, fwRenderOgreTest, fwThreadTest.*

This fixes the following unit tests:

- fwJobsTest: The problem was visible on macOS, but other platforms maybe impacted. Testing if the future is valid before waiting did the trick. Also a wrong "+" with a char* and an integer which give also a crash in release, has been replaced with a proper string concatenation

- fwGdcmIOTest: The hashing function boost::hash_combine() doesn't always use the same implementation across platforms (!!!). We now use a regular sha1 hashing, a bit slower, but safer

- fwRenderOgreTest: Deleting the dummy RenderWindow with destroy() leads to a double delete crash when deleting ogre root node. Using the appropriate Ogre::RenderSystem::destroyRenderWindow() seems to fix the crash

- fwThreadTest: Use a workaround to mitigate the callback cost in time computation. This is a workaround because either the test itself is false (and maybe undoable, as unpredictable), either we must change the implementation of TimerAsio to take into account the time taken by the callback. We may look again for https://git.ircad.fr/Sight/sight/tree/fix/TimerAsio, but it is clear this change will have side effect since some code may actually want to wait for the callback to perform before reseting the timer. I guess that playing a video with openCV did fail with the above merge request

### cmake

*Use the correct macOS flags to build pch.*

Pass the correct macOS flags when compiling pch

### gitignore

*Parse also versioned qtcreator cmake preferences files.*

Resolve "Update gitignore to include qtcreator's versioned files"

### conan

*Quiet conan output.*

Update cmake script with conan OUTPUT_QUIET option to have a more quiet conan's output

### GridProxyGeometry

*Fix variables declaration.*

### SGrabberProxy

*Correct parsing of grabber list and ensure that the list is filled even if no config is set.*

Resolve "Empty SGrabberProxy list"

### ci

*Update ccache path.*

- now ccache use a nfs folder mounted in docker image
- previous gitlab-ci cache is not compatible with this system

### SFrameWriter

*Use the correct timestamp in image filename.*

Use the timeline timestamp instead of the current timestamp when saving frames using videoOpenCV::SFrameWriter

## Refactor:

### Ogre

*Remove IDVR from the repository.*

This MR removes all IDVR references from OgreViewer. This makes the application simpler,
because over time it accumulated too many features.

In the same time, we added volume rendering and negatoMPR adaptors to the VTK tab,
in order to show the comparison between the two back-ends.

OgreViewer has been updated to 0.3.

### ui

*Remove deprecated services (ShowLandmark, SaveLandmark, LoadLandmark and DynamicView).*

Resolve "Remove deprecated services"

### Bookmarks

*Deprecate ::fwTools::Bookmarks.*

Add FW_DEPRECATED macro in fwTools`::Bookmarks`and the associated service ctrlSelection::BookmarksSrv.
The bookmarks are no longer used and the service still use a deprecated API.

### HiResClock

*Un-deprecate getTime* functions.*

Resolve "Un-deprecate HiResClock functions"

### videoRealSense

*Use the version 2 of the realsense api.*
This is a basic implementation of a Grabber for D400 cameras:

* parameters such as resolution, fps, ... are set in code
* the camera chosen with the videoQt selector is not taken into account by the grabber (Qt doesn't recognize correctly realsense device)
* if multiple realsense cameras are plugged-in the grabber will pop a selector dialog
* depth pointcloud is not outputted for now

### filterVRRender

*Rename filterVRRender to filterUnknownSeries.*

`FilterUnknownActivities` filter from `filterUnknownSeries` bundle allows
to remove the unknown activities when reading a medical file (.json,
.jsonz, .apz, ...) with `::ioAtoms::SReader`.

The old filterVRRender bundle is kept for backward compatibility, but will
be removed in version 19.0.

### ioGdcm

*Remove plugin.xml in ioGdcm.*

Remove the plugin.xml file in ioGdcm bundle because it is useless (cmake generates it automatically).
Also change the documentation of services.

### uiVisuOgre

*Remove obsolete getObjSrvConnections().*

### fwTools

*Move shared library path search code and make it generic.*

This introduces a new function in `fwTools`::Os``:

```cpp
/**
 * @brief   Return the path to a shared library name
 * The library should have already been loaded before.
 * @param _libName The name of the shared library, without any 'lib' prefix, 'd' suffix or extension,
 * i.e. 'jpeg' or 'boost_filesystem'. The function will try to use the appropriate combination according to
 * the platform and the build type.
 * @return path to the library on the filesystem
 * @throw `::fwTools::Exception`if the library could not be found (not loaded for instance)
 */
FWTOOLS_API `::boost::filesystem::path`getSharedLibraryPath(const std::string& _libName);
```

This function has been used to search for Qt plugins in `WorkerQt.cpp`,
where this was originally needed. It can be used in some of our utilities as well. The code is generic and has been unit-tested properly.

### video

*Better support of selected device in video grabbers.*

* Strongly identify camera when using Qt Selector, and add better support of multiple same model cameras

  * Refactor CameraDeviceDlg to keep usb order in comboBox
  * Uniquely identify cameras using a prefix pushed in description.

* Grab Webcam with OpenCV is now multiplatform.

* Enhance the video support when using OpenNI grabbers.

  * Add support of multiple connected astra (videoOrbbec)
  * Use Opencv instead of Qt to grab RGB frame
  * Grab RGB & Depth frame in same thread (synchronized on depth)
  * Remove Qt in videoOrbbec

* Add Astra utility program to replace over-complicated SScanIR

  * astraViewer: to view Color/IR and Depth stream
  * astraRecord3Streams: to display Color + IR + Depth and take a snapshot (! FPS will be very very slow ! ).

Resolve "videoOpenni and videoOrbbec SScan don't take into account the selected device"


# sight 18.0.0

## Bug fixes:

### STransform

*Check parent node existence before creating it.*

### unit-tests

*Look for profile.xml in the source directory.*

Instead of looking for a profile.xml in the build directory for the unit-tests, now we look into the source directory. Otherwise, this does not work when CMake is only ran once.

Doing this, some missing bundle requirements in unit-tests were fixed.

### CMake

*Check if profile exists to enable bundles load on unit-tests.*

### *

*Add GL_SILENCE_DEPRECATION definition globally.*

Before it was only defined in fwRenderOgre.

Resolve "OpenGL is deprecated on MacOS 10.14"

### TutoOgreGenericScene

*Update deprecated service key.*

This made this application crash.

### install

*Remove useless install target.*

setpath.bat and <app_name>.bat are only usable in build dir

### unit-tests

*Ensure test data are really free and anonymized.*

This anonymize some data used in our unit-tests.

### SMesh

*Check if node exists before attaching it to root.*

### DCMTK

*Change DCMTK files path on Windows.*

### *

*Resolve errors reported by FOSSA.*

- Update our license headers to mention Sight and not FW4SPL
- Change the license headers to follow [GNU recommendations](https://www.gnu.org/copyleft/lesser.html)
- Add IHU Strasbourg copyright
- Remove LGPL license headers from minizip files
- Remove DCMTK files and copy them at build instead
- Add copyrights for MIT licensed files.

### fwVideoQt

*Properly close the camera device and deallocate the related resources.*

Resolve "ARCalibration crash when stream stops"

### macos

*Fix OgreViewer for macOS in debug mode.*

### calibrationActivity

*Correct intrinsic editor inputs.*

Calibration activity crashes when starting the intrinsic editor because of an input that should be an inout.

Merge remote-tracking branch 'origin/fix-calibration-activity-crash-on-start' into dev

### SReprojectionError

*Fix indexing of marker points.*

### fwRuntime

*Resolve NOMINMAX redefinition.*

- NOMINMAX was defined twice (in 'fw-boost.cmake' and in 'fwRuntime/dl/Win32.hpp')
- a warning C4005 was displayed each time this file is used

### conan

*Update conan packages.*

Conan has new packages available, it is necessary to update the versions used by Sight for macos and QML

### ImportanceCompositing_FP

*Allow Nsight compiling.*

### SActivityWizard

*Fix crash when loading or importing a data.*

Fix load and import in SActivityWizard. SIOSelector service must be configured before to associate the output object Id.

### CMake

*Use the correct variable for deps without Conan.*

We just used the wrong variable...

### visuOgre

*Add include file.*

### SCamera

*Replace assert by warning when creating cameras.*

Allow to use ``::videoQt::editor::SCamera``when we don't know if the input CameraSeries will be initialized or not: the cameras will be generated if the cameraSeries is empty.

### ogre

*Fix naming conflicts and double deletes.*

### sight

*Set external lib dir in scripts to launch Sight apps.*

Scripts used to launch Sight applications are not correct because the variable FW_EXTERNAL_LIBRARIES_DIR is empty.

### fwRenderOgre

*Silent deprecated OpenGL warnings on macos.*

Resolve "[macos 10.14] OpenGL is deprecated"

### tests

*Fix fwServicesTest and fwActivitiesTest.*

Fix the broken tests of `fwServicesTest` and `fwActivitiesTest`:
- fix the resources path
- update the wait macros to test if a service exists. Replace `::fwTools::fwID::getObject(uid)` by `::fwTools::fwID::exist(uid)` because when getObject() is called, an assert is raised: the service pointer expires before it is removed from the ID map.

### CMake

*Remove usage of QT_QPA_FONTDIR on Linux.*

Usage of **QT_QPA_FONTDIR** is deleted in both build & install scripts
since now we build Qt with fontconfig support.

### fwRenderOgre

*Move declaration of static const outside the class.*

### Ogre

*R2VB objects can't be picked.*

This allows quad-based meshes to be picked. Since the quads are triangulated with a geometry shader, it was not that easy to perform. The chosen solution is not very elegant, I added intersection tests on the quads themselves. Testing intersections on the triangulation is harder to perform and on top of that, at the beginning we did not intend to read it from the CPU.

### IDVR

*Mesh/countersink interference.*

Also brings a few new buttons to the OgreViewer toolbar and fixes the clipping box update.

Only works with the Default or the DepthPeeling transparency technique.

[Ogre] Fix idvr with mixed rendering

### SPointListRegistration

*Test if registered pointList has labels.*

Fix SPointListRegistration service when having two pointLists which one has labels and the other one has no labels.

Resolve "Fix SPointListRegistration"

### fwDataToolsTest

*Glm initialization.*

### CMakeLists

*Add a define for aligned storage compatibility with visual 2017.*

### macos

*Remove installation script for executable.*

Remove installation script for executable for macOS.
This script did not work and broke the cmake configuration. It should be rewrite completely.

See #131

### export

*Fix export macro in arData and calibrationActivity.*

### cmake

*Allow use of relative path for EXTERNAL_LIBRARIES.*

Resolve "Cannot use relative path for EXTERNAL_LIBRARIES"
### ogre

*Fix relative paths on linux.*

material paths are now absolute using predefined variables

### ogre

*Add missing makeCurrent() and fix glsl errors.*

### CMakeLists.txt

*Fix link errors with vtk libraries.*

The PCL discovery was done in the main CMakeLists.txt, thus defining VTK compile definitions for all targets. This led to strange link errors, notably in unit-tests.

## New features:

### conan

*Implement a better Conan support, especially on macOS.*

- Remove libxml2 and zlib dependencies on macOS
- Use DYLD_FALLBACK_LIBRARY_PATH instead of LD_LIBRARY_PATH because it is unsupported in macOS and DYLD_LIBRARY_PATH prevents using ANY system libs.
- Remove unneeded activities bundle dependency to gui
- Fix application script template generation to use : as path separator instead of ;

- Cleanup template script
- Add VLC conan package for Windows and macOS, use system for Linux
- Add scripts to launch unit tests and utilities with conan packages

- bonus: fix ioAtoms and ioITK unit tests (updated object access type)

### doxygen

*Generate sight doxygen with Gitlab-CI.*

- A new Gitlab-CI job is added to generate and publish Sight Doxygen
- Doxygen is built only if sheldon and debug/release build pass
- Doxygen is only deployed on the dev branch
- Sight doxygen is available on https://sight.pages.ircad.fr/sight

### tutoQml

*Use qml and c++ for tuto 08 and 09.*

New version of the Tutorial 08 and 09 using qml and c++ (without XML configuration).

- fwVTKQml:new library to allow to display a VTK scene in qml
- visuVTKQml:new bundle to initialize qml FrameBuffer
- uiMedDataQml, uiImageQml, uiReconstructionQml: new bundles containing services inherited from IQmlService and the associated qml files.

### tutoQml

*Add basic samples using qml interfaces.*

Add new samples to use Qml:
- `Tuto01BasicQml`: same as Tuto01Basic but using qml instead of fwGui frame and xml
- `TutoGuiQml`: same as TutoGui but using qml instead of fwGui frame and actions
- `TutoEditorQml`: sample to explain how to use a IQmlEditor

Add a new library `fwQml` that contains helpers to use qml:
- IQmlEditor: base class for f4s service associated to qml UI
- QmlEngine: to launch qml ui

### tutoCpp

*Convert tuto 08 and 09 to use C++.*

The new Samples `Tuto08GenericSceneCpp` and `Tuto09MesherWithGenericSceneCpp` are equivalent to the existing samples (same name without 'Cpp') but they doesn't use a XML configuration.

All the services are managed in C++ in the application `Plugin` class. The services' configurations are written with `::boost::property::tree`.

To achieve this, a new helper : ``::fwServices::AppManager``was added. It simplifies the management of objects/service/connections in an application.

### CMake

*Use Conan to build Sight (experimental).*

Add conan support in sight:
- A temporary conan registry is used for the moment: [Artifactory](http://5.39.78.163:8081/artifactory)
- All conan package are available on [gitlab Conan group](https://gitlab.lan.local/conan/)
- A new advanced CMake option is available: `USE_CONAN`

### trackingHandEyeActivity

*Load and evaluate calibration matrices.*

Adds the possibility to load the hand-eye X matrix for evaluation.

Once loaded the Z matrix is computed using the current tracked position and camera pose.

The camera can than be moved around and the app will display the reprojection and reprojection error.

Evaluate external calibrations in the hand-eye activity.

### SLabeledPointList

*Implement updateVisibility slot.*
- to update visibility of SLabeledPointList, we have to implement updateVisibility slot into SPoint, SPointLabel, SPointList

Resolve "Implement updateVisibility slot in SLabeledPointList"

### PointList

*Add pointlist management methods.*

* Add a slot to clear the pointlist in SMarkerToPoint
* Add a slot to manage the visibility of the pointlist in the adaptor SPointList3D

### SRenderStats

*Display rendering stats such as FPS in the OGRE overlay.*

- Adds a new adaptor to display FPS and triangle count in ogre windows.
- Fixes a crash when setting the volume visibility when no image has been loaded.
- Removes the option to print stats in a console.

### sslotcaller

*Add wait option for synchronized slot calling.*

Add a wait option to SSlotCaller that is used to call the slots synchronously. This allows for dependent slots to be called in the right order.

### 21-integration-of-sopticalflow-from-internship-repository

*Into dev.*

Add a new service that performs optical flow on a video to detect if camera is moving or not.
Use the service in a new example called ExDetectCamMotion.

SOpticalFlow is designed to only detect if camera is moving not if something happend on video.
It can send 2 signals: cameraMoved if motion and cameraRemained if camera is stable.

This can be usefull to trigger event when motion or remaining phases are detected, ex: only add calibration image if camera is not moving.

### 124-sort-example-by-topics-and-remove-numbering

*Into dev.*

Remove numbering of Samples, and add topics folders.

Resolve "Sort example by topics and remove numbering"

### CMake

*Introduce SDK mode.*

After many years, we propose a real build of Sight as a SDK. This allows to:
- create a binary package containing Sight binaries, libraries, includes and resources,
- include/link a Sight library in a non-Sight application/library,
- create a new Sight library, bundle or application outside Sight source and build tree

### .gitignore

*Ignore vs2017 cmake project files.*

## Refactor:

### *

*Remove "fw4spl" references.*

Resolve "Remove all references to "fw4spl""

### ITransformable

*Node managements is now handle by ITransformable.*

* Factorise some functions to have a common method into ITransformable.
* Set the parent transformation only in STransform (else, two adaptors with the same transform can have two different parents).
* Fix the stopping method of SAxis.

### ioIGTL

*Modify INetwork and STDataListener to manage timestamps.*

- Add receiveObject function in INetwork to return the message timestamp
- Add optional timestamp parameter in manageTimeline from STDataListener

### deprecated

*Remove 'FW_DEPRECATED' related to 18.0.*

Remove the deprecated code associated to the macros `FW_DEPRECATED_xxx(..., "18.0)`.

### MedicalImage

*Split to MedicalImage and TransferFunction.*
Splits `::fwDataTools::helper::MedicalImageAdaptor`to `::fwDataTools::helper::MedicalImage`and ::fwDataTools::helper::TransferFunction

Most of the adaptors used the half of the `::fwDataTools::helper::MedicalImageAdaptor`: one part to manage an fwData`::Image`and the other one to manage fwData::TransferFunction.

Resolve "Split `MedicalImageAdaptor` into `MedicalImage` and `TransferFunction`"

### getObjSrvConnections

*Replace getObjSrvConnections() by getAutoConnections().*

Resolve getObjSrvConnections() errors by implementing getAutoConnection() method.

### fwCore::HiResClock

*Use std::chrono inside and deprecate in favor of std::chrono.*

### ogre

*Refactor volume ray entries compositors.*

- Removes the volume ray entry compositor scripts and generates them instead.
It will be a lot easier to add stereo IDVR now.

- Moves all programs/materials/compositors used in fwRenderOgre to it.

* **:warning: IMPORTANT** Delete share/fwRenderOgre and share/material in your build directory

### cmake

*Cmake cleaning.*

cleans sight CMake scripts:
- removed unused platform
- removed racy backward compatibility

### *

*Reorganise some folders.*

After the big merge with fw4spl-ar, fw4spl-ogre and fw4spl-ext, it was necessary to clean up a bit the folders hierarchy, notably to group related items together. No project is renamed, the projects were only moved or their parent directory was renamed.

See sight!2

## Documentation:

### README.md

*Remove all mentions of 'fw4spl'.*


# fw4spl 17.2.0

## New features:

### calDataGenerator

*Add an utility program to generate stereo pair of chessboard/charucoboard images.*

This can be useful to test calibration algorithms.

### handEyeActivity

*Handling a step value in SMatricesReader and SFrameGrabber.*

Add a step value in readNext()/readPrevious() slots in SMatricesReader and SFrameGrabber when configured on oneShot mode.

This step value can be changed calling a setStep slot, connected with an int SParameter withstep` key.

We also needed to add this setStep slot in the SGrabberProxy and IGrabber in order to call it properly when using a SGrabberProxy instead of a SFrameGrabber directly.

### Ogre

*Update ogre to 1.11.*

This brings a bunch of fixes following API changes. Among them :

* default light direction is set to the camera's view direction, was implicitly the case in ogre 1.10 but changed in 1.11
* `Codec_FreeImage` plugin loaded to support common image file formats
* plugin config parsing was modified to be able to load multiple plugins
* `::Ogre::Affine3` replaces `::Ogre::Matrix4` when we need to decompose a matrix
* colour masks are enabled when computing volume ray entry points

## Refactor:

### ut

*Replace deprecated methods to register a service.*

Replace the `::OSR::registerService(obj, srv)` by `srv->registerInOut(obj, key)`
in the unit tests.

### cmake

*Remove racy backward compatibility.*

### registerService

*Replace deprecated methods to register a service.*

Replace the `::OSR::registerService(obj, srv)` by `srv->registerInOut(obj, key)`
and use `::fwServices::add(srv)` helper instead of calling directly `Factory

### SVolumeRender

*Store clipping matrices the same way VTK does.*

Now clipping box transforms are stored in world space instead of texture space. Clipping transforms can be passed from/to VTK that way.

Removes the **broken** slice based volume renderer.

### plugins

*Remove the freeimage plugin.*

### textures

*Convert all png and tga textures to dds.*

DDS is supported natively by ogre without the freeimage plugin.

The freeimage BinPkg is awful to maintain and should be considered as deprecated from now on.


## Bug fixes:

### glm

*Add missing GLM_ENABLE_EXPERIMENTAL define.*

Unused glm extensions have been removed

### fwRuntime

*Fix memory leaks.*

Fix leaks in fwRuntime

### SMaterial

*Texture rendering on other formats than 8 bits.*

### IHasServices

*Add wait() when stopping services.*

- add wait() when stopping services in unregisterService and unregisterServices methods.

### SGrabberProxy

*Include/exclude mode wasn't working as expected.*

- Improve include/exclude filtering. We can include/exclude a specific service or a specific configuration of a service or both.
- Grabbers are now always displayed in same order in selector dialog.
- Frame by Frame mode from `::videoOpenCV::SFrameGrabber`has been excluded from calibration

### registrationActivity

*Fix all errors in the registration app.*

- fix all log errors
- remove useless autoConnects


# fw4spl 17.1.0

## Refactor:

### fwServicesTest

*Clean unit test and deprecate unused methods.*

Add `FW_DEPRECATED` macro for:
- swapService(obj, srv)
- registerService(obj, service)
- getServices(obj)
- getServices(obj, type)
- fwServices::add(obj, srvType, srvImpl)
- fwServices::get(obj)

Replace the deprecated methods in the tests by the new ones.
Replace C++ configuration by XML file for the tests of AppConfigTest.

Keep a few tests on the deprecated methods until the methods are officially removed.

### VRRender

*Remove the deprecated logs.*

Clean the configurations to remove the deprecated logs:
- remove the useless objects and services
- use the right key
- remove useless autoConnect

Update appConfig.xsd to set 'uid' attribute as required for services.

Add missing 'getAutoConnections()' in some services from visuVTKAdaptor.

### tutorials

*Remove the deprecated logs.*

Remove the deprecated log:
- clean configurations
  - remove useless autoConnect
  - use the right keys in services
  - remove useless services and object
- add getAutoConnections() methods in uiReconstructionQt to replace the default deprecated getObjSrvConnections() from IService
- use the new API to register the reader/writer in ::uiTF::TransfertFunctionEditor
- remove auto-connection on 'tf' in the vtk adaptors when registering a sub-service

### ObjectService

*Support optional output in services.*

- if the output is not defined in the XML configuration, the object is not emitted to the configuration.
- add a method in IService to check if the object is defined: hasObjectId()
- the method getObjectId() throw an exception as described in the doxygen

### SWriter

*Set 'data' as input instead of inout.*

Set 'data' as input in '::ioAtoms::SWriter'
- update RecursiveLock visitor to use const object
- add constCast in SWriter before the conversion to atoms

### deprecated

*Remove deprecated getObject() in services.*

Replace 'getObject()' by 'getInput()' or 'getInout()' and add a deprecated
log if the input key is not correct.

### getObject

*Remove deprecated getObject().*

Replace deprecated `getObject()` by `getInout()` in `::uiCalibration::SIntrinsicEdition`

Depreciate some bundles and services:
- bundles: ioZMQ, uiZMQ, uiNetwork
- services: SProbeMesh and SProbePosition from echoEdSimu

### getObject

*Replace the last 'getObject()' by 'getInOut()'.*

Replace the last two getObject() by getInout().
They were forgotten in service that already use getInout() for these data,
so the deprecated log is useless.

## New features:

### VRRender

*Add activity to upload DICOM series via DicomWeb protocol.*

New activity that anonymizes and uploads DICOM series onto an Orthanc PACS.

### proxyConnection

*Catch exception when the connection failed.*

Catch the exception raised when a connection failed between signals/slots
defined in the configuration.
It displays a error log with the signal/slot information.

### cvIO

*Add new conversion function between cv::Mat and f4s matrices.*

Convert from/to `::cv::Mat`to ::fwData::TransformationMatrix3D
Convert from/to `::cv::Mat`rvec & tvec convention to ::fwData::TransformationMatrix3D
Add new unit tests cases.
Refactor Calibration code to use new helpers.

### trackedCameraCalibration

*Merge activities.*

Fuse sense specific activity and rgb activity thanks to SGrabberProxy.

### video

*Import VLC, Orbbec and RealSense grabbers.*

VLC, Orbbec and RealSense grabbers code is now open and imported into fw4pl-ar, as well as the video filtering. The VLC grabber is convenient especially for RTSP streams. It may also be used as a fallback when the QtMultimedia grabber fails... The Orbbec grabber works for Astra camera and the RealSense brings support for cameras based on Intel sensors.

### fwRenderOgre

*Add a helper to convert pixel to view space position.*

The function `convertPixelToViewSpace` translates a pixel coordinates to view space coordinates.

### SNegato2D,3D

*Use the transparency of the transfer function (optionally).*

A new option was added to use the transparency of the transfer function.

### SAxis

*Add a label configurable option.*

SAxis now has an option `label` that can be set to `true` or `false`
to display or hide the axis labels (`true` by default).

### SRender

*Add a 'sync' renderMode.*

In the following of our recent rework of the synchronization for real-time augmented-reality, this new mode allows to make the Ogre generic scene compatible with the approach. The example ExSimpleARCVOgre was reworked to use the new sync mechanism and proves that this works.

## Documentation:

### eigenTools

*Document helper namespace.*

### visuOgreAdaptor

*Update some documentation.*

The documentation of several adaptors were fixed.

## Bug fixes:

### cmake

*Update wildcard to search all external libraries.*

Before only .so.* was found.

### pchServicesOmp

*Remove clang specific hack about OpenMP.*

Remove a clang specific OpenMP hack in our CMake code.

### fwDataCamp

*Fix compilation.*

Add a missing header in fwDataCamp (Build without PCH)

### plugin_config_command

*Support 0 in service or bundle names.*

Fix the regex used to generate the service definition in plugin to
support zero.

### docset

*Fix broken docset generation.*

### boost

*Add support of Boost 1.67 on Windows.*

Boost >= 1.67 changes the default random provider on Windows to use BCrypt.
So a link to system library bcrypt is now required to use Boost::UUID.

The changes are compatible with old Boost version.

### ARCalibration

*Remove warnings by using seriesDB key instead of series.*

Fix series keys to seriesDB used in various configurations because it will be removed in 18.0 version of FW4SPL.

### activitySelector

*Remove warnings by using seriesDB key instead of series.*

Fix series keys to seriesDB used in various configurations because it will be removed in 18.0 version of FW4SPL.

### beginnerTraining

*Fix the training samples.*

- fix the documentation for the plugin.xml generation
- remove the fwServicesRegisterMacro from the services to let cmake to generate the right one
- add getAutoConnections() for tuto03 and tuto04 SStringEditor

### fwRenderOgre

*Correct valgrind errors and leaks.*

Memory errors were fixed and memory leaks detected by valgrind (memcheck) on the test suite:
* One out of bounds in read `fwRenderOgre::helper::Mesh`
* Memory leaks on Ogre root destruction

### fwRenderOgre

*Missing headers.*

### fwRenderOgre

*Remove clang specific hack about OpenMP.*

Remove a clang specific OpenMP hack in our CMake code.

### R2VBRenderable

*Clear vertex declaration before filling it.*

This caused the varying to be duplicated, and thus the program link to fail.

### Mesh

*Generate normals each time the mesh is modified.*

For triangle based meshes, when we don't have normals, we generate them. The problem was that it was only done on the first update of the mesh. If points were added to the mesh, the corresponding normals were not computed accordingly, thus the normal layer ended to be be shorter than the position layer. This led eventually to crash at some  point...


# fw4spl 17.0.0

## Bug fixes:

### VRRender

*Do not crash when clicking on the distance button in VR.*

The service `::uiMeasurement::editor::Distance` was also cleaned a bit, and the unused configuration option 'placeInScene' was removed.

### docset

*Generation on case sensitive systems.*

### SMesh

*Lock the input mesh properly in slots.*

### SPoseFrom2d

*Trigger modified signal even if nothing is detected.*

To keep the processing pipeline updated, we need to keep to trigger the modified signal anytime, like in SArucoTracker.

### videoQt/editor

*Properly handle button actions on choose device.*

- Use accept() and reject QtDialog slots instead of our own onValidate() and generic close()
- In SCamera, check the result of exec dialog window to check if it's canceled and don't continue to configure the camera if so.

### Mesh

*Do not compute normals with point based meshes.*

We are not supposed to compute normals when displaying a point based mesh only, however the condition testing this was wrong in the code.

### fwRenderOgre

*Missing headers.*

### material

*Ensure Common.{program,materials} are parsed first.*

Depending on your file system, the `Common.program` could be parsed after the `Video.program`, causing it to fail because it needs `TransferFunction_FP`, which lies inside Common.program to be declared first.

### IDVR

*Compute the countersink geometry in world space.*

We changed the way the MImP IDVR countersink geometry (CSG) is defined/computed:

* CSG used to have a fixed viewport radius, it now has a fixed angle and isn't resized when zooming with the camera.
* Depth lines now start at the importance zone and are in the same unit as the image's spacing.
* The CSG border had to be removed because we couldn't easily adapt it to this new method :crying_cat_face:
* Greyscale CSG and modulation are now separate.

### SMesh

*Build error with GCC 5.4.0.*

### SAxis

*Make the visibility changeable and fix adaptor stop.*

### RayTracingVolumeRenderer

*Do not delay the resize of the viewport.*

Delaying the resize of the entry points textures broke the auto-stereoscopic rendering. This was introduced recently in 6e2946 but was actually not necessary and did not fix anything.

## New features:

### uiPreferences

*Handle floating value in preferences.*

SPreferencesConfiguration only handles integer values.

 - number configuration element is now deprecated it has to be replaced by int
 - add double configuration element to handle float/double type (min: -1000000.0 max:1000000.0, decimals: 6)

### SSignalShortcut

*Create new service to handle shortcuts.*

A new SSignalShortcut service in fw4spl has been added. This service allows to map keys or combination of keys to the trigger of a signal.

### dicom

*Add dicom_reference in Image and Model Series.*

The purpose of this commit is to keep DICOM tags into fw4spl data and use them to create back valid DICOM to save image and/or models.

- Added new example ExDicomSegmentation to generate a ImageSeries mask and a ModelSeries
- Removed `::boost::filesystem::path`in DicomSeries
- Added BufferObject in DicomSeries to store Dicom data
- Updated gdcm/dcmtk reader/writer and unit tests
- Updated `::opImageFilter::SThreshold`to `::fwServices::IOperator`(used in ExDicomSegmentation)
- Updated `::opVTKMesh::SVTKMesher`to `::fwServices::IOperator`(used in ExDicomSegmentation)
- Added dicom_reference in ModelSeries and ImageSeries
- Added new MedicalData version V11

### Calibration

*Add charuco calibration.*

Add ChArUco board calibration in ARCalibration:
* New Bundle with services related to Charuco calibration
* Brand new utility to generate charuco board.
* New activity in ARCalibration
* ARCalibration has been updated to version 0.5
* Both standard calibration and charuco calibration displays now reprojection error when calibration (intrinsic and extrinsic) is performed.
* videoCalibration Bundles were moved from video folder to calibration folder.

### SPoseFrom2D

*Add a points list data containing the corners of the marker model.*

SPoseFrom2D now provides an inout data that can be used to retrieve the
3D geometry of the marker model.

A mistake was also corrected in hand eye calibration, that called `SOpenCVIntrinsic` instead of `SSolvePnP`. In that case, the camera calibration was overwritten by the first service. Now, it just finds the
pose of a chessboard model in the camera only, without calling the camera calibration service (what we really want to).

### MedicalData

*Update fw4spl-ar data to V13AR.*

- This commit adds a new data version V13AR for AR data
- This new V13AR is require to manage new ModelSeries & ImageSeries with Dicom reference (fw4spl!259)

### SLandmarks

*Add adaptor to display landmarks.*

The new adaptor SLandmarks displays landmarks with Ogre generic scene.

### SLine

*Allow length update via a slot.*

A `updateLength()` slot was implemented to update of the length of the rendered line.

## Refactor:

### ioAtoms

*Find the correct version without an XML parameter.*

Improve `::ioAtoms::SReader`and `::ioAtoms::SReader`to find the correct
data version without setting an XML parameter, only the 'patcher' tag is
required to use the patch system.

When no version is defined in SReader and SWriter, the current version
of MedicalData defined in fwMDSemanticPatch is used. This version can be
overridden by the new method 'setCurrentVersion'. You can still define
your own version and context.

### CMakeLists.txt

*Add discovery of additional repositories.*

Setting the CMake variable ADDITIONAL_PROJECTS was tedious and error-prone. Now we explore the folders at the same level of FW4SPL to find extra repositories. Then a CMake option, set to ON by default, is proposed to enable/disable the repository. This will make CMake configuration phase easier than ever !

### deprecated

*Replace getObject by getInput or getInOut.*

- Replace deprecated `getObject()` by `getInput()` or `getInOut()`
- Add deprecated log if the key is not correct in the configuration.
- Set the services `ExternalDataReaderService`, `SInitNewSeries` and `SSeries` as deprecated
- Improve the `FW_DEPRECATED` macros to display the version where the support will be discontinued
- Add a new macro `FW_DEPRECATED_KEY(key, access, version)` to define the correct 'in/inout' key.

All XML configurations have not been updated, so expect to see more [deprecated] mentions in the log. Please fix your application as required.

### SSeriesDBMerger

*Replace getObject.*

### ioAtoms

*Find the correct version without an XML parameter.*

Override the current version of fwMDSemanticPatch to use the AR version.
Clean the useless IO configuration.

### Synchronization

*Improve the synchronization for augmented-reality.*

We reworked the way we synchronize the video frames and the extracted data in real-time.

So far, we have made an extensive use of timelines. First the video grabbers store the frames in timelines. Then we process some algorithms on them and we store all the extracted data (markers, transforms, etc...) in timelines as well. At the end, we rely on ``::videoTools::SFrameMatrixSynchronizer``to pick frames, matrices, etc... at the same timestamp and give these synchronized objects to the renderer, i.e. the generic scene.

However this does not work well. First this is very tedious to work with timelines, since we need to create a dedicated C++ class for each kind of data we want to manage. For some big data, like meshes, we never did it because this would consume too much memory. And some services are simply not well coded and work directly on the data instead of timelines, etc... Eventually, the renderer even screws up the synchronization since all updated objects request the rendering to be done.

So we propose a different approach here. First we restrict the usage of timelines to synchronize video grabbers together, for instance when you
use a camera with multiple sensors or simply several cameras. After that point, all algorithm process the data directly. A new data ``::arData::MarkerMap``is introduced to store a list of markers, since this was the only "data" that only existed in a "timeline" version. To synchronize the results of the algorithms, we propose a new service called `SSignalGate`. This service waits for several signals to be triggered before sending itself a signal, which indicates everyone before is done. This service is typically used to inform the renderer that it must send everything to the GPU right now. To achieve this, we introduced a new rendering mode in `::fwRenderVTK::SRender`.

You can try Ex04SimpleARCV which uses the new design, but for now everything is backward compatible. But we strongly encourage to have a look at this
very soon and try to port your application to benefit of this improvement.

### SExportWithSeriesDB

*Remove getObject.*

### MesherActivity

*Refactor CGogn and VTK mesher.*

- This commit removed dependency to bundle opVTKMesh in opPOCMesher
- CGoGNMesher is now a standard ::fwServices::IOperator
- Updated MesherActivity config with new VTK/CGoGN mesher API

Refactor MesherActivity

### Interactors

*Allow adaptors to be an interactor implementation.*

This is a first step in the refactor of interactors. We plan to implement interactors directly instead of using the only one SInteractorStyle that instantiates sub-classes. It is actually more complicated than if the interactor does the job directly.

In ARPerfusion, we had to create a new interactor to select regions (ARPerfusion!10). We wanted to implement it as an adaptor, which allows us to test if the design works. So we modify the inheritance to allow adaptors to behave as an interactor directly. Consider this as a temporary step in the migration of interactors, where both solutions are possible.

Besides this, there are some changes that might seem unrelated but they were necessary for our new interactor. There is a first fix to allow all kind of meshes to be displayed with a SPointList adaptor. Then there is a second commit to fix the cell color textures, which were not correctly fetched from the texture used to store them.


