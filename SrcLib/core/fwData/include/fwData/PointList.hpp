/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_POINT_LIST_HPP_
#define _FWDATA_POINT_LIST_HPP_

#include <vector>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Point.hpp"


namespace fwData
{
/**
 * @class 	PointList
 * @brief 	This class defines a list of points
 * @see 	::fwData::Point
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
class FWDATA_CLASS_API PointList : public Object
{

public :
	fwCoreClassDefinitionsWithFactoryMacro( (PointList)(::fwData::Object::Baseclass),
        (()), ::fwTools::Factory::New< PointList >) ;

	typedef std::vector< ::fwData::Point::sptr > PointListContainer;

	/// Constructor
	FWDATA_API PointList();

	/// Destructor
	FWDATA_API virtual ~PointList();

	/// Clone method
	FWDATA_API PointList::sptr clone() const;

	/// Copy method
	FWDATA_API PointList &operator=( const PointList & _pointList ) ;

	/// Points container
	fwGettersSettersDocMacro(Points, vPoints, PointListContainer, a container of all points);

protected :

	//! Points container
	PointListContainer m_vPoints;

}; // end class PointList

} // end namespace fwData

#endif // _FWDATA_POINT_LIST_HPP_
