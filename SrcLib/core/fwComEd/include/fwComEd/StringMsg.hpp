/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_STRINGMSG_HPP_
#define _FWCOMED_STRINGMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief	Object event message specialized for string field : store modification information
 * @class 	StringMsg
 * @author  IRCAD (Research and Development Team).
 * @author	Jean-Baptiste Fasquel
 * @date    2007-2009.
 * @see 	::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API StringMsg : public ::fwServices::ObjectMsg
{
public:
	fwCoreClassDefinitionsWithFactoryMacro((StringMsg)(::fwServices::ObjectMsg::Baseclass), ( ( ) ), new StringMsg );

	/**
	  * @brief	Constructor : do nothing.
	  */
	FWCOMED_API StringMsg() throw();

	/**
	  * @brief	Destuctor : do nothing
	  */
	FWCOMED_API virtual ~StringMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_STRINGMSG_HPP_

