#include "videoRealSense/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace videoRealSense
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::videoRealSense::Plugin");

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

} // namespace videoRealSense
