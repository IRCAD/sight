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

#include <sight/data/config.hpp>

#include "data/factory/new.hpp"
#include "data/resection.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

namespace sight::data
{

/**
 * @brief   This class defines a resection container.
 *
 * @see     Resection
 */
class SIGHT_DATA_CLASS_API resection_db final : public object
{
public:

    SIGHT_DECLARE_CLASS(resection_db, object);

    /**
     * @brief Constructor
     */
    SIGHT_DATA_API resection_db();

    /// Destructor
    SIGHT_DATA_API ~resection_db() noexcept override = default;

    using resection_container_t = std::vector<resection::sptr>;

    /**
     * @brief Get the number of resections
     */
    SIGHT_DATA_API resection_container_t::size_type num_resections() const;

    /**
     * @brief add resection
     */
    SIGHT_DATA_API void add_resection(const resection::sptr& _resection);

    /**
     * @{
     * @brief Get/Set value of the resections.
     */
    const resection_container_t& get_resections() const;
    void set_resections(const resection_container_t& _val);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the safe resection.
     */
    resection::sptr get_safe_resection();
    resection::csptr get_safe_resection() const;
    void set_safe_resection(const resection::sptr& _safe_resection);
    /// @}

    /***
     * @name Signals
     * @{
     */

    /// Type of signal when the safe part is added
    using safe_part_added_signal_t = core::com::signal<void ()>;

    /// Key in m_signals map of signal m_sigSafePartAdded
    SIGHT_DATA_API static const core::com::signals::key_t SAFE_PART_ADDED_SIG;

    /// Type of signal when a resection is added
    using resection_added_signal_t = core::com::signal<void ()>;

    /// Key in m_signals map of signal m_sigResectionAdded
    SIGHT_DATA_API static const core::com::signals::key_t RESECTION_ADDED_SIG;

    /**
     * @}
     */

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const resection_db& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const resection_db& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    resection::sptr m_safe_resection;

    resection_container_t m_resections;

private:

    /// Signal emitted when the safe part is added
    safe_part_added_signal_t::sptr m_sig_safe_part_added;

    /// Signal emitted when a resection is added
    resection_added_signal_t::sptr m_sig_resection_added;
};

//-----------------------------------------------------------------------------

inline const resection_db::resection_container_t& resection_db::get_resections() const
{
    return m_resections;
}

//-----------------------------------------------------------------------------

inline void resection_db::set_resections(const resection_db::resection_container_t& _val)
{
    m_resections = _val;
}

//-----------------------------------------------------------------------------

inline resection::sptr resection_db::get_safe_resection()
{
    return m_safe_resection;
}

//-----------------------------------------------------------------------------

inline resection::csptr resection_db::get_safe_resection() const
{
    return m_safe_resection;
}

//-----------------------------------------------------------------------------

inline void resection_db::set_safe_resection(const resection::sptr& _safe_resection)
{
    m_safe_resection = _safe_resection;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
