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
#include "data/Point.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

#include <vector>

SIGHT_DECLARE_DATA_REFLECTION((sight)(data)(PointList));

namespace sight::data
{
/**
 * @brief   This class defines a list of points.
 * @see     data::Point
 */
class DATA_CLASS_API PointList : public Object
{

public:
    SIGHT_DECLARE_CLASS(PointList, data::Object, data::factory::New< PointList >)

    SIGHT_MAKE_FRIEND_REFLECTION((sight)(data)(PointList));

    typedef std::vector< data::Point::sptr > PointListContainer;

    /**
     * @brief Constructor
     * @param[in] _key Private construction key
     */
    DATA_API PointList(data::Object::Key _key);
    /**
     * @brief Destructor
     */
    DATA_API virtual ~PointList();
    /**
     * @brief Defines shallow copy
     */
    DATA_API void shallowCopy( const Object::csptr& _source ) override;
    /**
     * @brief Defines deep copy
     */
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;
    /**
     * @brief Gets point vector
     * @return the vector of points
     */
    PointListContainer& getPoints ();
    /**
     * @brief Gets point vector
     * @return the vector of points
     */
    const PointListContainer& getPoints () const;
    /**
     * @brief Sets point vector
     * @param[in] _vPoints The vector of points to set
     */
    void setPoints (const PointListContainer& _vPoints);
    /**
     * @brief Adds a data::Point in the pointlist
     * @param[in] _p The point to push
     */
    void pushBack(const data::Point::sptr& _p);
    /**
     * @brief: Deletes a point at the specified index
     * @param[in] _index Index of point to delete
     **/
    void remove(size_t _index);
    /**
     * @brief Clears the list
     */
    void clear();

    /**
     * @name Signals
     * @{
     */
    /**
     * @brief Signal emitted when a Point is added
     */
    typedef core::com::Signal< void (data::Point::sptr) > PointAddedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_ADDED_SIG;
    /**
     * @brief Signal emitted when a Point is removed
     */
    typedef core::com::Signal< void (data::Point::sptr) > PointRemovedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_REMOVED_SIG;
    /**
     * @}
     */

protected:

    /**
     * @brief Points container
     */
    PointListContainer m_vPoints;

}; // end class PointList

//-----------------------------------------------------------------------------

inline PointList::PointListContainer& PointList::getPoints ()
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline const PointList::PointListContainer& PointList::getPoints () const
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline void PointList::setPoints (const PointList::PointListContainer& _vPoints)
{
    this->m_vPoints = _vPoints;
}

//-----------------------------------------------------------------------------

inline void PointList::pushBack(const data::Point::sptr& _p)
{
    this->m_vPoints.push_back(_p);
}

//-----------------------------------------------------------------------------

inline void PointList::remove(size_t _index)
{
    const auto it = m_vPoints.begin() + static_cast<ptrdiff_t>(_index);
    this->m_vPoints.erase(it);
}

//-----------------------------------------------------------------------------

inline void PointList::clear()
{
    this->m_vPoints.clear();
}

} // end namespace sight::data
