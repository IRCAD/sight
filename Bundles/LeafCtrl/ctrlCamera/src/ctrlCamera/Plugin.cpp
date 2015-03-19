#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include "ctrlCamera/Plugin.hpp"

namespace ctrlCamera
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::ctrlCamera::Plugin");

Plugin::~Plugin() throw()
{
}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
}

void Plugin::stop() throw()
{
}

} // namespace ctrlCamera
