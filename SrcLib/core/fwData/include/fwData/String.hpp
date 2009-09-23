/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_STRING_HPP_
#define _FWDATA_STRING_HPP_


#include "fwData/Object.hpp"

namespace fwData
{
/**
 * @class 	String
 * @brief 	This class contains an std::string value
 *
 * String object is essentially used as a field in other objects.
 *
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
class FWDATA_CLASS_API String : public Object
{
public:
	fwCoreClassDefinitionsWithFactoryMacro( (String)(::fwData::Object::Baseclass), ( ((std::string)("")) ), new String) ;

	/**
	 * @brief Constructor.
	 * @param[in] value The initial value.
	 */
	FWDATA_API String(std::string _value="" );

	/**
	 * @brief Destructor.
	 */
	FWDATA_API virtual ~String();

	/// Value accessor
	FWDATA_API std::string & value();
	/// Value accessor
	FWDATA_API const std::string & value() const;

protected :

	std::string m_value;
};

}


#endif /*_FWDATA_STRING_HPP_*/
