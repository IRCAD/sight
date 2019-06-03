#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include "hybridMarkerTracker/Plugin.hpp"
#include <fwServices/macros.hpp>

namespace hybridMarkerTracker
{

//-----------------------------------------------------------------------------

    static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::hybridMarkerTracker::Plugin");

//-----------------------------------------------------------------------------

    Plugin::~Plugin() noexcept
    {
    }

//-----------------------------------------------------------------------------

    void Plugin::start()
    {
    }

//-----------------------------------------------------------------------------

    void Plugin::stop() noexcept
    {
    }

//-----------------------------------------------------------------------------

} // namespace hybridMarkerTracker