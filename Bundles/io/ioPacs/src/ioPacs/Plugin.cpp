/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioPacs/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/macros.hpp>

namespace ioPacs
{

using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("ioPacs::Plugin");

Plugin::~Plugin() noexcept
{
}

void Plugin::start()
{
}

void Plugin::stop() noexcept
{
}

} // namespace ioPacs
