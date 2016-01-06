#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include "calibrationActivity/Plugin.hpp"

namespace calibrationActivity
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::calibrationActivity::Plugin");

Plugin::~Plugin() throw()
{
}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
}

void Plugin::stop() throw()
{
}

} // namespace calibrationActivity
