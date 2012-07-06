/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_FIELDHELPER_BACKUPHELPER_HPP_
#define _FWCOMED_FIELDHELPER_BACKUPHELPER_HPP_

#include <fwServices/IService.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Image.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

namespace fieldHelper
{

/**
 * @class   BackupHelper
 * @brief   This class contains helpers for backup and selection for patientDB's fields.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class FWCOMED_CLASS_API BackupHelper
{
public :

    typedef int SelectionIdType;

    /**
     * @brief   Set the selected patient, study and acquisition index in the patientDB.
     */
    FWCOMED_API static void setSelection(::fwData::PatientDB::sptr pPatientDB,
            SelectionIdType patientId, SelectionIdType studyId, SelectionIdType acquisitionId);

    /**
     * @brief   Returns the selected patient index in the patientDB.
     * @note    If pPatientDB haven't selected patient, this method return -1.
     */
    FWCOMED_API static SelectionIdType getSelectedPatientIdx(::fwData::PatientDB::sptr patientDB);

    /**
     * @brief   Returns the selected study index in the patientDB.
     * @note    If pPatientDB haven't selected study, this method return -1.
     */
    FWCOMED_API static SelectionIdType getSelectedStudyIdx(::fwData::PatientDB::sptr patientDB);

    /**
     * @brief   Returns the selected acquisition index in the patientDB.
     * @note    If pPatientDB haven't selected acquisition, this method return -1.
     */
    FWCOMED_API static SelectionIdType getSelectedAcquisitionIdx(::fwData::PatientDB::sptr patientDB);

    /**
     * @brief       Backup the selected image.
     *
     * Create a new image same of the selected image. And notify observers with ADD_PATIENT event.
     *
     * @param[in]   _pPatientDB patientDB root containing the selected image
     * @param[in]   _MsgSource  source service for the notification.
     * @return      the backup of the image if success, else a null pointer.
     *
     * @note        If pPatientDB haven't selected image, this method do nothing and return a null pointer.
     */
    FWCOMED_API static ::fwData::Image::sptr backupSelectedImage( ::fwData::PatientDB::sptr _pPatientDB, ::fwServices::IService::sptr _MsgSource);

    /**
     * @brief   Returns the selected image in the patientDB.
     * @note    If pPatientDB haven't selected image, this method return a null pointer.
     */
    FWCOMED_API static ::fwData::Image::sptr getSelectedImage(::fwData::PatientDB::sptr _pPatientDB);

    /**
     * @brief   Returns the selected acquisition in the patientDB.
     * @note    If pPatientDB haven't selected acquisition, this method return a null pointer.
     */
    FWCOMED_API static ::fwData::Acquisition::sptr getSelectedAcquisition(::fwData::PatientDB::sptr _pPatientDB);

    /**
     * @brief   Returns the selected study in the patientDB.
     * @note    If pPatientDB haven't selected study, this method return a null pointer.
     */
    FWCOMED_API static ::fwData::Study::sptr getSelectedStudy(::fwData::PatientDB::sptr _pPatientDB);

    /**
     * @brief   Returns the selected patient in the patientDB.
     * @note    If pPatientDB haven't selected patient, this method return a null pointer.
     */
    FWCOMED_API static ::fwData::Patient::sptr getSelectedPatient(::fwData::PatientDB::sptr _pPatientDB);

protected:

    /**
     * @brief   Returns the selected data index in the patientDB specified by the name field.
     * @note    If pPatientDB haven't selected data, this method return -1.
     */
    FWCOMED_API static SelectionIdType getSelectedIdx(::fwData::Object::sptr data, ::fwData::Object::FieldNameType name);
};

} // fieldHelper
} // fwComEd

#endif /* _FWCOMED_FIELDHELPER_BACKUPHELPER_HPP_ */
