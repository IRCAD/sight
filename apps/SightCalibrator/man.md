% SIGHTCALIBRATOR(1)
% IRCAD
% September 2023

# NAME

sighcalibrator - a camera calibration application.

# SYNOPSIS

**sighcalibrator** \[OPTIONS\]

# DESCRIPTION

**sighcalibrator** is a user-friendly application to calibrate mono and stereo cameras. It can take images, video files,
or any supported camera device as input. It detects the presence of a checkerboard in images. The parameters of the checkerboard can be customized. Individual images can then be marked to be used in the computation of
the calibration. Last, the intrinsic and extrinsic parameters can be exported in OpenCV format, Yaml or XML.

# OPTIONS

**-h**, **\--help**
:   Display a friendly help message.

**\--clog**
:   Enable log output to console.

**\--no-clog**
:   Disable log output to console.

**\--flog**
:   Enable log output to file.

**\--no-flog**
:   Disable log output to file.

**\--log-output arg**
:   Log output filename. Default to sight.log.

**\--log-trace**
:   Set log level to trace.

**\--log-debug**
:   Set log level to debug.

**\--log-info**
:   Set log level to info.

**\--log-warn**
:   Set log level to warn.

**\--log-error**
:   Set log level to error.

**\--log-fatal**
:   Set log level to fatal.

# BUGS

For community support, see GitHub Issues: <https://github.com/IRCAD/Sight/issues>.

Also, internal bug reports can be found on <https://git.ircad.fr/sight/sight/-/issues>.
