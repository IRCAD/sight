/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/Object.hpp"
#include "data/Plane.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

#include <vector>

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (PlaneList));

namespace sight::data
{

/**
 * @brief   This class defines a list of planes.
 * @see     data::Point
 */
class DATA_CLASS_API PlaneList : public Object
{
public:

    SIGHT_DECLARE_CLASS(PlaneList, data::Object, data::factory::New<PlaneList>);

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (PlaneList));

    typedef std::vector<data::Plane::sptr> PlaneListContainer;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API PlaneList(data::Object::Key key);

    /// Destructor
    DATA_API virtual ~PlaneList();

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /** @{
     *  @brief get/set container of all planes
     */
    PlaneListContainer& getPlanes();
    const PlaneListContainer& getPlanes() const;
    void setPlanes(const PlaneListContainer& _vPlanes);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when a plane is added
    typedef core::com::Signal<void (data::Plane::sptr)> PlaneAddedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_PLANE_ADDED_SIG;

    /// Signal emitted when a plane is removed
    typedef core::com::Signal<void (data::Plane::sptr)> PlaneRemovedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_PLANE_REMOVED_SIG;

    /// Signal emitted when the visibility changed
    typedef core::com::Signal<void (bool)> VisibilityModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_VISIBILITY_MODIFIED_SIG;
/**
 * @}
 */

protected:

    //! Planes container
    PlaneListContainer m_vPlanes;
}; // end class PlaneList

//-----------------------------------------------------------------------------

inline PlaneList::PlaneListContainer& PlaneList::getPlanes()
{
    return this->m_vPlanes;
}

//-----------------------------------------------------------------------------

inline const PlaneList::PlaneListContainer& PlaneList::getPlanes() const
{
    return this->m_vPlanes;
}

//-----------------------------------------------------------------------------

inline void PlaneList::setPlanes(const PlaneList::PlaneListContainer& _vPlanes)
{
    this->m_vPlanes = _vPlanes;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
