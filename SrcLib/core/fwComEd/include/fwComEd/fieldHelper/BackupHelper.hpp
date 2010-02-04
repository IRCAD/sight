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

#include "fwComEd/export.hpp"


namespace fwComEd
{

namespace fieldHelper
{

/**
 * @class 	BackupHelper
 * @brief 	This class contains helpers for backup and selection for patientDB's fields.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 */
class FWCOMED_CLASS_API BackupHelper
{
	public :

		/**
		 * @brief		Backup the selected image.
		 *
		 * Create a new image same of the selected image. And notify observers with ADD_PATIENT event.
		 *
		 * @param[in]	_pPatientDB	patientDB root containing the selected image
		 * @param[in]	_MsgSource	source service for the notification.
		 * @return 		true if backup successed.
		 *
		 * @note		If pPatientDB haven't fwComEd::Dictionary::m_imageSelectedId field, this method do nothing and return false.
		 */
		FWCOMED_API static bool backupSelectedImage( ::fwData::PatientDB::sptr _pPatientDB, ::fwServices::IService::sptr _MsgSource);

		/**
		 * @brief	Return the selected image in the patientDB.
		 * @note	If pPatientDB haven't fwComEd::Dictionary::m_imageSelectedId field, this method return a null pointer.
		 */
		FWCOMED_API static ::fwData::Image::sptr getSelectedImage(::fwData::PatientDB::sptr _pPatientDB);

		/**
		 * @brief	Return the selected acquisition in the patientDB.
		 * @note	If pPatientDB haven't fwComEd::Dictionary::m_imageSelectedId field, this method return a null pointer.
		 */
		FWCOMED_API static ::fwData::Acquisition::sptr getSelectedAcquisition(::fwData::PatientDB::sptr _pPatientDB);

		/**
		 * @brief	Return the selected patient in the patientDB.
		 * @note	If pPatientDB haven't fwComEd::Dictionary::m_imageSelectedId field, this method return a null pointer.
		 */
		FWCOMED_API static ::fwData::Patient::sptr getSelectedPatient(::fwData::PatientDB::sptr _pPatientDB);
};

} // fieldHelper
} // fwComEd

#endif /* _FWCOMED_FIELDHELPER_BACKUPHELPER_HPP_ */
