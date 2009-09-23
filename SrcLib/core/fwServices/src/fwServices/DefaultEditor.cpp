/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/IEditionService.hpp"
#include "fwServices/DefaultEditor.hpp"
#include "fwServices/macros.hpp"

namespace fwServices
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::fwServices::IEditionService, ::fwServices::DefaultEditor, ::fwTools::Object    ) ;

//-----------------------------------------------------------------------------

DefaultEditor::DefaultEditor() throw()
{}

//-----------------------------------------------------------------------------

DefaultEditor::~DefaultEditor() throw()
{}

//-----------------------------------------------------------------------------

void DefaultEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
	SLM_TRACE_FUNC();

	SLM_FATAL("ACH : Comments follow lines. This method is not used in framework");

//	::fwServices::ObjectMsg::sptr msg ( new ::fwServices::ObjectMsg( this->getObject() ) ) ;
//	msg->addModif( _msg ) ;
//	this->notify( msg ) ;
}

//-----------------------------------------------------------------------------

void DefaultEditor::starting()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void DefaultEditor::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void DefaultEditor::configuring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void DefaultEditor::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void DefaultEditor::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void DefaultEditor::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

}
