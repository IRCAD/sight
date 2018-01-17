/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"
#include "fwData/Plane.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

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
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /** @{
     *  @brief get/set container of all planes
     */
    PlaneListContainer& getPlanes ();
    const PlaneListContainer& getPlanes () const;
    void setPlanes (const PlaneListContainer& _vPlanes);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when a plane is added
    typedef ::fwCom::Signal< void (::fwData::Plane::sptr) > PlaneAddedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_PLANE_ADDED_SIG;

    /// Signal emitted when a plane is removed
    typedef ::fwCom::Signal< void (::fwData::Plane::sptr) > PlaneRemovedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_PLANE_REMOVED_SIG;

    /// Signal emitted when the visibility changed
    typedef ::fwCom::Signal< void (bool) > VisibilityModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_VISIBILITY_MODIFIED_SIG;
    /**
     * @}
     */

protected:

    //! Planes container
    PlaneListContainer m_vPlanes;

}; // end class PlaneList

//-----------------------------------------------------------------------------

inline PlaneList::PlaneListContainer& PlaneList::getPlanes ()
{
    return this->m_vPlanes;
}

//-----------------------------------------------------------------------------

inline const PlaneList::PlaneListContainer& PlaneList::getPlanes () const
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
