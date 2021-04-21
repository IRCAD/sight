# sight::activity

This is one of the most important library in _Sight_. It allows to build, to configure, and to launch _activities_:

- **Builder**: contains the base interface for the data ActivitySeries builder.
- **Extension**: 
- **Validator**: 

## Classes

### Activity builders

### Activity interfaces

## How to use it

### CMake

```cmake
target_link_libraries(myTarget <PUBLIC|PRIVATE> activity)
```

### XML

#### Activity configuration

An activity is defined by the extension "::sight::activity::extension::Activity". It is used to launch an
AppConfig with the selected data, it will create a new data data::ActivitySeries that inherits from a
data::Series.

The service activity::action::SLauncher allows to launch an activity. Its role is to create the specific
Activity associated with the selected data.

This action should be followed by the service module::ui::qt::editor::SDynamicView : this service listens the action
signals and launches the activity in a new tab.

- activity::action::SLauncher: uses the selected data to generate the activity.
- module::ui::qt::editor::SDynamicView: displays the activity in the application.
- data::Vector contains the set of selected data .
 
Example of configuration:

```xml
<extension implements="::sight::activity::extension::Activity">
    <id>myActivityId</id>
    <title>3D Visu</title>
    <desc>Activity description ...</desc>
    <icon>sight::module::ui::icons/icon-3D.png</icon>
    <requirements>
        <requirement name="param1" type="data::Image" /> <!-- defaults : minOccurs = 1, maxOccurs = 1-->
        <requirement name="param2" type="data::Mesh" maxOccurs="3" >
            <key>Item1</key>
            <key>Item2</key>
            <key>Item3</key>
        </requirement>
        <requirement name="param3" type="data::Mesh" maxOccurs="*" container="vector" />
        <requirement name="imageSeries" type="data::ImageSeries" minOccurs="0" maxOccurs="2" />
        <requirement name="modelSeries" type="data::ModelSeries" minOccurs="1" maxOccurs="1">
            <desc>Description of the required data....</desc>
            <validator>activity::validator::ImageProperties</validator>
        </requirement>
        <requirement name="transformationMatrix" type="data::Matrix4" minOccurs="0" maxOccurs="1" create="true" />
             <!-- ...-->
    </requirements>
    <builder>activity::builder::ActivitySeries</builder>
    <validator>activity::validator::ImageProperties</validator>
    <appConfig id="myAppConfigId">
        <parameters>
            <parameter replace="registeredImageUid" by="@values.param1" />
            <parameter replace="orientation" by="frontal" />
            <!-- ...-->
        </parameters>
     </appConfig>
</extension>
```

#### Activity parameters

The activity parameters are (in this order):
- **id**: activity unique identifier.
- **title**: activity title that will be displayed on the tab.
- **desc**: description of the activity displayed by the SLauncher.
- **icon**: path to the activity icon displayed by the SLauncher.
- **requirements**: list of the data required to launch the activity.
  - **requirement**: a required data.
    - **name**: key used to add the data in the activity Composite.
    - **type**: the data type (ex: ``data::ImageSeries``).
    - **minOccurs** (optional, "1" by default): the minimum number of occurrences.
    - **maxOccurs** (optional, "1" by default): the maximum number of occurrences.
    - **container** (optional, "vector" or "composite", default: composite): container used to contain the data if minOccurs or
            maxOccurs are not   "1". If the container is "composite", you need to specify the "key" of each object in the composite.
    - **create** (optional, default "false"): if true and (minOccurrs == 0 && maxOccurs == 1), the data will be automatically 
            created if it is not present.
    - **desc** (optional): description of the parameter
    - **validator** (optional): validator to check if the associated data is well formed (inherited of ::fwAtivities::IObjectValidator)
- **builder**: implementation of the activity builder. The default builder is ``activity::builder::ActivitySeries`` : it creates the
        ``data::ActivitySeries`` and adds the required data in its composite with de defined key.
        The builder ``activity::builder::ActivitySeriesInitData`` allows, in addition to what the default builder
        does, to create data when minOccurs == 0 et maxOccurs == 0.
- **validators** (optional): it defines the list of validator. If you need only one validator, you don't need the "validators"
        tag (only "validator").
- **validator** (optional): it allows to validate if the selected required object are correct for the activity.
        For example, the validator ``activity::validator::ImageProperties`` checks that all the selected images
        have the same size, spacing and origin.
- **appConfig**: it defines the AppConfig to launch and its parameters
- **id**: identifier of the AppConfig
- **parameters**: list of the parameters required by the AppConfig
  - **parameter:** defines a parameter
    - **replace**: name of the parameter as defined in the AppConfig
    - **by**: defines the string that will replace the parameter name. It should be a simple string (ex.
            frontal) or define a sesh@ path (ex. @values.myImage). The root object of the sesh@ path if the
            composite contained in the ActivitySeries.

 * @section Validators Validators
 *
 * There is three types of validator :
 *
 * @subsection prebuild Pre-build validator
 *
 * This type of validators checks if the current selection of data is correct to build the activity. It inherits of
 * activity::IValidator and must implement the methods:
 *
 * @code {.cpp}
   ValidationType validate(
           const activity::extension::ActivityInfo& activityInfo,
           SPTR(data::Vector) currentSelection ) const;
   @endcode
 *
 * @subsection activity Activity validator
 *
 * This type of validator checks if the data::ActivitySeries is correct to launch its associated activity.
 * It inherits of activity::IActivityValidator and must implement the method:
 *
 * @code {.cpp}
   ValidationType validate(const CSPTR(data::ActivitySeries) &activity ) const;
   @endcode
 *
 * The validator activity::validator::DefaultActivity is applied if no other validator is defined. It checks if
 * all the required objets are present in the series and if all the parameters delivered to the AppConfig are present.
 *
 * It provides some method useful to implement your own validator.
 *
 * @subsection object Object validator
 *
 * This type of validator checks if the required object is well formed. It can check a single object or a Vector or
 * a Composite containing one type of object. It inherits of activity::IObjectValidator and must implement the
 * method:
 *
 * @code {.cpp}
   ValidationType validate(const CSPTR(data::Object) &currentData ) const;
   @endcode
 */