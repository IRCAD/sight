/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include <fwData/Composite.hpp>
#include <fwData/ProcessObject.hpp>
#include <fwData/Image.hpp>
#include <fwData/Video.hpp>

#include "dataReg/Plugin.hpp"

namespace fwDataReg
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::dataReg::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
	// HACK ???? data lib should be loaded !!!
	REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object,  ::fwData::ProcessObject,  	::fwData::ProcessObject);
	REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object,  ::fwData::Image, 			::fwData::Image );
	REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object,  ::fwData::Composite, 		::fwData::Composite );
	REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object,  ::fwData::Video, 			::fwData::Video );
	// etc ?not necessary to declare ohter data ?
}

void Plugin::stop() throw()
{}

} // namespace operators
