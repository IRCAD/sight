/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwRuntime/IPlugin.hpp"


namespace fwRuntime
{



IPlugin::~IPlugin()
{}


void IPlugin::initialize() throw(::fwRuntime::RuntimeException)
{
}

void IPlugin::uninitialize() throw(::fwRuntime::RuntimeException)
{
}


} // namespace fwRuntime
