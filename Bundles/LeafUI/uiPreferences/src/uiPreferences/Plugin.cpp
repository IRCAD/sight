/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include "uiPreferences/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/macros.hpp>

namespace uiPreferences
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::uiPreferences::Plugin");

} // namespace uiServer
