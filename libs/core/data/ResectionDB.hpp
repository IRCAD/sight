/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

namespace sight::data
{

/**
 * @brief   This class defines a resection container.
 *
 * @see     Resection
 */
class DATA_CLASS_API ResectionDB : public Object
{
public:

    SIGHT_DECLARE_CLASS(ResectionDB, Object, factory::New<ResectionDB>);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API ResectionDB(Object::Key key);

    /// Destructor
    DATA_API ~ResectionDB() override;

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

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
    typedef core::com::Signal<void ()> SafePartAddedSignalType;

    /// Key in m_signals map of signal m_sigSafePartAdded
    DATA_API static const core::com::Signals::SignalKeyType s_SAFE_PART_ADDED_SIG;

    /// Type of signal when a resection is added
    typedef core::com::Signal<void ()> ResectionAddedSignalType;

    /// Key in m_signals map of signal m_sigResectionAdded
    DATA_API static const core::com::Signals::SignalKeyType s_RESECTION_ADDED_SIG;

    /**
     * @}
     */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const ResectionDB& other) const noexcept;
    DATA_API bool operator!=(const ResectionDB& other) const noexcept;
    /// @}

protected:

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

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
