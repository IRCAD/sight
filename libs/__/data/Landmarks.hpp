/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

    struct DATA_CLASS_API LandmarksGroup
    {
        LandmarksGroup(ColorType color, SizeType size, Shape shape, bool visibility) :
            m_color(color),
            m_size(size),
            m_shape(shape),
            m_visibility(visibility)
        {
        }

        /// Equality comparison operators
        /// @{
        DATA_API bool operator==(const LandmarksGroup& other) const noexcept;
        DATA_API bool operator!=(const LandmarksGroup& other) const noexcept;
        /// @}

        ColorType m_color;
        SizeType m_size;
        Shape m_shape;
        bool m_visibility;
        PointContainer m_points;
    };

    SIGHT_DECLARE_CLASS(Landmarks, Object);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Landmarks();

    /// Destructor
    DATA_API ~Landmarks() noexcept override = default;

    /**
     * @brief Add a new landmark group
     *
     * @throw data::Exception if a group exists with the given name
     */
    DATA_API void addGroup(
        const std::string& name,
        const ColorType& color = {{1.0F, 1.0F, 1.0F, 1.0F}},
        SizeType size          = 1.0F,
        Shape shape            = Shape::SPHERE,
        bool visibility        = true
    );

    /// Return all group names
    DATA_API GroupNameContainer getGroupNames() const;

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
     * @brief Check if the group exists
     */
    DATA_API bool hasGroup(const std::string& name) const noexcept;

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
    DATA_API void setGroupSize(const std::string& name, SizeType size);

    /**
     * @brief Set the group's shape (SPHERE or CUBE)
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API void setGroupShape(const std::string& name, Shape shape);

    /**
     * @brief Set the group's visibility
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API void setGroupVisibility(const std::string& name, bool visibility);

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
    DATA_API void insertPoint(const std::string& name, std::size_t index, const PointType& point);

    /**
     * @brief Get the point at the given index
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API const PointType& getPoint(const std::string& name, std::size_t index) const;

    /**
     * @brief Get the point at the given index
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API PointType& getPoint(const std::string& name, std::size_t index);

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
    DATA_API void removePoint(const std::string& name, std::size_t index);

    /**
     * @brief Remove all the group's points
     *
     * @throw data::Exception if the group does not exist
     */
    DATA_API void clearPoints(const std::string& name);

    /// Return the number of groups
    std::size_t numGroups() const;

    /// Return the number of points in the entire structure
    DATA_API std::size_t numPoints() const;

    /// Return the number of points in the group
    DATA_API std::size_t numPoints(const std::string& name) const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when a group is added
    typedef core::com::signal<void (std::string name)> GroupAddedSignalType;
    DATA_API static const core::com::signals::key_t GROUP_ADDED_SIG;

    /// Type of signal when a group is removed
    typedef core::com::signal<void (std::string name)> GroupRemovedSignalType;
    DATA_API static const core::com::signals::key_t GROUP_REMOVED_SIG;

    /// Type of signal when a point is added
    typedef core::com::signal<void (std::string name)> PointAddedSignalType;
    DATA_API static const core::com::signals::key_t POINT_ADDED_SIG;

    /// Type of signal when a point is removed
    typedef core::com::signal<void (std::string name, std::size_t index)> PointRemovedSignalType;
    DATA_API static const core::com::signals::key_t POINT_REMOVED_SIG;

    /// Type of signal when a point is inserted
    typedef core::com::signal<void (std::string name, std::size_t index)> PointInsertedSignalType;
    DATA_API static const core::com::signals::key_t POINT_INSERTED_SIG;

    /// Type of signal when a point is modified
    typedef core::com::signal<void (std::string name, std::size_t index)> PointModifiedSigType;
    DATA_API static const core::com::signals::key_t POINT_MODIFIED_SIG;

    /// Type of signal when group properties changed
    typedef core::com::signal<void (std::string name)> GroupModifiedSignalType;
    DATA_API static const core::com::signals::key_t GROUP_MODIFIED_SIG;

    /// Type of signal when a group is renamed
    typedef core::com::signal<void (std::string oldName, std::string newName)> GroupRenamedSignalType;
    DATA_API static const core::com::signals::key_t GROUP_RENAMED_SIG;

    /// Type of signal when point is selected
    typedef core::com::signal<void (std::string name, std::size_t index)> PointSelectedSignalType;
    DATA_API static const core::com::signals::key_t POINT_SELECTED_SIG;

    /// Type of signal when point is deselected
    typedef core::com::signal<void (std::string name, std::size_t index)> PointDeselectedSignalType;
    DATA_API static const core::com::signals::key_t POINT_DESELECTED_SIG;

/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Landmarks& other) const noexcept;
    DATA_API bool operator!=(const Landmarks& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;

private:

    typedef std::map<std::string, LandmarksGroup> LandmarksContainer;
    LandmarksContainer m_landmarks;
}; // end class Landmarks

//------------------------------------------------------------------------------

inline std::size_t Landmarks::numGroups() const
{
    return m_landmarks.size();
}

} // end namespace sight::data
