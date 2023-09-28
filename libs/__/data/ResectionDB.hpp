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
#include "data/Resection.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

namespace sight::data
{

/**
 * @brief   This class defines a resection container.
 *
 * @see     Resection
 */
class DATA_CLASS_API ResectionDB final : public Object
{
public:

    SIGHT_DECLARE_CLASS(ResectionDB, Object);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API ResectionDB();

    /// Destructor
    DATA_API ~ResectionDB() noexcept override = default;

    typedef std::vector<Resection::sptr> ResectionContainerType;

    /**
     * @brief Get the number of resections
     */
    DATA_API ResectionContainerType::size_type numResections() const;

    /**
     * @brief add resection
     */
    DATA_API void addResection(const Resection::sptr& resection);

    /**
     * @{
     * @brief Get/Set value of the resections.
     */
    const ResectionContainerType& getResections() const;
    void setResections(const ResectionContainerType& val);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the safe resection.
     */
    Resection::sptr getSafeResection();
    Resection::csptr getSafeResection() const;
    void setSafeResection(const Resection::sptr& _safeResection);
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
    typedef core::com::signal<void ()> ResectionAddedSignalType;

    /// Key in m_signals map of signal m_sigResectionAdded
    DATA_API static const core::com::signals::key_t RESECTION_ADDED_SIG;

    /**
     * @}
     */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const ResectionDB& other) const noexcept;
    DATA_API bool operator!=(const ResectionDB& other) const noexcept;
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

protected:

    Resection::sptr m_safeResection;

    ResectionContainerType m_resections;

private:

    /// Signal emitted when the safe part is added
    SafePartAddedSignalType::sptr m_sigSafePartAdded;

    /// Signal emitted when a resection is added
    ResectionAddedSignalType::sptr m_sigResectionAdded;
};

//-----------------------------------------------------------------------------

inline const ResectionDB::ResectionContainerType& ResectionDB::getResections() const
{
    return m_resections;
}

//-----------------------------------------------------------------------------

inline void ResectionDB::setResections(const ResectionDB::ResectionContainerType& val)
{
    m_resections = val;
}

//-----------------------------------------------------------------------------

inline Resection::sptr ResectionDB::getSafeResection()
{
    return m_safeResection;
}

//-----------------------------------------------------------------------------

inline Resection::csptr ResectionDB::getSafeResection() const
{
    return m_safeResection;
}

//-----------------------------------------------------------------------------

inline void ResectionDB::setSafeResection(const Resection::sptr& _safeResection)
{
    m_safeResection = _safeResection;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
