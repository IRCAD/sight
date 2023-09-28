# sight::activity

This is one of the most important library in _Sight_. It allows to build, to configure, and to launch _activities_:

- **Builder**: contains the base interface for the data Activity builder.
- **Extension**:
- **Validator**: contains the classes defining validators checking properties and requirements.

## Classes

### Activity builders

- **Activity**: defines the default builder for the data Activity.

### Activity interfaces

- **launcher**: defines the interface for services that launches Activity instances.
- **sequencer**: defines the interface for services that creates Activity instances sequentially.
- **builder**: defines the interface for all activity builder's classes.

### Validators

- **base**: defines the interface for all activity validator's classes.
- **activity**: defines the interface for all activity validator's classes.
- **object**: defines the interface for all activity data validator's classes.
- **DefaultActivity**: defines a validator which checks that activity requirements are valid.
- **ImageProperties**: defines a validator which checks that properties of given images are the same.
- **RelatedStudy**: defines a validator which checks that given series refer to the same study.

### Other

- **message**: defines Activity information sent by signal to launch new activities in a tab.

## How to use it

### CMake

```cmake
target_link_libraries(myTarget <PUBLIC|PRIVATE> activity)
```

### XML

Please read the dedicated [Activities](https://sight.pages.ircad.fr/sight-doc/SAD/src/Activities.html) page on
[Sight documentation](https://sight.pages.ircad.fr/sight-doc).
