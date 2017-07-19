/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include "validators/Plugin.hpp"

namespace validators
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::validators::Plugin");

Plugin::~Plugin() noexcept
{
}

void Plugin::start()
{
}

void Plugin::stop() noexcept
{
}

} // namespace validators
