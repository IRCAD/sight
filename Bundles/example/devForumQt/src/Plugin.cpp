/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/Base.hpp>

#include <fwRender/IRender.hpp>

#include "devForumQt/Plugin.hpp"

#include "devForumQt/tuto01/ImageViewerService.hpp"

#include "devForumQt/tuto02/ImageViewerService.hpp"
#include "devForumQt/tuto02/ImageReaderService.hpp"

#include "devForumQt/tuto03/ImageViewerService.hpp"
#include "devForumQt/tuto03/ImageReaderService.hpp"

#include "devForumQt/tuto04/ImageReaderService.hpp"
#include "devForumQt/tuto04/ImageReaderActionService.hpp"

namespace devForum
{

using namespace ::fwRuntime::utils;
static GenericExecutableFactoryRegistrar<Plugin> registrar("::devForum::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
//    // Register services of tuto 01
//    REGISTER_SERVICE( ::fwRender::IRender , ::devForum::tuto01::ImageViewerService, ::fwData::Image ) ;

//    // Register services of tuto 02
//    REGISTER_SERVICE( ::fwRender::IRender , ::devForum::tuto02::ImageViewerService, ::fwData::Image ) ;
//    REGISTER_SERVICE( ::io::IReader , ::devForum::tuto02::ImageReaderService, ::fwData::Image ) ;

//    // Register services of tuto 03
//    REGISTER_SERVICE( ::fwRender::IRender , ::devForum::tuto03::ImageViewerService, ::fwData::Image ) ;
//    REGISTER_SERVICE( ::io::IReader , ::devForum::tuto03::ImageReaderService, ::fwData::Image ) ;

//    // Register services of tuto 04
//    REGISTER_SERVICE( ::io::IReader , ::devForum::tuto04::ImageReaderService, ::fwData::Image ) ;
//    REGISTER_SERVICE( ::fwGui::IActionSrv , ::devForum::tuto04::ImageReaderActionService, ::fwData::Image ) ;
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{}

//-----------------------------------------------------------------------------

} // namespace devForum
