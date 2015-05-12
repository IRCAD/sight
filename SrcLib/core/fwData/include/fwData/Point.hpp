/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_POINT_HPP__
#define __FWDATA_POINT_HPP__

#include <fwMath/IntrasecTypes.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"

fwCampAutoDeclareDataMacro((fwData)(Point), FWDATA_API);


namespace fwData
{
/**
 * @brief   This class define a 3D point.
 */
class FWDATA_CLASS_API Point : public Object
{

public:

    typedef double PointCoordType;
    typedef fwVec3d PointCoordArrayType;

    fwCoreClassDefinitionsWithNFactoriesMacro( (Point)(::fwData::Object),
                                               ((::fwData::factory::New< Point >,() ))
                                                   ((PointFactory,((float))((float)(0.0f)) ((float) (0.0f)) ))
                                                   ((PointFactory,((double))((double)(0.0)) ((double) (0.0)) ))
                                                   ((PointFactory,((const PointCoordArrayType &)) ))
                                                   ((PointFactory,((Point::sptr)) ))
                                               );

    fwCampMakeFriendDataMacro((fwData)(Point));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Point(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Point();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    /// Coordinates of point
    fwGettersSettersDocMacro(Coord, vCoord, fwVec3d, "point coordinates." );

protected:

    /// Point factory
    FWDATA_API static Point::sptr PointFactory(float x, float y, float z);
    FWDATA_API static Point::sptr PointFactory(double x, double y, double z);
    FWDATA_API static Point::sptr PointFactory(const PointCoordArrayType& coord);
    FWDATA_API static Point::sptr PointFactory(Point::sptr p);

    /// point coordinates
    PointCoordArrayType m_vCoord;

}; // end class Point

} // end namespace fwData

#endif // __FWDATA_POINT_HPP__

