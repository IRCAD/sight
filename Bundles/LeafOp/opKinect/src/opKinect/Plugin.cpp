#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include "opKinect/Plugin.hpp"

namespace opKinect
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::opKinect::Plugin");


/**
 * @brief destructor
 */
Plugin::~Plugin() throw()
{
}

/**
 * @brief start plugin
 */
void Plugin::start() throw( ::fwRuntime::RuntimeException )
{
}

/**
 * @brief stop plugin
 */
void Plugin::stop() throw()
{
}

} // namespace opKinect
