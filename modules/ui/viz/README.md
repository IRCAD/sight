# sight::module::ui::viz

Contains all user interface widgets related to scene3d (Ogre).

## Services

* **SAddPoint**
Adds points in a `data::PointList`

* **SCompositorParameterEditor**
Allows to edit shader uniform from a compositor material.

* **SCompositorSelector**
Allows to select an Ogre Compositor and apply it to a layer.

* **SCoreCompositorEditor**
Allows to select an Ogre Compositor and apply it to a layer.

* **SLightEditor**
Creates a user interface to manage a light adaptor.

* **SLightSelector**
Selects light adaptor from a selected layer or create a new one.

* **SMaterialSelector**
Allows to select an Ogre material template and apply it to the current `data::Reconstruction`.

* **SScreenSelector**
Selects a screen and sends its index.

* **SShaderParameterEditor**
Allows to edit each parameters from each shader of a `data::Reconstruction`.

* **SStereoSelector**
Allows to select the stereo mode of an Ogre Compositor.

* **SStereoToggler**
Enables/disables stereo in an ogre scene layer.

* **STextureSelector**
Allows to select a `data::Image` and apply it to the current reconstruction as an Ogre texture.
    
## Other classes

* **helper/Utils**
Contains functions to convert from Ogre to Qt color format.

* **helper/ParameterEditor**
Contains static function  to create a config that will be used to expose a shader parameter adaptor in a SParameters.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_ui_viz ... )
```

### XML

Please see doxygen of each service to know more about how to use it in xml configurations.