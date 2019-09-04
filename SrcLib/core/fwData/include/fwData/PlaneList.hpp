/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
    fwCoreClassDefinitionsWithFactoryMacro( (PlaneList)(::fwData::Object), ::fwData::factory::New< PlaneList >);

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
