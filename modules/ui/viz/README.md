# sight::module::ui::viz

Contains all user interface widgets related to scene3d (Ogre).

## Services

- **SAddPoint**: adds points in a `data::PointList`

- **SCompositorParameterEditor**: allows to edit shader uniform from a compositor material.

- **SCompositorSelector**: allows to select an Ogre Compositor and apply it to a layer.

- **SCoreCompositorEditor**: allows to select an Ogre Compositor and apply it to a layer.

- **SLightEditor**: creates a user interface to manage a light adaptor.

- **SLightSelector**: selects light adaptor from a selected layer or create a new one.

- **SMaterialSelector**: allows to select an Ogre material template and apply it to the current `data::Reconstruction`.

- **SScreenSelector**: selects a screen and sends its index.

- **SShaderParameterEditor**: allows to edit each parameters from each shader of a `data::Reconstruction`.

- **SStereoSelector**: allows to select the stereo mode of an Ogre Compositor.

- **SStereoToggler**: enables/disables stereo in an ogre scene layer.

- **STextureSelector**: allows to select a `data::Image` and apply it to the current reconstruction as an Ogre texture.
    
## Other classes

- **helper/Utils**: contains functions to convert from Ogre to Qt color format.

- **helper/ParameterEditor**: contains static function  to create a config that will be used to expose a shader parameter adaptor in a SParameters.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_ui_viz ... )
```

### XML

Please see doxygen of each service to know more about how to use it in xml configurations.