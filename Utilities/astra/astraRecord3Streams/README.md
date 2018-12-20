# AstraRecord3Streams

## Purpose

The aim is to take snapshot of the 3 sensors at same time (infrared + depth + color).

Since Astra cameras are not able to give infrared and color at same time, we should open/close color and infrared streams after each frames, that's why the FPS is extremly low.

However this is only usefull for strong calibration purpose, so if target is not moving, low fps is not an issue.

Make sure only one Astra is plugged before running application.

## How it works

### Command line arguments:

```
$ astraRecord3Streams -f saved/image/folder/path -d (0, 1, ...)
```

* **-f** : path to the folder in which images will be written default `./astraRecord/` if path does not exist it will be created.
* **-d** : num of the rgb device of the astra default `0`

### In-app commands:

Focus should be on one of the three windows.

 * Press `q` or `ESC` to quit the application.
 * Press `r` or `SPACE` to take a snapshot.

 