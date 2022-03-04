/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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
#include "data/PointList.hpp"

#include <vector>

namespace sight::data
{

/**
 * @brief This class represent a tag. A tag contains a list of points.
 * @see PointList
 */
class DATA_CLASS_API Tag : public Object
{
public:

    SIGHT_DECLARE_CLASS(Tag, Object, factory::New<Tag>);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Tag(Object::Key key);

    /// Destructor
    DATA_API virtual ~Tag();

    /**
     * @{
     * @brief Get/Set value of the pointList.
     */
    PointList::sptr& getPointList();
    const PointList::sptr& getPointList() const;
    void setPointList(const PointList::sptr& _pointList);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the Type.
     */
    std::string& getType();
    const std::string& getType() const;
    void setType(const std::string& _sType);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the Tag size.
     */
    double& getSize();
    double getSize() const;
    void setSize(const double _size);
    /// @}

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Tag& other) const noexcept;
    DATA_API bool operator!=(const Tag& other) const noexcept;
    /// @}

protected:

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache) override;

    /// list of points
    PointList::sptr m_pointList;

    /// type of tag
    std::string m_sType;

    /// size of tag
    double m_size;
}; // end class Tag

//-----------------------------------------------------------------------------

inline PointList::sptr& Tag::getPointList()
{
    return this->m_pointList;
}

//-----------------------------------------------------------------------------

inline const PointList::sptr& Tag::getPointList() const
{
    return this->m_pointList;
}

//-----------------------------------------------------------------------------

inline void Tag::setPointList(const PointList::sptr& _pointList)
{
    this->m_pointList = _pointList;
}

//-----------------------------------------------------------------------------

inline std::string& Tag::getType()
{
    return m_sType;
}

//-----------------------------------------------------------------------------

inline const std::string& Tag::getType() const
{
    return m_sType;
}

//-----------------------------------------------------------------------------

inline void Tag::setType(const std::string& _sType)
{
    this->m_sType = _sType;
}

//-----------------------------------------------------------------------------

inline double Tag::getSize() const
{
    return m_size;
}

//-----------------------------------------------------------------------------

inline double& Tag::getSize()
{
    return m_size;
}

//-----------------------------------------------------------------------------

inline void Tag::setSize(const double _size)
{
    m_size = _size;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
