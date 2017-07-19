/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/macros.hpp>

#include <ioCalibration/Plugin.hpp>

namespace ioCalibration
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin>
registrar("::ioCalibration::Plugin");

Plugin::~Plugin() noexcept
{
}

void Plugin::start()
{
}

void Plugin::stop() noexcept
{
}

} // namespace ioCalibration
