# sight::module::memory

The namespace sight::module::memory contains services to manage memory and lock objects to prevent dump.


## Services

- **LockDumpSrv** : performs dumps-locks on objects. They are unlocked when the service stops.
- **SDumpPolicy** : configures dumping policy.

## How to use it

### CMake

```cmake
target_link_libraries(myTarget <PUBLIC|PRIVATE> module_memory)
```

### XML

```xml
   <service type="sight::module::memory::LockDumpSrv">
       <inout key="target" uid="..." />
   </service>
```