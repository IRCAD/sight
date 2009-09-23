/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>

#include "uiIO/editor/IOSelectorService.hpp"
#include "uiIO/Plugin.hpp"

namespace uiIO
{

//-----------------------------------------------------------------------------

using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("::uiIO::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
	REGISTER_SERVICE( ::gui::editor::IEditor , ::uiIO::editor::IOSelectorService , ::fwTools::Object ) ;
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{}

//-----------------------------------------------------------------------------

} // namespace operators
