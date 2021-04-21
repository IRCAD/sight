# sight::io::atoms

Contains the core class to read and write atoms data. 
The usage of atoms allows to take data version into account. 
Data are written with a version, and, when loaded, if the version differs, the patches allows to upgrade them to correspond to the up-to-date version. 

## Classes

- **Reader**: reads atoms data from a file and convert them to atoms and then to sight native data.
- **Writer**: writes a set of sight native data into atoms into a file.


### _filter_

- **new**: generic constructor for atoms reader and writer with restrictions on objects construction.
- **detail**: internal mechanism: provides the instances of the factory registry.
- **macro**: contains a filter macro for the atoms registry.
- **functions**: verifies if a givent atom matches a `sight::data::Series`.
- **IFilter**: removes atom attributes which are not managed by a context.

### _patch_

Defines classes which allows to convert atoms of different versions from one version to another. 

- **new**: generic constructor for atoms patcher with restrictions on objects construction.
- **detail**: internal mechanism: provides the instances of the factory registry.
- **DefaultPatcher**: default patcher for data upgrade or downgrade when no other patcher exist.
- **IPatcher**: generic interface which defines patchers to migrate atoms from a version to an other one.

- **IPatch**: generic interface for all patches.
- **ISemanticPatch**: base class of contextual patches.
- **IStructuralCreator**: base class of all structural creators. Creates a new atom object with the default values corresponding to a data object.
- **IStructuralPatch**: base class of all structural patches.
- **LinkDescriptor**: identifies links between two atoms version.
- **PatchingManager**: manages object patching.
- **SemanticPatchDB**: contextual patch database. All contextual patches are stored in this database.
- **StructuralCreatorDB**: structural creator database. All structural creators are stored in this database.
- **IStructuralPatch**: generic interface for structural patch database.
- **VersionDescriptor**: version descriptor used to identify a version.
- **VersionsGraph**: versions graph description.
- **VersionsManager**: maintains a table of object versions and a table of links between objects.


- **Abstract**: defines a condition in the patch.
- **NumericOverflow**: defines a condition on numeric value in the patch.

- **Base**: base class of atoms treatment exceptions.
- **BadExtension**: exception defining a wrong extension in a filename.
- **ImpossibleConversion**: reports an impossible conversion between data objects.
- **MissingInformation**: reports a missing information required for data object patching.
- **UnknownVersion**: reports an unknown version or a version which can not be processed.

- **functions**: helper for multiple patch tasks (version, Id, Classname, UID handling,....) .
- **Object**: helper for atoms Objects.

- **Logger**: logs events occurring during object patching.


## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE> 
                       io_atoms
)
```


