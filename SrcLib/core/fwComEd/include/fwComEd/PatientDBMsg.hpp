/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_PATIENTDBMSG_HPP_
#define _FWCOMED_PATIENTDBMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief	Object message specialized for PatientDB : store modification information
 * @class 	PatientDBMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009
 * @see 	::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API PatientDBMsg : public ::fwServices::ObjectMsg
{

public:

	fwCoreClassDefinitionsWithFactoryMacro((PatientDBMsg)(::fwServices::ObjectMsg::Baseclass), (()), new PatientDBMsg );

	/**
	 * @name Event identifier
	 */
	/// @{
	/// Event identifier used to inform for modification
	FWCOMED_API static std::string NEW_PATIENT;
	FWCOMED_API static std::string ADD_PATIENT;
	FWCOMED_API static std::string CLEAR_PATIENT;
	FWCOMED_API static std::string NEW_IMAGE_SELECTED;
	FWCOMED_API static std::string NEW_LOADED_PATIENT; /*!< Event when the patient is modified from a file loading (uses a reader). */
	/// @}

	/**
	 * @brief Constructor : does nothing.
	 */
	FWCOMED_API PatientDBMsg() throw();

	/**
	 * @brief Destructor : does nothing.
	 */
	FWCOMED_API virtual ~PatientDBMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_PATIENTDBMSG_HPP_

