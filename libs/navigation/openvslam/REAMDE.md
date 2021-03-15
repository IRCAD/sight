# Openvslam lib

`Openvslam` lib is a library which allows to handle passages between the sight framework elements and Openvslam ones. 

Plugin id is `sight::navigation::openvslam`

## How to use it

Include the target in the cmake file, and the Helper header file.
All methods are static, call them directly to convert sight Camera objects to openvslam camera forward and backward, and writte and load openvslam configuration.  




### Cmake

```cmake

add_dependencies(myTarget 
                ...
                 navigation_openvslam
)

```

