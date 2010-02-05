/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/ProcessObject.hpp>
#include <fwData/Object.hpp>

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/ProcessObjectEditor.hpp"
#include "fwComEd/PoMsg.hpp"

namespace fwComEd
{

ProcessObjectEditor::~ProcessObjectEditor() throw()
{}

void ProcessObjectEditor::updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed)
{
    SLM_FATAL("ACH: must report ObjectMsg API modification. This method is used ?");
//  std::cout << "ProcessObjectEditor::objectModified( const ObjectEvent & event )" << std::endl;
//   ::boost::shared_ptr< ::fwData::ProcessObject > m_client = this->getObject< ::fwData::ProcessObject>() ;
//   ::boost::shared_ptr< fwComEd::ProcessObjectMsg > poMsg( new fwComEd::ProcessObjectMsg( m_client ) ) ;
//  poMsg->addIoModif( _msg ) ; //One assumes that event is related to a modification of either an input or an output
//
//  this->notify( poMsg ) ;
}

//-----------------------------------------------------------------------------

void ProcessObjectEditor::starting()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ProcessObjectEditor::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ProcessObjectEditor::configuring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ProcessObjectEditor::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ProcessObjectEditor::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ProcessObjectEditor::info(std::ostream &_sstream )
{
    _sstream << "ProcessObjectEditor" ;
}

//-----------------------------------------------------------------------------

}


