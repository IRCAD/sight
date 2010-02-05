/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_TRANSFERTFUNCTIONPOINT_HPP_
#define _FWDATA_TRANSFERTFUNCTIONPOINT_HPP_

#include <vector>
#include <boost/cstdint.hpp>

#include "fwData/config.hpp"

#include "fwData/Object.hpp"
#include "fwData/Color.hpp"

namespace fwData
{
/**
 * @class 	TransfertFunctionPoint
 * @brief 	This class defines a point of a transfert function.
 *
 * A point is represented by a color dans a value.
 * It is use in ::fwData::TransfertFunction.
 *
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
class FWDATA_CLASS_API TransfertFunctionPoint : public Object
{

public :
	fwCoreClassDefinitionsWithFactoryMacro( (TransfertFunctionPoint)(::fwData::Object::Baseclass),
        (()), ::fwTools::Factory::New< TransfertFunctionPoint >) ;

	typedef ::boost::int32_t TFValueType;

	/// Constructor
	FWDATA_API TransfertFunctionPoint();

	/// Destructor
	FWDATA_API virtual ~TransfertFunctionPoint();

	/// Clone
	FWDATA_API TransfertFunctionPoint::sptr clone() const;

	/// indentier for field color
	static const Object::FieldID ID_COLOR;

	/**
	 * @brief Set color
	 */
	FWDATA_API void setColor( ::fwData::Color::sptr _pColor );

	/**
	 * @{
	 * @brief Get the color of the point
	 */
	FWDATA_API ::fwData::Color::csptr getColor() const;
	FWDATA_API ::fwData::Color::sptr getColor();
	//@}

	fwGettersSettersDocMacro(Value, i32Value, TFValueType, the value)


protected :

	//! point value
	TFValueType  m_i32Value;

}; // end class TransfertFunctionPoint

} // end namespace fwData

#endif // _FWDATA_TRANSFERTFUNCTIONPOINT_HPP_
