/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_PLANELIST_HPP__
#define __FWDATA_PLANELIST_HPP__


#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Plane.hpp"

#include <vector>

fwCampAutoDeclareDataMacro((fwData)(PlaneList), FWDATA_API);
namespace fwData
{
/**
 * @brief   This class defines a list of planes.
 * @see     ::fwData::Point
 */
class FWDATA_CLASS_API PlaneList : public Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (PlaneList)(::fwData::Object), (()), ::fwData::factory::New< PlaneList >);

    fwCampMakeFriendDataMacro((fwData)(PlaneList));

    typedef std::vector< ::fwData::Plane::sptr > PlaneListContainer;


    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API PlaneList(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~PlaneList();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    // Looking for duplicate plan
    FWDATA_API void deleteDuplicatedPlan(void);

    /** @{
     *  @brief get/set container of all planes
     */
    const PlaneListContainer  getPlanes () const;
    PlaneListContainer & getRefPlanes ();
    const PlaneListContainer & getCRefPlanes () const;
    void setPlanes (const PlaneListContainer& _vPlanes);
    /// @}

protected:

    //! Planes container
    PlaneListContainer m_vPlanes;

}; // end class PlaneList

//-----------------------------------------------------------------------------

inline const PlaneList::PlaneListContainer PlaneList::getPlanes () const
{
    return m_vPlanes;
}

//-----------------------------------------------------------------------------

inline PlaneList::PlaneListContainer & PlaneList::getRefPlanes ()
{
    return this->m_vPlanes;
}

//-----------------------------------------------------------------------------

inline const PlaneList::PlaneListContainer & PlaneList::getCRefPlanes () const
{
    return this->m_vPlanes;
}

//-----------------------------------------------------------------------------

inline void PlaneList::setPlanes (const PlaneList::PlaneListContainer& _vPlanes)
{
    this->m_vPlanes = _vPlanes;
}

//-----------------------------------------------------------------------------

} // end namespace fwData

#endif // __FWDATA_PLANELIST_HPP__

