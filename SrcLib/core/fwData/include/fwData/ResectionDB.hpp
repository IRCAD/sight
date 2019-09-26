/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Resection.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

fwCampAutoDeclareDataMacro((fwData)(ResectionDB), FWDATA_API);

namespace fwData
{
/**
 * @brief   This class defines a resection container.
 *
 * @see     ::fwData::Resection
 */
class FWDATA_CLASS_API ResectionDB : public Object
{

public:
    fwCoreClassMacro(ResectionDB, ::fwData::Object, ::fwData::factory::New< ResectionDB >);
    fwCampMakeFriendDataMacro((fwData)(ResectionDB));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API ResectionDB(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~ResectionDB();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    typedef std::vector< ::fwData::Resection::sptr > ResectionContainerType;

    /**
     * @brief Get the number of resections
     */
    FWDATA_API ResectionContainerType::size_type getNumberOfResections() const;

    /**
     * @brief add resection
     */
    FWDATA_API void addResection(const ::fwData::Resection::sptr& resection );

    /**
     * @{
     * @brief Get/Set value of the resections.
     */
    const ResectionContainerType& getResections () const;
    void setResections (const ResectionContainerType& val);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the safe resection.
     */
    ::fwData::Resection::sptr& getSafeResection();
    const ::fwData::Resection::sptr& getSafeResection() const;
    void setSafeResection(const ::fwData::Resection::sptr& _safeResection);
    /// @}

    /***
     * @name Signals
     * @{
     */

    /// Type of signal when the safe part is added
    typedef ::fwCom::Signal< void () > SafePartAddedSignalType;

    /// Key in m_signals map of signal m_sigSafePartAdded
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_SAFE_PART_ADDED_SIG;

    /// Type of signal when a resection is added
    typedef ::fwCom::Signal< void () > ResectionAddedSignalType;

    /// Key in m_signals map of signal m_sigResectionAdded
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_RESECTION_ADDED_SIG;

    /**
     * @}
     */

protected:

    ::fwData::Resection::sptr m_safeResection;

    ResectionContainerType m_resections;

private:

    /// Signal emitted when the safe part is added
    SafePartAddedSignalType::sptr m_sigSafePartAdded;

    /// Signal emitted when a resection is added
    ResectionAddedSignalType::sptr m_sigResectionAdded;
};

//-----------------------------------------------------------------------------

inline const ResectionDB::ResectionContainerType& ResectionDB::getResections () const
{
    return m_resections;
}

//-----------------------------------------------------------------------------

inline void ResectionDB::setResections (const ResectionDB::ResectionContainerType& val)
{
    m_resections = val;
}

//-----------------------------------------------------------------------------

inline ::fwData::Resection::sptr& ResectionDB::getSafeResection()
{
    return m_safeResection;
}

//-----------------------------------------------------------------------------

inline const ::fwData::Resection::sptr& ResectionDB::getSafeResection() const
{
    return m_safeResection;
}

//-----------------------------------------------------------------------------

inline void ResectionDB::setSafeResection(const ::fwData::Resection::sptr& _safeResection)
{
    m_safeResection = _safeResection;
}

//-----------------------------------------------------------------------------

} // namespace fwData
