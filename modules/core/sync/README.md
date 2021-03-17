# sight::module::sync

Contains services that control execution flow to synchronize various data streams (frames, matrix, timelines...) with sight signals and slots.

## Services

* **SFrameMatrixSynchronizer**
 Synchronizes video frames from a (::data::FrameTL) and tracking matrix from a (::extData::MatrixTL) into groups of images (data::Image) and matrices (data::Matrix4) which are synchronized.

* **SFrameUpdater**
 Extract a frame from a timeline given a timestamp, or when timelines is updated

* **SMatrixSynchronizer**
 Reads the content of a Matrix timeline (::data::MatrixTL) into a ::sight::data::Matrix4.
 
* **SMatrixTLSynchronizer**
 Extract matrices from a Matrix timeline (::data::MatrixTL) into independent matrices (::data::Matrix4) which are synchronized
 


## How to use it

### CMake

```cmake
add_dependencies(myTarget module_sync ... )
```

### XML

Please find here some exemple of  frame matrix synchroniser
```
<service type="sight::module::sync::SFrameMatrixSynchronizer" autoConnect="yes">
            <in group="frameTL">
                <key uid="frameTL1" />
                <key uid="frameTL4" />
                <key uid="frameTL6" />
            </in>
            <inout group="image">
                <key uid="videoImage1" />
                <key uid="videoImage2" />
                <key uid="videoImage3" />
            </inout>
            <in group="matrixTL">
                <key uid="matrixTL1" />
                <key uid="matrixTL2" />
            </in>
            <inout group="matrices0">
                <key uid="matrix0" sendStatus="true"/>
                <key uid="matrix1" />
                <key uid="matrix2" />
            </inout>
            <inout group="matrices1">
                <key uid="matrix3" sendStatus="false"/>
                <key uid="matrix4" />
            </inout>
            <framerate>30</framerate>
            <tolerance>500</tolerance>
       </service>
```
