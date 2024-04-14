% SIGHTVIEWER(1)
% IRCAD
% September 2023

# NAME

sight_viewer - medical image viewer.

# SYNOPSIS

**sight_viewer** \[OPTIONS\]

# DESCRIPTION

**sight_viewer** is a medical image viewer featuring negatoscope, multi-planar reconstruction, and volume rendering.
Many image formats are handled such as DICOM, mhd, inr.gz, vtk, vti,... Reconstructions can also be imported from DICOM
segmentations or VTK supported formats, and visualized in mixed rendering with images.
Tools are provided to measure distances of structures in the image and to place landmarks.
Last, sight_viewer can also connect directly to a PACS, and then request, download or upload DICOM files.

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
