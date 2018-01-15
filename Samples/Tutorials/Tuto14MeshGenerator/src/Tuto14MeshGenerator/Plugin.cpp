/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Tuto14MeshGenerator/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace Tuto14MeshGenerator
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::Tuto14MeshGenerator::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//-----------------------------------------------------------------------------

} // namespace Tuto14MeshGenerator
