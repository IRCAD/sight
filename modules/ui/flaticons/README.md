# sight::module::ui::flaticons

`flaticons` is a resource module that contains icons in "flat" style.

## How to use it

### XML

Example in a toolbar:

```xml

<service uid="topToolbarView" type="sight::module::ui::base::SToolBar">
    <gui>
        <layout>
            <menuItem name="Show parameters" icon="sight::module::ui::flaticons/YellowLeftChevron.svg" />
            ...
        </layout>
    </gui>
    <registry>
        <menuItem sid="showParamAction" start="yes" />
    </registry>
</service>
```

### CMake

```cmake

add_dependencies(myTarget
                ...
                 module_ui_flaticons
)

```

## Style guidelines

- All icons must be in the svg format.
- The page size must be 130*130px.
- Margins must be 15px for each border so the drawing must have a size of 100*100px.
- Borders thickness must be 2px.
- Each border as the same color as the background with a lightness divided by 2.
- If an icon is used in another one, it sizes should be 50*50px.
- Colors should fit the given ones:
    - Yellow : 255/198/109.
    - Red : 210/82/82
    - Blue : 105/151/187
    - Orange : 203/119/47
    - Green : 97/150/71
    - Grey : 54/57/62
