/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#ifndef _FWDATA_VALUED_POINT_LIST_HPP_
#define _FWDATA_VALUED_POINT_LIST_HPP_

#include <vector>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/ValuedPoint.hpp"


namespace fwData
{
/**
 * @class   ValuedPointList
 * @brief   This class defines a list of valued points
 */
class FWDATA_CLASS_API ValuedPointList : public Object
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (ValuedPointList)(::fwData::Object), (()), ::fwTools::Factory::New< ValuedPointList >) ;

    typedef std::vector< ::fwData::ValuedPoint::sptr > PointListContainer;

    /// Constructor
    FWDATA_API ValuedPointList();

    /// Destructor
    FWDATA_API virtual ~ValuedPointList();

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( ValuedPointList::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( ValuedPointList::csptr _source );

    /// Points container
    fwGettersSettersDocMacro(Points, vPoints, PointListContainer, a container of all valued points);

protected :

    //! Points container
    PointListContainer m_vPoints;

}; // end class ValuedPointList

} // end namespace fwData

#endif // _FWDATA_POINT_LIST_HPP_
