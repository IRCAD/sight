# sightlog

This is the standalone sight logger which is started as a child detached process by Sight
It uses [minizip-ng](https://github.com/zlib-ng/minizip-ng) with built-in AES encryption as the backend to store the log
and boost log as the frontend.

Alternatively, it can also decrypt the real log from a log archive, which can be useful if a human need to read it back.

## CMake
- Log encryption support is enabled with CMake build options: `SIGHT_ENABLE_ENCRYPTED_LOG` (OFF by default)
- A default hardcoded (but obfuscated) password at compile time can be set by `SIGHT_DEFAULT_PASSWORD` CMake definition.

## Password management

### `SIGHT_DEFAULT_PASSWORD`

It will be used when required (log encryption, preferences, session, ..) by default, until overwritten by user when an
input dialog asking the user for a password is shown. It allows to use encryption, without having to ask user for a
password. BTW, this is **less** secure, as the password is still hardcoded, even if obfuscated in the binary.

### User password
An input dialog asking for a password will be shown when:
- the log is encrypted and no `SIGHT_DEFAULT_PASSWORD` is set **or** if `sightrun` is launched with `--ask-password`
  argument.
- for preferences, if the appropriate `password` and `encryption` `policy` is set in module `ui_base` configuration
  > take a look at `libs/ui/base/Preferences.hpp`.
- for session files, if the appropriate `password` and `encryption` `policy` is set in SReader and SWriter service
  configuration
  > take a look at `modules/io/session/SReader.hpp` and `modules/io/session/SWriter.hpp`.

In all case, the entered password will be kept (obfuscated) in memory, allowing to only ask the password once, until, of
course, it is explicitly configured to not do so.

## sightlog usage

```
Sightlog logger options:
  -h [ --help ]          Display this help message.
  -v [ --version ]       Display the version of the program.
  -i [ --input ] arg     Log archive to extract.
  -p [ --password ] arg  Password to use for encryption and decryption.
  -a [ --ask-password ]  Show a popup to enter the password.
  -d [ --directory ] arg Output directory when extracting a log archive.
```

For example to extract the log file from archive `/home/bozo/sight/bin/sight.log.0.zip` protected with password
`w34r3th3B3st` in directory /home/bozo/logs and  we can use:

```bash
./sightlog -i /home/bozo/sight/bin/sight.log.0.zip -d /home/bozo/logs -p "w34r3th3B3st"
```
