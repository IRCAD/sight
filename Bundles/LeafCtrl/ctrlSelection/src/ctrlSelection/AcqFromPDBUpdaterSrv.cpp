/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/PatientDB.hpp>
#include <fwData/Composite.hpp>

#include <fwTools/UUID.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/PatientDBMsg.hpp>
#include <fwComEd/CompositeMsg.hpp>

#include "ctrlSelection/AcqFromPDBUpdaterSrv.hpp"

namespace ctrlSelection
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::AcqFromPDBUpdaterSrv, ::fwData::PatientDB    ) ;

//-----------------------------------------------------------------------------

AcqFromPDBUpdaterSrv::AcqFromPDBUpdaterSrv() throw()
 :      m_compositeKey(""),
        m_patientDBUID("")
{
        addNewHandlingEvent( ::fwComEd::PatientDBMsg::NEW_LOADED_PATIENT );
}

//-----------------------------------------------------------------------------

AcqFromPDBUpdaterSrv::~AcqFromPDBUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{

        ::fwComEd::PatientDBMsg::csptr pPatientDBMsg = ::fwComEd::PatientDBMsg::dynamicConstCast( _msg ) ;

        if( pPatientDBMsg && pPatientDBMsg->hasEvent( ::fwComEd::PatientDBMsg::NEW_IMAGE_SELECTED ) )
        {

                // Patient selection
                ::fwData::PatientDB::sptr patientDB = ::fwTools::UUID::get< ::fwData::PatientDB >( m_patientDBUID ) ;
                ::fwData::PatientDB::PatientIterator patientIter = patientDB->getPatients().first;

                // Study selection
                ::fwData::Patient::StudyIterator studyIter = (*patientIter)->getStudies().first;

                // Acquisition selection
                ::fwData::Study::AcquisitionIterator acquisitionIter = (*studyIter)->getAcquisitions().first;

                // Image selection
                // ::fwData::Image::sptr imageSelectedInDB =  (*acquisitionIter)->getImage();

                // Set new selection (Acquisition) to Ctrl
                this->updateCompositeWithAcq(*acquisitionIter);
        }
}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::updateCompositeWithAcq ( ::fwData::Acquisition::sptr _acq )
{

        ::fwData::Composite::sptr pComposite = this->getObject< ::fwData::Composite >();

        ::fwData::Object::sptr backupAcq = pComposite->getRefMap()[ m_compositeKey ];

        // not notify if same acquisition
        if( backupAcq == _acq )
        {
                SLM_FATAL("It is the same object");
        }

        // Change acq of composite visu
        pComposite->getRefMap()[ m_compositeKey ] = _acq;

        // Change image of composite visu
        //pCompositeVisu->getRefMap()[UPDATE_IMAGE] = _acq->getImage();

        // Backup
        std::vector< ::fwData::Object::sptr > oldObjects;
        oldObjects.push_back( backupAcq );
        //oldObjects.push_back( backupImg );

        // Message
        std::vector< std::string > modifiedFields;
        modifiedFields.push_back(m_compositeKey);

        ::fwComEd::CompositeMsg::NewSptr compositeMsg;
        compositeMsg->addEventModifiedFields(modifiedFields,oldObjects);
        ::fwServices::IEditionService::notify(this->getSptr(), pComposite, compositeMsg);
}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::starting()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::configuring()  throw ( ::fwTools::Failed )
{
        m_compositeKey = m_configuration->findConfigurationElement("acquisitionKeyInComposite")->getExistingAttributeValue("value");
        m_patientDBUID = m_configuration->findConfigurationElement("patientDB")->getExistingAttributeValue("uid");

        OSLM_INFO( "m_compositeKey = " << m_compositeKey);
        OSLM_INFO( "m_patientDBUID = " << m_patientDBUID);
}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

}
