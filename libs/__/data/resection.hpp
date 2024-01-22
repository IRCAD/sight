/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
#include "data/plane_list.hpp"
#include "data/reconstruction.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include <vector>

namespace sight::data
{

/**
 * @brief This class defines a resection.
 */
class DATA_CLASS_API resection final : public object
{
public:

    SIGHT_DECLARE_CLASS(resection, object);

    using resection_inputs  = std::vector<reconstruction::sptr>;
    using resection_outputs = std::vector<reconstruction::sptr>;

    /**
     * @brief Constructor
     */
    DATA_API resection();

    /// Destructor
    DATA_API ~resection() noexcept override = default;

    /**
     * @{
     * @brief Get/Set value of the planeList.
     */
    plane_list::sptr& get_plane_list();
    const plane_list::sptr& get_plane_list() const;
    void set_plane_list(const plane_list::sptr& _plane_list);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the inputs.
     */
    resection_inputs& get_inputs();
    const resection_inputs& get_inputs() const;
    void set_inputs(const resection_inputs& _v_inputs);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the outputs.
     */
    resection_outputs& get_outputs();
    const resection_outputs& get_outputs() const;
    void set_outputs(const resection_outputs& _v_outputs);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the IsSafePart.
     */
    bool get_is_safe_part() const;
    void set_is_safe_part(bool _is_safe_part);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the name.
     */
    std::string& get_name();
    const std::string& get_name() const;
    void set_name(const std::string& _name);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the IsVisible.
     */
    bool get_is_visible() const;
    void set_is_visible(bool _is_visible);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the IsValid.
     */
    bool get_is_valid() const;
    void set_is_valid(bool _is_valid);
    /// @}

    /***
     * @name Signals
     * @{
     */

    /// Type of signal when a reconstruction is added
    using reconstruction_added_signal_t = core::com::signal<void ()>;

    /// Key in m_signals map of signal m_sigReconstructionAdded
    DATA_API static const core::com::signals::key_t RECONSTRUCTION_ADDED_SIG;

    /// Type of signal when the resection visibility is modified
    using visibility_modified_signal_t = core::com::signal<void ()>;

    /// Key in m_signals map of signal m_sigVisibilityModified
    DATA_API static const core::com::signals::key_t VISIBILITY_MODIFIED_SIG;
    /**
     * @}
     */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const resection& _other) const noexcept;
    DATA_API bool operator!=(const resection& _other) const noexcept;
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

protected:

    //! resection name
    std::string m_name;

    //! Planes list
    plane_list::sptr m_plane_list;

    //! Inputs (reconstructions)
    resection_inputs m_v_inputs;

    //! Outputs (reconstructions)
    resection_outputs m_v_outputs;

    //! flag if the part is safe
    bool m_is_safe_part {true};

    //! flag if the resection is valid
    bool m_is_valid {false};

    //! flag if the resection is visible
    bool m_visible {true};

private:

    /**
     * @name Signals attributes
     * @{
     */
    /// Signal emitted when a reconstruction is added
    reconstruction_added_signal_t::sptr m_sig_reconstruction_added;

    /// Signal emitted when the resection visibility is modified
    visibility_modified_signal_t::sptr m_sig_visibility_modified;
    /**
     * @}
     */
}; // end class resection

//-----------------------------------------------------------------------------

inline plane_list::sptr& resection::get_plane_list()
{
    return m_plane_list;
}

//-----------------------------------------------------------------------------

inline const plane_list::sptr& resection::get_plane_list() const
{
    return m_plane_list;
}

//-----------------------------------------------------------------------------

inline void resection::set_plane_list(const plane_list::sptr& _plane_list)
{
    m_plane_list = _plane_list;
}

//-----------------------------------------------------------------------------

inline resection::resection_inputs& resection::get_inputs()
{
    return m_v_inputs;
}

//-----------------------------------------------------------------------------

inline const resection::resection_inputs& resection::get_inputs() const
{
    return m_v_inputs;
}

//-----------------------------------------------------------------------------

inline void resection::set_inputs(const resection::resection_inputs& _v_inputs)
{
    m_v_inputs = _v_inputs;
}

//-----------------------------------------------------------------------------

inline resection::resection_outputs& resection::get_outputs()
{
    return m_v_outputs;
}

//-----------------------------------------------------------------------------

inline const resection::resection_outputs& resection::get_outputs() const
{
    return m_v_outputs;
}

//-----------------------------------------------------------------------------

inline void resection::set_outputs(const resection::resection_outputs& _v_outputs)
{
    m_v_outputs = _v_outputs;
}

//-----------------------------------------------------------------------------

inline bool resection::get_is_safe_part() const
{
    return m_is_safe_part;
}

//-----------------------------------------------------------------------------

inline void resection::set_is_safe_part(const bool _is_safe_part)
{
    m_is_safe_part = _is_safe_part;
}

//-----------------------------------------------------------------------------

inline std::string& resection::get_name()
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline const std::string& resection::get_name() const
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline void resection::set_name(const std::string& _name)
{
    m_name = _name;
}

//-----------------------------------------------------------------------------

inline bool resection::get_is_visible() const
{
    return m_visible;
}

//-----------------------------------------------------------------------------

inline void resection::set_is_visible(const bool _is_visible)
{
    m_visible = _is_visible;
}

//-----------------------------------------------------------------------------

inline bool resection::get_is_valid() const
{
    return m_is_valid;
}

//-----------------------------------------------------------------------------

inline void resection::set_is_valid(const bool _is_valid)
{
    m_is_valid = _is_valid;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
