# sight::module::io::igtl

Contains services which allow to communicate with the OpenIGTLink network communication protocol.  
Please refer to [OpenIGTLink official website](http://openigtlink.org/) for more detailed information about this network protocol.

## Services

This module mainly contains services that can receive or send objects. Objects converters can be found in the
`sight::io::igtl` library. Please refer to the README.md of this library to get the list of supported data. The services can be either be used in client or server mode whatever the direction of communication. Thus, four generic services are provided and can be used for any
IGTL message except TDATA, for which a specific service **tdata_listener** exists.

- **client_listener**: OpenIGTLink client that will listen objects to the connected server
- **server_listener**: OpenIGTLink server that will listen objects from the connected clients
- **client_sender**: OpenIGTLink client that will send objects to the connected server
- **server_sender**: OpenIGTLink server that will send objects to the connected clients

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_io_igtl ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
