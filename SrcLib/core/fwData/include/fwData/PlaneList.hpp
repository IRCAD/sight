/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_PLANELIST_HPP_
#define _FWDATA_PLANELIST_HPP_

#include <vector>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Plane.hpp"

fwCampAutoDeclareDataMacro((fwData)(PlaneList), FWDATA_API);
namespace fwData
{
/**
 * @class   PlaneList
 * @brief   This class defines a list of planes
 * @see     ::fwData::Point
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API PlaneList : public Object
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (PlaneList)(::fwData::Object),
        (()), ::fwData::factory::New< PlaneList >) ;

    fwCampMakeFriendDataMacro((fwData)(PlaneList));

    typedef std::vector< ::fwData::Plane::sptr > PlaneListContainer;

    fwDataObjectMacro();

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API PlaneList(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~PlaneList();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( PlaneList::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( PlaneList::csptr _source );

    // Looking for duplicate plan
    FWDATA_API void deleteDuplicatedPlan(void);

    /// Planes container
    fwGettersSettersDocMacro(Planes, vPlanes, PlaneListContainer, a container of all planes);

protected :

    //! Planes container
    PlaneListContainer m_vPlanes;

}; // end class PlaneList

} // end namespace fwData

#endif // _FWDATA_PLANELIST_HPP_
