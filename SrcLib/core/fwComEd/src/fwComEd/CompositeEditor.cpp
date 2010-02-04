/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Composite.hpp>
#include <fwData/Object.hpp>

#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/macros.hpp>

#include "fwComEd/CompositeMsg.hpp"
#include "fwComEd/CompositeEditor.hpp"

namespace fwComEd
{

//-----------------------------------------------------------------------------

CompositeEditor::~CompositeEditor() throw()
{}

//-----------------------------------------------------------------------------

void CompositeEditor::updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed)
{
	SLM_FATAL("ACH: must report compositeMsg API modification. This method is used ?"); // If comment this fatal, reactivate cppunit test (CompositeMessageTest::methodeMessageNotification)
//	//std::cout << "CompositeEditor::objectModified( const ObjectEvent & event )" << std::endl;
//	::boost::shared_ptr< ::fwData::Composite > m_client = this->getObject< ::fwData::Composite >() ;
//	::boost::shared_ptr< ::fwComEd::CompositeMsg > poMsg( new fwComEd::CompositeMsg( m_client ) ) ;
//	poMsg->addModif( _msg ) ;	//One assumes that event is related to a modification of either an input or an output
//
//	this->notify( poMsg ) ;
//	//::fwServices::IEditionService::notify(this->getSptr(),m_client, poMsg);
}

//-----------------------------------------------------------------------------

void CompositeEditor::starting()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void CompositeEditor::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void CompositeEditor::configuring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void CompositeEditor::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void CompositeEditor::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void CompositeEditor::info(std::ostream &_sstream )
{
	_sstream << "CompositeEditor" ;
}

//-----------------------------------------------------------------------------

}


