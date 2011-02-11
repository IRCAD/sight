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

#include <fwServices/helper.hpp>
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
    ::fwData::Acquisition::sptr pAcq;

    if(pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId))
    {
        // Get Selection

        ::fwTools::Field::sptr pDataInfo = pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );

        /*::fwData::Object::csptr const vSelection = mySpecificMsg->m_modifiedObject;*/
        ::fwData::Integer::sptr myIntPat = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(0) );
        ::fwData::Integer::sptr myIntStu = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(1) );
        ::fwData::Integer::sptr myIntAcq = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(2) );

        // Print the selection (debug)
        OSLM_DEBUG( "myIntPat->value : " << myIntPat->value() );
        OSLM_DEBUG( "myIntStu->value : " << myIntStu->value() );
        OSLM_DEBUG( "myIntAcq->value : " << myIntAcq->value() );

        // Patient selection
        ::fwData::PatientDB::PatientIterator patientIter = pPatientDB->getPatients().first;
        patientIter += myIntPat->value();

        // Study selection
        ::fwData::Patient::StudyIterator studyIter = (*patientIter)->getStudies().first;
        studyIter += myIntStu->value();

        // Acquisition selection
        ::fwData::Study::AcquisitionIterator acquisitionIter = (*studyIter)->getAcquisitions().first;
        acquisitionIter += myIntAcq->value();

        pAcq = (*acquisitionIter);
    }

    return pAcq;
}

//-----------------------------------------------------------------------------

::fwData::Study::sptr BackupHelper::getSelectedStudy(::fwData::PatientDB::sptr pPatientDB)
{
    ::fwData::Study::sptr pStudy;

    if(pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId))
    {
        // Get Selection

        ::fwTools::Field::sptr pDataInfo = pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );

        /*::fwData::Object::csptr const vSelection = mySpecificMsg->m_modifiedObject;*/
        ::fwData::Integer::sptr myIntPat = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(0) );
        ::fwData::Integer::sptr myIntStu = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(1) );
        ::fwData::Integer::sptr myIntAcq = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(2) );

        // Print the selection (debug)
        OSLM_DEBUG( "myIntPat->value : " << myIntPat->value() );
        OSLM_DEBUG( "myIntStu->value : " << myIntStu->value() );
        OSLM_DEBUG( "myIntAcq->value : " << myIntAcq->value() );

        // Patient selection
        ::fwData::PatientDB::PatientIterator patientIter = pPatientDB->getPatients().first;
        patientIter += myIntPat->value();

        // Study selection
        ::fwData::Patient::StudyIterator studyIter = (*patientIter)->getStudies().first;
        studyIter += myIntStu->value();

        pStudy = (*studyIter);
    }

    return pStudy;
}

//-----------------------------------------------------------------------------

::fwData::Patient::sptr BackupHelper::getSelectedPatient(::fwData::PatientDB::sptr pPatientDB)
{
    ::fwData::Patient::sptr pPatient;

    if(pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId))
    {
        // Get Selection

        ::fwTools::Field::sptr pDataInfo = pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );

        /*::fwData::Object::csptr const vSelection = mySpecificMsg->m_modifiedObject;*/
        ::fwData::Integer::sptr myIntPat = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(0) );

        // Print the selection (debug)
        OSLM_DEBUG( "myIntPat->value : " << myIntPat->value() );

        // Patient selection
        ::fwData::PatientDB::PatientIterator patientIter = pPatientDB->getPatients().first;
        patientIter += myIntPat->value();

        pPatient = (*patientIter);
    }

    return pPatient;
}

//-----------------------------------------------------------------------------

::fwData::Image::sptr BackupHelper::backupSelectedImage(::fwData::PatientDB::sptr _pPatientDB, ::fwServices::IService::sptr _MsgSource)
{
    ::fwData::Image::sptr backupImage;
    if( _pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId) )
    {
        // Get Selection

        ::fwTools::Field::sptr pDataInfo = _pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );

        /*::fwData::Object::csptr const vSelection = mySpecificMsg->m_modifiedObject;*/
        ::fwData::Integer::sptr myIntPat = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(0) );
        ::fwData::Integer::sptr myIntStu = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(1) );
        ::fwData::Integer::sptr myIntAcq = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(2) );

        // Print the selection (debug)
        OSLM_DEBUG( "myIntPat->value : " << myIntPat->value() );
        OSLM_DEBUG( "myIntStu->value : " << myIntStu->value() );
        OSLM_DEBUG( "myIntAcq->value : " << myIntAcq->value() );

        // Patient selection
        ::fwData::PatientDB::PatientIterator patientIter = _pPatientDB->getPatients().first;
        patientIter += myIntPat->value();

        // Study selection
        ::fwData::Patient::StudyIterator studyIter = (*patientIter)->getStudies().first;
        studyIter += myIntStu->value();

        // Acquisition selection
        ::fwData::Study::AcquisitionIterator acquisitionIter = (*studyIter)->getAcquisitions().first;
        acquisitionIter += myIntAcq->value();

        //::fwData::Acquisition::sptr pAquisitionBackup = (*acquisitionIter)->clone();
        ::fwData::Acquisition::NewSptr pAquisitionBackup;
        pAquisitionBackup->deepCopy( *acquisitionIter );
        (*studyIter)->addAcquisition( pAquisitionBackup );
        backupImage = pAquisitionBackup->getImage();

        // Fire Event
        ::fwComEd::PatientDBMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::PatientDBMsg::ADD_PATIENT );

        ::fwServices::IEditionService::notify(_MsgSource, _pPatientDB, msg);
    }
    return backupImage;
}

//------------------------------------------------------------------------------


} // fieldHelper

} // fwComEd
