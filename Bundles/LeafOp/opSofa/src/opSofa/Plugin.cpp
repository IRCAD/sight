#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include "opSofa/Plugin.hpp"

namespace opSofa
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::opSofa::Plugin");


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

} // namespace opSofa
