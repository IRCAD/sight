/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#ifndef _FWDATA_VALUEDPOINT_HPP_
#define _FWDATA_VALUEDPOINT_HPP_

#include <boost/array.hpp>

#include "fwData/config.hpp"
#include "fwData/Point.hpp"
#include "fwMath/IntrasecTypes.hpp"


namespace fwData
{
/**
 * @class   ValuedPoint
 * @brief   This class define a valued 3D point.
 */
class FWDATA_CLASS_API ValuedPoint : public Point
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (ValuedPoint)(::fwData::Object), (()), ::fwTools::Factory::New< ValuedPoint >) ;

    typedef double PointValueType;

    /// Constructor
    FWDATA_API ValuedPoint();
    /// Destructor
    FWDATA_API virtual ~ValuedPoint();

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( ValuedPoint::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( ValuedPoint::csptr _source );

    /// Coordinates of point
    fwGettersSettersDocMacro(Value, value, PointValueType, point value. );

protected :

    /// point value
    PointValueType m_value;

}; // end class ValuedPoint

} // end namespace fwData

#endif // _FWDATA_VALUEDPOINT_HPP_
