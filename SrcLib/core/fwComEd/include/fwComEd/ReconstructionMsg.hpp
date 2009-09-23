/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_RECONSTRCUTIONMSG_HPP_
#define _FWCOMED_RECONSTRCUTIONMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief	Object event message specialized for reconstruction : store modification information
 * @class 	ReconstructionMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2009
 * @see 	::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API ReconstructionMsg : public ::fwServices::ObjectMsg
{
public:
	fwCoreClassDefinitionsWithFactoryMacro((ReconstructionMsg)(::fwServices::ObjectMsg::Baseclass), ( () ), new ReconstructionMsg );

	/**
	 * @name Event identifier
	 */
	/// @{
	/// Event identifier used to inform for modification
	FWCOMED_API static std::string MATERIAL;
	FWCOMED_API static std::string MESH;
	FWCOMED_API static std::string VISIBILITY;
	/// @}

	/**
	 * @brief 	Constructor : do nothing.
	 */
	FWCOMED_API ReconstructionMsg() throw();

	/**
	 * @brief	Destuctor : do nothing.
	 */
	FWCOMED_API virtual ~ReconstructionMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_RECONSTRCUTIONMSG_HPP_

