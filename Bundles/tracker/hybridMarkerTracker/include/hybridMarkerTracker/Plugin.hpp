#pragma once

#include <fwRuntime/Plugin.hpp>

namespace hybridMarkerTracker
{

class HYBRIDMARKERTRACKER_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Plugin destructor
    ~Plugin() noexcept;

    /// This method is used by runtime to start the bundle.
     void start();

    /// This method is used by runtime to stop the bundle.
     void stop() noexcept;
};

} // namespace hybridMarkerTrack