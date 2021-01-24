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
#include "data/Point.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

#include <vector>

fwCampAutoDeclareDataMacro((sight)(data)(Plane));
namespace sight::data
{
/**
 * @brief   This class defines a plane defined by tree points.
 * @see     data::Point
 */
class DATA_CLASS_API Plane : public Object
{

public:
    fwCoreClassMacro(Plane, data::Object, data::factory::New< Plane >)

    fwCampMakeFriendDataMacro((sight)(data)(Plane));

    typedef std::array< data::Point::sptr, 3> PointContainer;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Plane(data::Object::Key key);

    /// Destructor
    DATA_API virtual ~Plane();

    /// Defines shallow copy
    DATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// Re-initialize the plane with 3 points
    DATA_API void setValue(data::Point::sptr _point1, data::Point::sptr _point2,
                           data::Point::sptr _point3);

    /** @{
     *  @brief get/set points container
     */
    PointContainer& getPoints();
    const PointContainer& getPoints() const;
    void setPoints(const PointContainer& _vPoints);
    /// @}

    /** @{
     *  @brief get/set flag if the plane is an intersection one (else an union one)
     */
    bool getIsIntersection () const;
    void setIsIntersection(bool _isIntersection);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when plane is selected/deselected
    typedef core::com::Signal< void (bool) > SelectedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_SELECTED_SIG;
    /**
     * @}
     */

protected:

    //! Points container
    PointContainer m_vPoints;

    //! flag if the plane is an intersection (else an union)
    bool m_isIntersection;

}; // end class Plane

//-----------------------------------------------------------------------------

inline Plane::PointContainer& Plane::getPoints()
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline const Plane::PointContainer& Plane::getPoints() const
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline void Plane::setPoints(const Plane::PointContainer& _vPoints)
{
    this->m_vPoints = _vPoints;
}

//-----------------------------------------------------------------------------

inline bool Plane::getIsIntersection () const
{
    return m_isIntersection;
}

//-----------------------------------------------------------------------------

inline void Plane::setIsIntersection(bool _isIntersection)
{
    this->m_isIntersection = _isIntersection;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
