/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
class DATA_CLASS_API resection_db final : public object
{
public:

    SIGHT_DECLARE_CLASS(resection_db, object);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API resection_db();

    /// Destructor
    DATA_API ~resection_db() noexcept override = default;

    typedef std::vector<resection::sptr> resection_container_t;

    /**
     * @brief Get the number of resections
     */
    DATA_API resection_container_t::size_type numResections() const;

    /**
     * @brief add resection
     */
    DATA_API void addResection(const resection::sptr& _resection);

    /**
     * @{
     * @brief Get/Set value of the resections.
     */
    const resection_container_t& getResections() const;
    void setResections(const resection_container_t& _val);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the safe resection.
     */
    resection::sptr getSafeResection();
    resection::csptr getSafeResection() const;
    void setSafeResection(const resection::sptr& _safe_resection);
    /// @}

    /***
     * @name Signals
     * @{
     */

    /// Type of signal when the safe part is added
    typedef core::com::signal<void ()> SafePartAddedSignalType;

    /// Key in m_signals map of signal m_sigSafePartAdded
    DATA_API static const core::com::signals::key_t SAFE_PART_ADDED_SIG;

    /// Type of signal when a resection is added
    typedef core::com::signal<void ()> resection_added_signal_t;

    /// Key in m_signals map of signal m_sigResectionAdded
    DATA_API static const core::com::signals::key_t RESECTION_ADDED_SIG;

    /**
     * @}
     */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const resection_db& _other) const noexcept;
    DATA_API bool operator!=(const resection_db& _other) const noexcept;
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

protected:

    resection::sptr m_safeResection;

    resection_container_t m_resections;

private:

    /// Signal emitted when the safe part is added
    SafePartAddedSignalType::sptr m_sigSafePartAdded;

    /// Signal emitted when a resection is added
    resection_added_signal_t::sptr m_sigResectionAdded;
};

//-----------------------------------------------------------------------------

inline const resection_db::resection_container_t& resection_db::getResections() const
{
    return m_resections;
}

//-----------------------------------------------------------------------------

inline void resection_db::setResections(const resection_db::resection_container_t& _val)
{
    m_resections = _val;
}

//-----------------------------------------------------------------------------

inline resection::sptr resection_db::getSafeResection()
{
    return m_safeResection;
}

//-----------------------------------------------------------------------------

inline resection::csptr resection_db::getSafeResection() const
{
    return m_safeResection;
}

//-----------------------------------------------------------------------------

inline void resection_db::setSafeResection(const resection::sptr& _safe_resection)
{
    m_safeResection = _safe_resection;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
