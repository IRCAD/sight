# AstraViewer

## Purpose

Basic viewer of Astra camera.

This open two windows, one with the depth image and the other with infrared or color image. (infrared and color images cannot be displayed at same time).

The program can also take snapshot, pause/play camera, and merge depth with infrared/color image.

Make sure only one Astra is plugged before running application.

## How it works

### Command line arguments:

```
$ astraViewer -f saved/image/folder/path -d (0, 1, ...)
```

* **-f** : path to the folder in which images will be written default `./astraRecord/` if path does not exist it will be created.
* **-d** : num of the rgb device of the astra default `0`

### In-app commands:

Focus should be on one of the three windows.

 * Press `q` or `ESC` to quit the application.
 * Press `r` or `SPACE` to take a snapshot.
 * Press `i` to switch between infrared and color (default) stream.
 * Press `m` to merge infrared/color into depth.
 * Press `p` to pause/play.