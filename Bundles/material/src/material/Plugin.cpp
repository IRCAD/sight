/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "material/Plugin.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace material
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::material::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    ::fwRenderOgre::Utils::addResourcesPath( ::fwRuntime::getBundleResourceFilePath("material", "resources.cfg" ) );
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

} // namespace material
