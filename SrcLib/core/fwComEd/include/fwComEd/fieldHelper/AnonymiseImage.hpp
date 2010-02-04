/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_FIELDHELPER_ANONYMISEIMAGE_HPP_
#define _FWCOMED_FIELDHELPER_ANONYMISEIMAGE_HPP_

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Image.hpp>

#include "fwComEd/export.hpp"

namespace fwComEd
{

namespace fieldHelper
{

/**
 * @brief 	This class contains helpers to anonymise patients, studies, images,...
 * @class 	AnonymiseImage
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 */
class FWCOMED_CLASS_API AnonymiseImage
{
public:

	/**
	 * @brief Anonymise the patient.
	 *
	 * Anonyme the patient (set patient's name and fistname to "anonymous",set default birthdate to
	 * 1400-01-01) and anonymise all his studies.
	 *
	 * @param[in] 	_pPatient	patient to anonymise
	 */
	FWCOMED_API static void anonymisePatient( ::fwData::Patient::sptr _pPatient);

	/**
	 * @brief	Anonymise each patient in the patientDB.
	 */
	FWCOMED_API static void anonymisePatientDB( ::fwData::PatientDB::sptr _pPatientDB);

	/**
	 * @brief 	Anonymise the image (do nothing).
	 */
	FWCOMED_API static void anonymiseImage( ::fwData::Image::sptr _pImage);

	/**
	 * @brief 	Anonymise the study.
	 *
	 * Anonnymise the study (set hospital's name to "anonymous", remove indentifiers)
	 * and anonymise all his acquisitions.
	 */
	FWCOMED_API static void anonymiseStudy( ::fwData::Study::sptr _pStudy);

	/**
	 * @brief	Anonymise the acquisition.
	 *
	 * Anonymise the acquisition (set the creation date to 1400-01-01 and remove indentifiers).
	 */
	FWCOMED_API static void anonymiseAcquisition( ::fwData::Acquisition::sptr _pAcquisition);

	/**
	 * @brief	Create a new patient anonymised.
	 *
	 * The patient's elements (parameters and fields) are cloned from the original patient and anonymised .
	 *
	 * @note 	Image are not cloned but are on the same pointer for the two patients.
	 */
	FWCOMED_API static ::fwData::Patient::sptr createAnonymisedPatient( ::fwData::Patient::sptr _pPatient);

	/**
	 * @brief	Create a new patientDB anonymised.
	 *
	 * Each patient in the patientDB is cloned and anonymise.
	 *
	 * @note 	Image are not cloned.
	 * @see 	createAnonymisedPatient
	 */
	FWCOMED_API static ::fwData::PatientDB::sptr createAnonymisedPatientDB( ::fwData::PatientDB::sptr _pPatientDB);

};


} // fieldHelper

} // fwComEd

#endif /* _FWCOMED_FIELDHELPER_ANONYMISEIMAGE_HPP_ */
