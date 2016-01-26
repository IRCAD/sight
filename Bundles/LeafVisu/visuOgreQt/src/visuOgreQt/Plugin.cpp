/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreQt/Plugin.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwCore/spyLog.hpp>

#define RESOURCES_PATH  "./Bundles/visuOgreQt_0-1/resources.cfg"

namespace visuOgreQt
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("visuOgreQt::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{
}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    ::fwRenderOgre::Utils::addResourcesPath( RESOURCES_PATH );
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{
}

//-----------------------------------------------------------------------------

} // namespace visuOgreQt
