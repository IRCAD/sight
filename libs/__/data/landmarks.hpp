/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

    using color_t            = std::array<float, 4>;
    using point_t            = std::array<double, 3>;
    using size_t             = float;
    using point_container    = std::vector<point_t>;
    using GroupNameContainer = std::vector<std::string>;

    enum class shape
    {
        sphere,
        cube
    };

    struct DATA_CLASS_API landmarks_group
    {
        landmarks_group(color_t _color, size_t _size, shape _shape, bool _visibility) :
            m_color(_color),
            m_size(_size),
            m_shape(_shape),
            m_visibility(_visibility)
        {
        }

        /// Equality comparison operators
        /// @{
        DATA_API bool operator==(const landmarks_group& _other) const noexcept;
        DATA_API bool operator!=(const landmarks_group& _other) const noexcept;
        /// @}

        color_t m_color;
        size_t m_size;
        shape m_shape;
        bool m_visibility;
        point_container m_points;
    };

    SIGHT_DECLARE_CLASS(landmarks, object);

    /**
     * @brief Constructor
     */
    DATA_API landmarks();

    /// Destructor
    DATA_API ~landmarks() noexcept override = default;

    /**
     * @brief Add a new landmark group
     *
     * @throw data::exception if a group exists with the given name
     */
    DATA_API void add_group(
        const std::string& _name,
        const color_t& _color = {{1.0F, 1.0F, 1.0F, 1.0F}},
        size_t _size          = 1.0F,
        shape _shape          = shape::sphere,
        bool _visibility      = true
    );

    /// Return all group names
    DATA_API GroupNameContainer get_group_names() const;

    /**
     * @brief Get the group properties
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API const landmarks_group& get_group(const std::string& _name) const;

    /**
     * @brief Get the group properties
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API landmarks_group& get_group(const std::string& _name);

    /**
     * @brief Check if the group exists
     */
    DATA_API bool has_group(const std::string& _name) const noexcept;

    /**
     * @brief Rename the group
     *
     * @throw data::exception if the group does not exist or if a group exists with the new name
     */
    DATA_API void rename_group(const std::string& _old_name, const std::string& _new_name);

    /**
     * @brief Remove the group
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void remove_group(const std::string& _name);

    /**
     * @brief Set the group's color.
     *
     * The color is encoded as a rgba float array ranging from 0 to 1
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void set_group_color(const std::string& _name, const color_t& _color);

    /**
     * @brief Set the group's landmark size
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void set_group_size(const std::string& _name, size_t _size);

    /**
     * @brief Set the group's shape (SPHERE or CUBE)
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void set_group_shape(const std::string& _name, shape _shape);

    /**
     * @brief Set the group's visibility
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void set_group_visibility(const std::string& _name, bool _visibility);

    /**
     * @brief Add a point to the group
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void add_point(const std::string& _name, const point_t& _point);

    /**
     * @brief Insert a point to the group at the given index
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void insert_point(const std::string& _name, std::size_t _index, const point_t& _point);

    /**
     * @brief Get the point at the given index
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API const point_t& get_point(const std::string& _name, std::size_t _index) const;

    /**
     * @brief Get the point at the given index
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API point_t& get_point(const std::string& _name, std::size_t _index);

    /**
     * @brief Get all group's points
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API const point_container& get_points(const std::string& _name) const;

    /**
     * @brief Remove the point at the given index
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void remove_point(const std::string& _name, std::size_t _index);

    /**
     * @brief Remove all the group's points
     *
     * @throw data::exception if the group does not exist
     */
    DATA_API void clear_points(const std::string& _name);

    /// Return the number of groups
    std::size_t num_groups() const;

    /// Return the number of points in the entire structure
    DATA_API std::size_t num_points() const;

    /// Return the number of points in the group
    DATA_API std::size_t num_points(const std::string& _name) const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when a group is added
    using group_added_signal_t = core::com::signal<void (std::string)>;
    DATA_API static const core::com::signals::key_t GROUP_ADDED_SIG;

    /// Type of signal when a group is removed
    using group_removed_signal_t = core::com::signal<void (std::string)>;
    DATA_API static const core::com::signals::key_t GROUP_REMOVED_SIG;

    /// Type of signal when a point is added
    using point_added_signal_t = core::com::signal<void (std::string)>;
    DATA_API static const core::com::signals::key_t POINT_ADDED_SIG;

    /// Type of signal when a point is removed
    using point_removed_signal_t = core::com::signal<void (std::string, std::size_t)>;
    DATA_API static const core::com::signals::key_t POINT_REMOVED_SIG;

    /// Type of signal when a point is inserted
    using point_inserted_signal_t = core::com::signal<void (std::string, std::size_t)>;
    DATA_API static const core::com::signals::key_t POINT_INSERTED_SIG;

    /// Type of signal when a point is modified
    using point_modified_sig_t = core::com::signal<void (std::string, std::size_t)>;
    DATA_API static const core::com::signals::key_t POINT_MODIFIED_SIG;

    /// Type of signal when group properties changed
    using group_modified_signal_t = core::com::signal<void (std::string)>;
    DATA_API static const core::com::signals::key_t GROUP_MODIFIED_SIG;

    /// Type of signal when a group is renamed
    using group_renamed_signal_t = core::com::signal<void (std::string, std::string)>;
    DATA_API static const core::com::signals::key_t GROUP_RENAMED_SIG;

    /// Type of signal when point is selected
    using point_selected_signal_t = core::com::signal<void (std::string, std::size_t)>;
    DATA_API static const core::com::signals::key_t POINT_SELECTED_SIG;

    /// Type of signal when point is deselected
    using point_deselected_signal_t = core::com::signal<void (std::string, std::size_t)>;
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
    /// @param[in] _source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

private:

    using landmarks_container = std::map<std::string, landmarks_group>;
    landmarks_container m_landmarks;
}; // end class landmarks

//------------------------------------------------------------------------------

inline std::size_t landmarks::num_groups() const
{
    return m_landmarks.size();
}

} // end namespace sight::data
