/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_DEBUGOBJECT_H_
#define _FWDATA_DEBUGOBJECT_H_

#include <iostream>
#include "fwData/Object.hpp"

namespace fwData
{
/**
 * @class 	DebugObject
 * @brief 	This class is used for debugging
 *
 * This contains a static method to print informations about the object and its fields.
 *
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
class FWDATA_CLASS_API DebugObject : public ::fwCore::BaseObject
{
public:
	fwCoreClassDefinitionsWithFactoryMacro( (DebugObject), (()), new DebugObject) ;

	/// Construcotr
	FWDATA_API DebugObject();

	/// Destructor
	FWDATA_API virtual ~DebugObject();

	/**
	 * @brief Print informations about the object
	 *
	 * Print object classname, number of field, list of all fields
	 *
	 * @param[out] _os output stream
	 * @param[in] _object object to be debug
	 * @param[in] _level number of blank before text
	 */
	FWDATA_API static void print( std::ostream &_os, fwTools::Object *_object, int _level = 0 );
};

} // namespace fwData

#endif // _FWDATA_DEBUGOBJECT_H_
