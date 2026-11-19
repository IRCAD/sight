% SIGHTRUN(1)
% IRCAD
% September 2023

# NAME

sightrun - launcher for _Sight_ applications.

# SYNOPSIS

**sightrun** \[OPTIONS\] \[PROFILE(=profile.xml)\] \[PROFILE-ARGS ...\]

# DESCRIPTION

**sightrun** is the launcher for applications built with _Sight_, the Surgical Image Guidance and Healthcare Toolkit.

# OPTIONS

**-h**, **\--help**
:   Display a friendly help message.

**-B**
:   Adds a module path.

**\--mcp**
:   Serves the GUI to an MCP client on the standard input and output, which then belong to the protocol; the
    application ends when the client closes its input. Only in a build with `SIGHT_ENABLE_UI_MCP`.

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
