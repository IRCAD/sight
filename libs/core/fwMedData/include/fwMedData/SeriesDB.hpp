/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwMedData/config.hpp"
#include "fwMedData/types.hpp"

#include <data/factory/new.hpp>
#include <data/Object.hpp>

#include <vector>

fwCampAutoDeclareDataMacro((fwMedData)(SeriesDB))

namespace fwMedData
{

class Series;

/**
 * @brief Holds series data.
 */
class FWMEDDATA_CLASS_API SeriesDB : public data::Object
{

public:
    fwCoreClassMacro(SeriesDB, data::Object, data::factory::New< SeriesDB >)

    fwCampMakeFriendDataMacro((fwMedData)(SeriesDB))

    typedef std::vector< SPTR(Series) > ContainerType;

    typedef ContainerType::value_type ValueType;
    typedef ContainerType::reference ReferenceType;
    typedef ContainerType::const_reference ConstReferenceType;
    typedef ContainerType::iterator IteratorType;
    typedef ContainerType::const_iterator ConstIteratorType;
    typedef ContainerType::reverse_iterator ReverseIteratorType;
    typedef ContainerType::const_reverse_iterator ConstReverseIteratorType;
    typedef ContainerType::size_type SizeType;

    /// boost_foreach/stl compatibility
    /// @{
    typedef ContainerType::value_type value_type;
    typedef ContainerType::iterator iterator;
    typedef ContainerType::const_iterator const_iterator;
    typedef ContainerType::reverse_iterator reverse_iterator;
    typedef ContainerType::const_reverse_iterator const_reverse_iterator;
    typedef ContainerType::size_type size_type;

    IteratorType begin();
    IteratorType end();
    ConstIteratorType begin() const;
    ConstIteratorType end()   const;

    ReverseIteratorType rbegin();
    ReverseIteratorType rend();
    ConstReverseIteratorType rbegin() const;
    ConstReverseIteratorType rend()   const;

    bool empty() const;
    SizeType size() const;

    ValueType front();
    ValueType back();

    ReferenceType operator[] ( size_type n );
    ConstReferenceType operator[] ( size_type n ) const;

    ReferenceType at ( SizeType n );
    ConstReferenceType at ( SizeType n ) const;
    /// @}

    /**
     * @brief Creates the series DB.
     * @param _key private construction key.
     */
    FWMEDDATA_API SeriesDB(data::Object::Key _key);

    /// Destroys the series DB.
    FWMEDDATA_API virtual ~SeriesDB();

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    FWMEDDATA_API void shallowCopy( const data::Object::csptr& _source ) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache contains all copied objects to avoid duplication.
     */
    FWMEDDATA_API void cachedDeepCopy( const data::Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /// Gets the series container.
    ContainerType& getContainer();

    /// Gets the series const container.
    const ContainerType& getContainer() const;

    /// Sets the series container.
    void setContainer (const ContainerType& _val);

    /**
     * @name Signals
     * @{
     */
    /// Defines the type of signal sent when series are added.
    typedef core::com::Signal< void (ContainerType) > AddedSeriesSignalType;
    FWMEDDATA_API static const core::com::Signals::SignalKeyType s_ADDED_SERIES_SIG;

    /// Defines the type of signal sent when series are removed.
    typedef core::com::Signal< void (ContainerType) > RemovedSeriesSignalType;
    FWMEDDATA_API static const core::com::Signals::SignalKeyType s_REMOVED_SERIES_SIG;
    /**
     * @}
     */

protected:

    /// Stores all series.
    ContainerType m_container;

};

//-----------------------------------------------------------------------------

inline SeriesDB::IteratorType SeriesDB::begin()
{
    return m_container.begin();
}

//-----------------------------------------------------------------------------

inline SeriesDB::IteratorType SeriesDB::end()
{
    return m_container.end();
}

//-----------------------------------------------------------------------------

inline SeriesDB::ConstIteratorType SeriesDB::begin() const
{
    return m_container.begin();
}

//-----------------------------------------------------------------------------

inline SeriesDB::ConstIteratorType SeriesDB::end() const
{
    return m_container.end();
}

//-----------------------------------------------------------------------------

inline SeriesDB::ReverseIteratorType SeriesDB::rbegin()
{
    return m_container.rbegin();
}

//-----------------------------------------------------------------------------

inline SeriesDB::ReverseIteratorType SeriesDB::rend()
{
    return m_container.rend();
}

//-----------------------------------------------------------------------------

inline SeriesDB::ConstReverseIteratorType SeriesDB::rbegin() const
{
    return m_container.rbegin();
}

//-----------------------------------------------------------------------------

inline SeriesDB::ConstReverseIteratorType SeriesDB::rend() const
{
    return m_container.rend();
}

//-----------------------------------------------------------------------------

inline bool SeriesDB::empty() const
{
    return m_container.empty();
}

//-----------------------------------------------------------------------------

inline SeriesDB::SizeType SeriesDB::size() const
{
    return m_container.size();
}

//-----------------------------------------------------------------------------

inline SeriesDB::ValueType SeriesDB::front()
{
    return m_container.front();
}

//-----------------------------------------------------------------------------

inline SeriesDB::ValueType SeriesDB::back()
{
    return m_container.back();
}

//-----------------------------------------------------------------------------

inline SeriesDB::ReferenceType SeriesDB::operator[](SeriesDB::size_type n)
{
    return this->m_container[n];
}

//-----------------------------------------------------------------------------

inline SeriesDB::ConstReferenceType SeriesDB::operator[](SeriesDB::size_type n) const
{
    return this->m_container[n];
}

//-----------------------------------------------------------------------------

inline SeriesDB::ReferenceType SeriesDB::at(SeriesDB::SizeType n)
{
    return m_container.at(n);
}

//-----------------------------------------------------------------------------

inline SeriesDB::ConstReferenceType SeriesDB::at(SeriesDB::SizeType n) const
{
    return m_container.at(n);
}

//-----------------------------------------------------------------------------

inline SeriesDB::ContainerType& SeriesDB::getContainer()
{
    return m_container;
}

//-----------------------------------------------------------------------------

inline const SeriesDB::ContainerType& SeriesDB::getContainer () const
{
    return m_container;
}

//-----------------------------------------------------------------------------

inline void SeriesDB::setContainer (const SeriesDB::ContainerType& _val)
{
    m_container = _val;
}

//-----------------------------------------------------------------------------

} // Namespace fwMedData.
