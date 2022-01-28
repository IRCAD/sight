# sight::activity

This is one of the most important library in _Sight_. It allows to build, to configure, and to launch _activities_:

- **Builder**: contains the base interface for the data ActivitySeries builder.
- **Extension**: 
- **Validator**: contains the classes defining validators checking properties and requirements.

## Classes

### Activity builders

- **ActivitySeries**: defines the default builder for the data ActivitySeries.
- **ActivitySeriesInitData**: defines an activity builder which instantiates a new requirement if minOccurs and maxOccurs are set to 0.

### Activity interfaces

- **IActivityLauncher**: defines the interface for services that launches Activity instances.
- **IActivitySequencer**: defines the interface for services that creates Activity instances sequentially.
- **IActivityValidator**: defines the interface for all activity validator's classes.
- **IBuilder**: defines the interface for all activity builder's classes.
- **IObjectValidator**: defines the interface for all activity data validator's classes.
- **IValidator**: defines the interface for all activity validator's classes.

### Validators

- **DefaultActivity**: defines a validator which checks that activity requirements are valid.
- **ImageProperties**: defines a validator which checks that properties of given images are the same.
- **RelatedStudy**: defines a validator which checks that given series refer to the same study.

### Other

- **ActivityMsg**: defines Activity information sent by signal to launch new activities in a tab.

## How to use it

### CMake

```cmake
target_link_libraries(myTarget <PUBLIC|PRIVATE> activity)
```

### XML

Please read the dedicated [Activities](https://sight.pages.ircad.fr/sight-doc/SAD/src/Activities.html) page on 
[Sight documentation](https://sight.pages.ircad.fr/sight-doc).
