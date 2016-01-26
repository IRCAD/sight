/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
