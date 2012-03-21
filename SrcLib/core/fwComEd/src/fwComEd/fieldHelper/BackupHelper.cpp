/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/Object.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Composite.hpp>
#include <fwData/TransferFunction.hpp>
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

void BackupHelper::setSelection(::fwData::PatientDB::sptr patientDB,
        SelectionIdType patientId, SelectionIdType studyId, SelectionIdType acquisitionId)
{
    SLM_ASSERT("PatientDB null pointer", patientDB);
    patientDB->setField("SELECTED_PATIENT", ::fwData::Integer::New(patientId));
    patientDB->setField("SELECTED_STUDY", ::fwData::Integer::New(studyId));
    patientDB->setField("SELECTED_ACQUISITION", ::fwData::Integer::New(acquisitionId));
}

//-----------------------------------------------------------------------------

BackupHelper::SelectionIdType BackupHelper::getSelectedIdx(::fwData::Object::sptr data, ::fwData::Object::FieldNameType name)
{
    SelectionIdType index = -1;
    ::fwData::Integer::sptr dataIdx = ::fwData::Integer::dynamicCast(data->getField(name));
    if(dataIdx)
    {
        index = dataIdx->value();
    }
    return index;
}

//-----------------------------------------------------------------------------

BackupHelper::SelectionIdType BackupHelper::getSelectedPatientIdx(::fwData::PatientDB::sptr patientDB)
{
    return BackupHelper::getSelectedIdx(patientDB, "SELECTED_PATIENT");
}

//-----------------------------------------------------------------------------

BackupHelper::SelectionIdType BackupHelper::getSelectedStudyIdx(::fwData::PatientDB::sptr patientDB)
{
    return BackupHelper::getSelectedIdx(patientDB, "SELECTED_STUDY");
}

//-----------------------------------------------------------------------------

BackupHelper::SelectionIdType BackupHelper::getSelectedAcquisitionIdx(::fwData::PatientDB::sptr patientDB)
{
    return BackupHelper::getSelectedIdx(patientDB, "SELECTED_ACQUISITION");
}

//-----------------------------------------------------------------------------

::fwData::Patient::sptr BackupHelper::getSelectedPatient(::fwData::PatientDB::sptr pPatientDB)
{
    ::fwData::Patient::sptr patient;
    SelectionIdType idx = BackupHelper::getSelectedPatientIdx(pPatientDB);
    if(idx >= 0)
    {
        patient = pPatientDB->getPatients().at(idx);
    }
    return patient;
}

//-----------------------------------------------------------------------------

::fwData::Study::sptr BackupHelper::getSelectedStudy(::fwData::PatientDB::sptr pPatientDB)
{
    ::fwData::Patient::sptr patient = BackupHelper::getSelectedPatient(pPatientDB);
    ::fwData::Study::sptr study;

    SelectionIdType idx = BackupHelper::getSelectedStudyIdx(pPatientDB);
    if(patient && idx >= 0)
    {
        study = patient->getStudies().at(idx);
    }
    return study;
}

//-----------------------------------------------------------------------------

::fwData::Acquisition::sptr BackupHelper::getSelectedAcquisition(::fwData::PatientDB::sptr pPatientDB)
{
    ::fwData::Patient::sptr patient = BackupHelper::getSelectedPatient(pPatientDB);
    ::fwData::Study::sptr study = BackupHelper::getSelectedStudy(pPatientDB);
    ::fwData::Acquisition::sptr acquisition;

    SelectionIdType idx = BackupHelper::getSelectedAcquisitionIdx(pPatientDB);
    if(patient && study && idx >= 0)
    {
        acquisition = study->getAcquisitions().at(idx);
    }
    return acquisition;
}

//-----------------------------------------------------------------------------

::fwData::Image::sptr BackupHelper::getSelectedImage(::fwData::PatientDB::sptr pPatientDB)
{
    ::fwData::Image::sptr pImage;
    ::fwData::Acquisition::sptr acquisition = BackupHelper::getSelectedAcquisition(pPatientDB);

    if(acquisition)
    {
        pImage = acquisition->getImage();
    }

    return pImage;
}

//-----------------------------------------------------------------------------

::fwData::Image::sptr BackupHelper::backupSelectedImage(::fwData::PatientDB::sptr _pPatientDB, ::fwServices::IService::sptr _MsgSource)
{
    ::fwData::Image::sptr backupImage;
    SelectionIdType patIdx = BackupHelper::getSelectedPatientIdx(_pPatientDB);
    ::fwData::Study::sptr study = BackupHelper::getSelectedStudy(_pPatientDB);
    ::fwData::Acquisition::sptr acquisition = BackupHelper::getSelectedAcquisition(_pPatientDB);

    if( patIdx >= 0 && study && acquisition )
    {
        ::fwData::Acquisition::NewSptr pAquisitionCopy;
        pAquisitionCopy->deepCopy( acquisition );
        study->addAcquisition( pAquisitionCopy );

        backupImage = pAquisitionCopy->getImage();

        // Fire Event
        ::fwComEd::PatientDBMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::PatientDBMsg::ADD_PATIENT, ::fwData::Integer::New(patIdx) );
        ::fwServices::IEditionService::notify(_MsgSource, _pPatientDB, msg);
    }
    return backupImage;
}

//------------------------------------------------------------------------------


} // fieldHelper

} // fwComEd
