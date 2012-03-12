/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/Object.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Composite.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/String.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/IEditionService.hpp>

#include "fwComEd/Dictionary.hpp"
#include "fwComEd/PatientDBMsg.hpp"
#include "fwComEd/fieldHelper/MedicalImageHelpers.hpp"
#include "fwComEd/fieldHelper/BackupHelper.hpp"


namespace fwComEd
{

namespace fieldHelper
{

//-----------------------------------------------------------------------------

::fwData::Image::sptr BackupHelper::getSelectedImage(::fwData::PatientDB::sptr pPatientDB)
{
    ::fwData::Image::sptr pImage;

    if(pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId))
    {
        pImage = getSelectedAcquisition(pPatientDB)->getImage();
    }

    return pImage;
}

//-----------------------------------------------------------------------------

::fwData::Acquisition::sptr BackupHelper::getSelectedAcquisition(::fwData::PatientDB::sptr pPatientDB)
{
    ::fwData::Acquisition::sptr acquisition;

    if(pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId))
    {
        // Get Selection

        ::fwTools::Field::sptr pDataInfo = pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );

        /*::fwData::Object::csptr const vSelection = mySpecificMsg->m_modifiedObject;*/
        ::fwData::Integer::sptr patIdx = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(0) );
        ::fwData::Integer::sptr stuIdx = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(1) );
        ::fwData::Integer::sptr acqIdx = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(2) );

        // Print the selection (debug)
        OSLM_DEBUG( "patIdx->value : " << patIdx->value() );
        OSLM_DEBUG( "stuIdx->value : " << stuIdx->value() );
        OSLM_DEBUG( "acqIdx->value : " << acqIdx->value() );

        ::fwData::Patient::sptr patient = pPatientDB->getPatients().at(patIdx->value());
        ::fwData::Study::sptr study = patient->getStudies().at(stuIdx->value());
        acquisition = study->getAcquisitions().at(acqIdx->value());
    }

    return acquisition;
}

//-----------------------------------------------------------------------------

::fwData::Study::sptr BackupHelper::getSelectedStudy(::fwData::PatientDB::sptr pPatientDB)
{
    ::fwData::Study::sptr study;

    if(pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId))
    {
        // Get Selection

        ::fwTools::Field::sptr pDataInfo = pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );

        /*::fwData::Object::csptr const vSelection = mySpecificMsg->m_modifiedObject;*/
        ::fwData::Integer::sptr patIdx = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(0) );
        ::fwData::Integer::sptr stuIdx = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(1) );

        // Print the selection (debug)
        OSLM_DEBUG( "patIdx->value : " << patIdx->value() );
        OSLM_DEBUG( "stuIdx->value : " << stuIdx->value() );

        ::fwData::Patient::sptr patient = pPatientDB->getPatients().at(patIdx->value());
        study = patient->getStudies().at(stuIdx->value());

    }

    return study;
}

//-----------------------------------------------------------------------------

::fwData::Patient::sptr BackupHelper::getSelectedPatient(::fwData::PatientDB::sptr pPatientDB)
{
    ::fwData::Patient::sptr patient;

    if(pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId))
    {
        // Get Selection

        ::fwTools::Field::sptr pDataInfo = pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );

        /*::fwData::Object::csptr const vSelection = mySpecificMsg->m_modifiedObject;*/
        ::fwData::Integer::sptr patIdx = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(0) );

        // Print the selection (debug)
        OSLM_DEBUG( "patIdx->value : " << patIdx->value() );

        patient = pPatientDB->getPatients().at(patIdx->value());
    }

    return patient;
}

//-----------------------------------------------------------------------------

::fwData::Image::sptr BackupHelper::backupSelectedImage(::fwData::PatientDB::sptr _pPatientDB, ::fwServices::IService::sptr _MsgSource)
{
    ::fwData::Image::sptr backupImage;
    if( _pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId) )
    {
        // Get Selection

        ::fwData::Study::sptr study = getSelectedStudy(_pPatientDB);
        ::fwData::Acquisition::sptr acquisition = getSelectedAcquisition(_pPatientDB);

        ::fwData::Acquisition::NewSptr pAquisitionCopy;
        pAquisitionCopy->deepCopy( acquisition );
        study->addAcquisition( pAquisitionCopy );

        backupImage = pAquisitionCopy->getImage();

        ::fwTools::Field::sptr pDataInfo = _pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );
        ::fwData::Integer::sptr patIdx = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(0) );
        // Fire Event
        ::fwComEd::PatientDBMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::PatientDBMsg::ADD_PATIENT, ::fwData::Integer::New(patIdx->value()) );

        ::fwServices::IEditionService::notify(_MsgSource, _pPatientDB, msg);
    }
    return backupImage;
}

//------------------------------------------------------------------------------


} // fieldHelper

} // fwComEd
