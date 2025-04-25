# sight::module::sync

Contains services that control execution flow to synchronize various data streams (frames, matrix, timelines...) with sight signals and slots.

## Services

* **synchronizer**: The synchronizer service synchronizes video frames and/or transform matrices.
 This service takes a set of frames timelines (**::data::frame_tl**) and matrices timelines (**::data::matrix_tl**).
 The timelines contents are synchronized when some data are added, and when the synchronization is required, 
 which can be automated or through slot.
 Once synchronized, the timeline data are pushed in output variables,(**data::image**) for frames and (**data::matrix4**) for matrices,
  which are associated to their respective timeline through the configuration.

## How to use it

### CMake

```cmake
add_dependencies(my_target module_sync ... )
```

### XML

Please find here some example of frame matrix synchronizer
```
<service type="sight::module::sync::synchronizer" auto_connect="true">
    <in group="frameTL">
        <key uid="frameTL1" />
        <key uid="frameTL4" delay="36" />
        <key uid="frameTL6" />
    </in>
    <inout group="frames">
        <key uid="frame1" sendStatus="true" />
        <key uid="frame6" tl="2" />
        <key uid="frame4" tl="1" sendStatus="false"/>
        <key uid="frame11" tl="0"  sendStatus="true" />
    </inout>
    <in group="matrixTL">
        <key uid="matrixTL1" />
        <key uid="matrixTL2" delay="56" />
    </in>
    <inout group="matrix">
        <key uid="matrix0" index="1" sendStatus="true" >
        <key uid="matrix1" />
        <key uid="matrix2" tl="1" index="0" sendStatus="false"/>
        <key uid="matrix3" tl="1" index="1"/>
        <key uid="matrix4" tl="0" index2"/>
    </inout>
    <tolerance>500</tolerance>
    <autoSync>true</autoSync>
</service>
```
