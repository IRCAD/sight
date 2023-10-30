# sight::io::session

Library for writing and reading recursively a root data object, including all fields, from/to a session file.

The session file is indeed a standard "ZIP" archive, while the compression algorithm for files inside the session
archive is ZSTD. A standard archive reader could open a session file, if it is able to handle ZIP archive with ZSTD
compression

The archive can be password protected using AES256 algorithm and the compression level is set individually, depending
of the type of data to serialize.

The entry points are `session_reader` and `session_writer`. They directly wrap `detail\Session[De]Serializer`, whose
contain the main \[de\]serialization algorithms. These algorithms call, for each data object, a specific serialization
and de-serialization function, stored in a separated header file (all of them are stored in `detail`).


## Classes

### Helper

- **PasswordKeeper**: keeps passwords in memory in a somewhat secure way (encrypted), so direct reading in memory is a bit more difficult. It can manage a global per application password and as many per `PasswordKeeper` instance.

### Reader / Writer

- **session_reader**: reads a root data object from a session file on disk.
- **session_writer**: writes a root data object to a session file on disk.

## How to use it

### Writing

```c++
    auto object = sight::data::string::New("Sample");
    auto sessionWriter = io::session::session_writer::New();

    // Configure the session writer
    sessionWriter->set_object(object);
    sessionWriter->setFile("Sample.zip");

    // Setting a password means the session will be encrypted
    sessionWriter->set_password("Password");

    // Write
    sessionWriter->write();
```

### Reading

```c++
    auto sessionReader = io::session::session_reader::New();

    // Configure the session reader
    sessionReader->setFile(testPath);

    // Setting a password means the session will be decrypted
    sessionReader->set_password(password);

    // Read the session
    sessionReader->read();

    // Retrieve the object
    auto object = sessionReader->get_object();
```

### CMake

```cmake
target_link_library(my_target <PUBLIC|PRIVATE> io_session)
```



