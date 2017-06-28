/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "material/Plugin.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>

#define RESOURCES_PATH BUNDLE_PREFIX "/material_0-1/resources.cfg"

namespace material
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::material::Plugin");

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

} // namespace material
