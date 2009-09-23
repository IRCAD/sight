/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_POINT_HPP_
#define _FWDATA_POINT_HPP_

#include <boost/array.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"


namespace fwData
{
/**
 * @class 	Point
 * @brief 	This class define a 3D point.
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
class FWDATA_CLASS_API Point : public Object
{

public :
	fwCoreClassDefinitionsWithFactoryMacro( (Point)(::fwData::Object::Baseclass),
        (()), ::fwTools::Factory::New< Point >) ;

	typedef double PointCoordType;

	/// Coordinates array with 3 elements
	typedef ::boost::array<PointCoordType, 3> PointCoordArray;

	/// Constructor
	FWDATA_API Point();
	/// Destructor
	FWDATA_API virtual ~Point();

	/// Clone method
	FWDATA_API Point::sptr clone() const;
	/// Copy method
	FWDATA_API Point &operator=( const Point & _point ) ;

	/// Coordinates of point
	fwGettersSettersDocMacro(Coord, vCoord, PointCoordArray, point coordinates. );


protected :

	/// point coordinates
	PointCoordArray m_vCoord;

}; // end class Point

} // end namespace fwData

#endif // _FWDATA_POINT_HPP_
