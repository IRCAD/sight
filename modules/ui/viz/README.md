# sight::module::ui::viz

Contains all user interface widgets related to scene3d (Ogre).

## Services

- **compositor_parameter_editor**: allows to edit shader uniform from a compositor material.

- **compositor_selector**: allows to select an Ogre Compositor and apply it to a layer.

- **core_compositor_editor**: allows to select an Ogre Compositor and apply it to a layer.

- **light_editor**: creates a user interface to manage a light adaptor.

- **light_selector**: selects light adaptor from a selected layer or create a new one.

- **material_selector**: allows to select an Ogre material template and apply it to the current `data::reconstruction`.

- **screen_selector**: selects a screen and sends its index.

- **shader_parameter_editor**: allows to edit each parameters from each shader of a `data::reconstruction`.

- **stereo_selector**: allows to select the stereo mode of an Ogre Compositor.

- **stereo_toggler**: enables/disables stereo in an ogre scene layer.

- **texture_selector**: allows to select a `data::image` and apply it to the current reconstruction as an Ogre texture.

## Other classes

- **helper/Utils**: contains functions to convert from Ogre to Qt color format.

- **helper/ParameterEditor**: contains static function  to create a config that will be used to expose a shader parameter adaptor in a parameters.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_ui_viz ... )
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.