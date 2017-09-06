/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreQt/Plugin.hpp"

#include <fwCore/spyLog.hpp>

#include <fwRenderOgre/Utils.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#define RESOURCES_PATH  BUNDLE_PREFIX "/visuOgreQt_0-1/resources.cfg"

namespace visuOgreQt
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("visuOgreQt::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    ::fwRenderOgre::Utils::addResourcesPath( RESOURCES_PATH );
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//-----------------------------------------------------------------------------

} // namespace visuOgreQt
