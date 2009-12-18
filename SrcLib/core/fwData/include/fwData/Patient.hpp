/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef FWDATA_PATIENT_H
#define FWDATA_PATIENT_H

#include <vector>
#include <iostream>
#include <ostream>
#include <map>

#include <boost/cstdint.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Study.hpp"
#include "fwData/Composite.hpp"
#include "fwData/DownCastIterator.hpp"

namespace fwData
{

/** \class Patient
 *********************
 * Namespace : data
 *********************
 * @brief This class defines a medical firmware
 * object. Patient represents fundamental
 * object in the patient folder. This class
 * defines patient's name, sex, Birthdate ..
 * This class contains patient's studies.
 *
 * @see ::fwData::Study
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
class FWDATA_CLASS_API Patient : public Object
{

public:
	fwCoreClassDefinitionsWithFactoryMacro( (Patient)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< Patient >) ;

	/// Constructor
	FWDATA_API Patient ();
	/// Destructor
	FWDATA_API virtual ~Patient ();

	/// Copy
	FWDATA_API Patient &operator=( const Patient & _patient ) ;

	// Studies -----------------------------------------------------------------
	/// Field identifier for studies
	FWDATA_API static const Object::FieldID ID_STUDIES;

	/// Iterator for study
	typedef ContainerCaster< Study >::iterator		 StudyIterator;
	/// const iterator for study
	typedef ContainerCaster< Study >::const_iterator StudyConstIterator;

	/**
	 * @brief Get the number of studies
	 * @return number of studies
	 */
	FWDATA_API boost::uint32_t  getStudySize() const;

	/**
	 * @brief add study
	 * @param[in] _study ::fwData::Study::sptr
	 */
	FWDATA_API void addStudy( ::fwData::Study::sptr _study );

	/**@{
	 * Get iterator on the first and the last study.
	 * @return std::pair( study.begin(), study.end() )
	 */
	FWDATA_API std::pair< StudyIterator, StudyIterator > getStudies();
	FWDATA_API std::pair< StudyConstIterator, StudyConstIterator > getStudies() const;
	//@}

	// toolBox -----------------------------------------------------------------
	/// Field identifier for toolBox
	FWDATA_API static const Object::FieldID ID_TOOLBOX;

	/**
	 * @brief set toolBox
	 * @param[in] _toolBox ::fwData::Composite::sptr
	 */
	FWDATA_API void setToolBox( ::fwData::Composite::sptr _toolBox );

	/**@{
	 * Get the toolBox.
	 */
	FWDATA_API ::fwData::Composite::sptr getToolBox();
	FWDATA_API ::fwData::Composite::csptr getToolBox() const;
	//@}


	// scenarios -----------------------------------------------------------------
	/// Field identifier for scenarios
	FWDATA_API static const Object::FieldID ID_SCENARIOS;

	/**
	 * @brief set scenarios
	 * @param[in] _scenarios ::fwData::Composite::sptr
	 */
	FWDATA_API void setScenarios( ::fwData::Composite::sptr _scenarios );

	/**@{
	 * Get the scenarios.
	 */
	FWDATA_API ::fwData::Composite::sptr getScenarios();
	FWDATA_API ::fwData::Composite::csptr getScenarios() const;
	//@}

	/**
	 * @brief add tool in the toolBox
	 * @param[in] _name tool identifier
	 * @param[in] _tool ::fwData::Object::sptr
	 */
	FWDATA_API void addTool( std::string _name, ::fwData::Object::sptr _tool );

	/**
	 * @brief get tool from the toolBox
	 * @param[in] _name tool identifier
	 * @return the tool
	 */
	FWDATA_API ::fwData::Object::sptr getTool( std::string _name );

	// Generator result---------------------------------------------------------

	fwGettersSettersDocMacro(Name, sName, std::string, name of the patient);

	fwGettersSettersDocMacro(Firstname, sFirstname, std::string, fisrtname of the patient );

	fwGettersSettersDocMacro(IDDicom, sIDDicom, std::string, DICOM identifiant (eg : 01254312));

	fwGettersSettersDocMacro(Birthdate, sBirthdate, ::boost::posix_time::ptime, birthdate of the patient (eg : 19771010) );

	fwGettersSettersDocMacro(IsMale, bIsMale, bool, if the patient is male (true if is male) );

	fwGettersSettersDocMacro(DbID, i32DbID, ::boost::int32_t, database identifier );

protected:

	//! Patient's name
	std::string m_sName;

	//! Patient's first name
	std::string m_sFirstname;

	//! DICOM identifiant (eg : 01254312)
	std::string m_sIDDicom;

	//! Birthdate (eg : 19771010)
	boost::posix_time::ptime m_sBirthdate;

	//! true if the patient is male, false if not
	bool m_bIsMale;

	/*
	 **************
	 * USED IN FWDATABASE
	 **************
	 */

	//! Database indentifier
	boost::int32_t  m_i32DbID;

	//! Vector of study (which contains acquistion and slice ...)
	//std::vector< ::fwData::Study::sptr > m_vStudies;

};

}//end namespace fwData

#endif // FWDATA_PATIENT_H


