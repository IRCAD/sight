/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "colourSegmentation/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>

namespace colourSegmentation
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::colourSegmentation::Plugin");

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

} // namespace segmentation
