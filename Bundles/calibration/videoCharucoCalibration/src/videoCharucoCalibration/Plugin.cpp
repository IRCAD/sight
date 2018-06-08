/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCharucoCalibration/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>

namespace videoCharucoCalibration
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::videoCharucoCalibration::Plugin");

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

} // namespace videoCharucoCalibration
