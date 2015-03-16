/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/bind.hpp>

#include <fwCore/base.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/macros.hpp>

#include "visuVideoAndroid/Plugin.hpp"

namespace visuVideoAndroid
{


static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::visuVideoAndroid::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{
}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{
}

} // namespace visuVideoAndroid
