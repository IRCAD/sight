# sight::io::http

Library containing classes to work with http protocol.

## Classes:
-**ClientQt**: defines an HTTP client using Qt Network.
-**Request**: defines an HTTP request.

### exceptions
This sub-folder contains classes defining exceptions.
-**Base**: defines the base call for all exceptions in this module.
-**ConnectionRefused**: implements exception for an HTTP connection refused.
-**ContentNotFound**: implements exception for HTTP content not found errors.
-**HostNotFound**: implements exception for an HTTP host not found errors.

### helper

-**Series**: defines methods to help converting data::dicom_series to http responses and more. 


## How to use it

### CMake

```cmake

target_link_libraries( my_target <PUBLIC|PRIVATE> io_http)

```

