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
#include "data/object.hpp"

#include <array>
#include <map>
#include <vector>

namespace sight::data
{

/**
 * @brief   This class defines a list of 3D points inside groups.
 */
class DATA_CLASS_API landmarks : public object
{
public:

    typedef std::array<float, 4> color_t;
    typedef std::array<double, 3> point_t;
    typedef float size_t;
    typedef std::vector<point_t> PointContainer;
    typedef std::vector<std::string> GroupNameContainer;

    enum class Shape
    {
        SPHERE,
        CUBE
    };

    struct DATA_CLASS_API LandmarksGroup
    {
        LandmarksGroup(color_t _color, size_t _size, Shape _shape, bool _visibility) :
            m_color(_color),
            m_size(_size),
            m_shape(_shape),
            m_visibility(_visibility)
        {
        }

        /// Equality comparison operators
        /// @{
        DATA_API bool operator==(const LandmarksGroup& _other) const noexcept;
        DATA_API bool operator!=(const LandmarksGroup& _other) const noexcept;
        /// @}

        color_t m_color;
        size_t m_size;
        Shape m_shape;
        bool m_visibility;
        PointContainer m_points;
    };

    SIGHT_DECLARE_CLASS(landmarks, object);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API landmarks();

    /// Destructor
    DATA_API ~landmarks() noexcept override = default;

    /**
     * @brief Add a new landmark group
     *
     * @throw data::exception if a group exists with the given name
     */
    DATA_API void addGroup(
        const std::string& _name,
        const color_t& _color = {{1.0F, 1.0F, 1.0F, 1.0F}},
        size_t _size          = 1.0F,
        Shape _shape          = Shape::SPHERE,
        bool _visibility      = true
    );

    /// Return all group names
    DATA_API GroupNameContainer getGroupNames() const;

    /**
     * @brief Get the group properties
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API const LandmarksGroup& getGroup(const std::string& _name) const;

    /**
     * @brief Get the group properties
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API LandmarksGroup& getGroup(const std::string& _name);

    /**
     * @brief Check if the group exists
     */
    DATA_API bool hasGroup(const std::string& _name) const noexcept;

    /**
     * @brief Rename the group
     *
     * @throw data::exception if the group does not exist or if a group exists with the new name
     */
    DATA_API void renameGroup(const std::string& _old_name, const std::string& _new_name);

    /**
     * @brief Remove the group
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void removeGroup(const std::string& _name);

    /**
     * @brief Set the group's color.
     *
     * The color is encoded as a rgba float array ranging from 0 to 1
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void setGroupColor(const std::string& _name, const color_t& _color);

    /**
     * @brief Set the group's landmark size
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void setGroupSize(const std::string& _name, size_t _size);

    /**
     * @brief Set the group's shape (SPHERE or CUBE)
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void setGroupShape(const std::string& _name, Shape _shape);

    /**
     * @brief Set the group's visibility
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void setGroupVisibility(const std::string& _name, bool _visibility);

    /**
     * @brief Add a point to the group
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void addPoint(const std::string& _name, const point_t& _point);

    /**
     * @brief Insert a point to the group at the given index
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void insertPoint(const std::string& _name, std::size_t _index, const point_t& _point);

    /**
     * @brief Get the point at the given index
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API const point_t& getPoint(const std::string& _name, std::size_t _index) const;

    /**
     * @brief Get the point at the given index
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API point_t& getPoint(const std::string& _name, std::size_t _index);

    /**
     * @brief Get all group's points
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API const PointContainer& getPoints(const std::string& _name) const;

    /**
     * @brief Remove the point at the given index
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void removePoint(const std::string& _name, std::size_t _index);

    /**
     * @brief Remove all the group's points
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void clearPoints(const std::string& _name);

    /// Return the number of groups
    std::size_t numGroups() const;

    /// Return the number of points in the entire structure
    DATA_API std::size_t numPoints() const;

    /// Return the number of points in the group
    DATA_API std::size_t numPoints(const std::string& _name) const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when a group is added
    typedef core::com::signal<void (std::string _name)> group_added_signal_t;
    DATA_API static const core::com::signals::key_t GROUP_ADDED_SIG;

    /// Type of signal when a group is removed
    typedef core::com::signal<void (std::string _name)> group_removed_signal_t;
    DATA_API static const core::com::signals::key_t GROUP_REMOVED_SIG;

    /// Type of signal when a point is added
    typedef core::com::signal<void (std::string _name)> point_added_signal_t;
    DATA_API static const core::com::signals::key_t POINT_ADDED_SIG;

    /// Type of signal when a point is removed
    typedef core::com::signal<void (std::string _name, std::size_t _index)> point_removed_signal_t;
    DATA_API static const core::com::signals::key_t POINT_REMOVED_SIG;

    /// Type of signal when a point is inserted
    typedef core::com::signal<void (std::string _name, std::size_t _index)> point_inserted_signal_t;
    DATA_API static const core::com::signals::key_t POINT_INSERTED_SIG;

    /// Type of signal when a point is modified
    typedef core::com::signal<void (std::string _name, std::size_t _index)> point_modified_sig_t;
    DATA_API static const core::com::signals::key_t POINT_MODIFIED_SIG;

    /// Type of signal when group properties changed
    typedef core::com::signal<void (std::string _name)> group_modified_signal_t;
    DATA_API static const core::com::signals::key_t GROUP_MODIFIED_SIG;

    /// Type of signal when a group is renamed
    typedef core::com::signal<void (std::string _old_name, std::string _new_name)> group_renamed_signal_t;
    DATA_API static const core::com::signals::key_t GROUP_RENAMED_SIG;

    /// Type of signal when point is selected
    typedef core::com::signal<void (std::string _name, std::size_t _index)> point_selected_signal_t;
    DATA_API static const core::com::signals::key_t POINT_SELECTED_SIG;

    /// Type of signal when point is deselected
    typedef core::com::signal<void (std::string _name, std::size_t _index)> point_deselected_signal_t;
    DATA_API static const core::com::signals::key_t POINT_DESELECTED_SIG;

/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const landmarks& _other) const noexcept;
    DATA_API bool operator!=(const landmarks& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

private:

    typedef std::map<std::string, LandmarksGroup> LandmarksContainer;
    LandmarksContainer m_landmarks;
}; // end class landmarks

//------------------------------------------------------------------------------

inline std::size_t landmarks::numGroups() const
{
    return m_landmarks.size();
}

} // end namespace sight::data
