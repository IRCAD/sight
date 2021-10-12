/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <array>
#include <map>
#include <vector>

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (Landmarks));

namespace sight::data
{

/**
 * @brief   This class defines a list of 3D points inside groups.
 */
class DATA_CLASS_API Landmarks : public Object
{
public:

    typedef std::array<float, 4> ColorType;
    typedef std::array<double, 3> PointType;
    typedef float SizeType;
    typedef std::vector<PointType> PointContainer;
    typedef std::vector<std::string> GroupNameContainer;

    enum class Shape
    {
        SPHERE,
        CUBE
    };

    struct LandmarksGroup
    {
        LandmarksGroup(ColorType color, SizeType size, Shape shape, bool visibility) :
            m_color(color),
            m_size(size),
            m_shape(shape),
            m_visibility(visibility)
        {
        }

        ColorType m_color;
        SizeType m_size;
        Shape m_shape;
        bool m_visibility;
        PointContainer m_points;
    };

    SIGHT_DECLARE_CLASS(Landmarks, data::Object, data::factory::New<Landmarks>);

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (Landmarks));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Landmarks(data::Object::Key key);

    /// Destructor
    DATA_API virtual ~Landmarks();

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief Add a new landmark group
     *
     * @throw data::Exception if a group exists with the given name
     */
    DATA_API void addGroup(
        const std::string& name,
        const ColorType& color = {{1.0f, 1.0f, 1.0f, 1.0f}},
        const SizeType size    = 1.0f,
        const Shape shape      = Shape::SPHERE,
        const bool visibility  = true
    );

    /// Return all group names
    DATA_API const GroupNameContainer getGroupNames() const;

    /**
     * @brief Get the group properties
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API const LandmarksGroup& getGroup(const std::string& name) const;

    /**
     * @brief Get the group properties
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API LandmarksGroup& getGroup(const std::string& name);

    /**
     * @brief Rename the group
     *
     * @throw data::Exception if the group does not exist or if a group exists with the new name
     */
    DATA_API void renameGroup(const std::string& oldName, const std::string& newName);

    /**
     * @brief Remove the group
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API void removeGroup(const std::string& name);

    /**
     * @brief Set the group's color.
     *
     * The color is encoded as a rgba float array ranging from 0 to 1
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API void setGroupColor(const std::string& name, const ColorType& color);

    /**
     * @brief Set the group's landmark size
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API void setGroupSize(const std::string& name, const SizeType size);

    /**
     * @brief Set the group's shape (SPHERE or CUBE)
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API void setGroupShape(const std::string& name, const Shape shape);

    /**
     * @brief Set the group's visibility
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API void setGroupVisibility(const std::string& name, const bool visibility);

    /**
     * @brief Add a point to the group
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API void addPoint(const std::string& name, const PointType& point);

    /**
     * @brief Insert a point to the group at the given index
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API void insertPoint(const std::string& name, const size_t index, const PointType& point);

    /**
     * @brief Get the point at the given index
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API const PointType& getPoint(const std::string& name, size_t index) const;

    /**
     * @brief Get the point at the given index
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API PointType& getPoint(const std::string& name, size_t index);

    /**
     * @brief Get all group's points
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API const PointContainer& getPoints(const std::string& name) const;

    /**
     * @brief Remove the point at the given index
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API void removePoint(const std::string& name, size_t index);

    /**
     * @brief Remove all the group's points
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API void clearPoints(const std::string& name);

    /// Return the number of groups
    size_t numGroups() const;

    /// Return the number of points in the entire structure
    DATA_API size_t numPoints() const;

    /// Return the number of points in the group
    DATA_API size_t numPoints(const std::string& name) const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when a group is added
    typedef core::com::Signal<void (std::string name)> GroupAddedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_GROUP_ADDED_SIG;

    /// Type of signal when a group is removed
    typedef core::com::Signal<void (std::string name)> GroupRemovedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_GROUP_REMOVED_SIG;

    /// Type of signal when a point is added
    typedef core::com::Signal<void (std::string name)> PointAddedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_ADDED_SIG;

    /// Type of signal when a point is removed
    typedef core::com::Signal<void (std::string name, size_t index)> PointRemovedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_REMOVED_SIG;

    /// Type of signal when a point is inserted
    typedef core::com::Signal<void (std::string name, size_t index)> PointInsertedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_INSERTED_SIG;

    /// Type of signal when a point is modified
    typedef core::com::Signal<void (std::string name, size_t index)> PointModifiedSigType;
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_MODIFIED_SIG;

    /// Type of signal when group properties changed
    typedef core::com::Signal<void (std::string name)> GroupModifiedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_GROUP_MODIFIED_SIG;

    /// Type of signal when a group is renamed
    typedef core::com::Signal<void (std::string oldName, std::string newName)> GroupRenamedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_GROUP_RENAMED_SIG;

    /// Type of signal when point is selected
    typedef core::com::Signal<void (std::string name, size_t index)> PointSelectedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_SELECTED_SIG;

    /// Type of signal when point is deselected
    typedef core::com::Signal<void (std::string name, size_t index)> PointDeselectedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_POINT_DESELECTED_SIG;

/**
 * @}
 */

private:

    typedef std::map<std::string, LandmarksGroup> LandmarksContainer;
    LandmarksContainer m_landmarks;
}; // end class Landmarks

//------------------------------------------------------------------------------

inline size_t Landmarks::numGroups() const
{
    return m_landmarks.size();
}

} // end namespace sight::data
