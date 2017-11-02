/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_SERIESDB_HPP__
#define __FWMEDDATA_SERIESDB_HPP__

#include "fwMedData/config.hpp"
#include "fwMedData/types.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwData/factory/new.hpp>
#include <fwData/Object.hpp>

#include <vector>

fwCampAutoDeclareDataMacro((fwMedData)(SeriesDB), FWMEDDATA_API);

namespace fwMedData
{

class Series;

/**
 * @brief Holds series data
 */
class FWMEDDATA_CLASS_API SeriesDB : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (SeriesDB)(::fwData::Object), (()), ::fwData::factory::New< SeriesDB >);

    fwCampMakeFriendDataMacro((fwMedData)(SeriesDB));

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
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API SeriesDB(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~SeriesDB();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief Series container
     * @{ */
    ContainerType& getContainer();
    const ContainerType& getContainer () const;
    void setContainer (const ContainerType& val);
    /**  @} */

    /**  @} */

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when series are added
    typedef ::fwCom::Signal< void (ContainerType) > AddedSeriesSignalType;
    FWMEDDATA_API static const ::fwCom::Signals::SignalKeyType s_ADDED_SERIES_SIG;

    /// Type of signal when series are removed
    typedef ::fwCom::Signal< void (ContainerType) > RemovedSeriesSignalType;
    FWMEDDATA_API static const ::fwCom::Signals::SignalKeyType s_REMOVED_SERIES_SIG;
    /**
     * @}
     */

protected:

    /// Series container
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

inline void SeriesDB::setContainer (const SeriesDB::ContainerType& val)
{
    m_container = val;
}

//-----------------------------------------------------------------------------

}   //end namespace fwMedData

#endif // __FWMEDDATA_SERIESDB_HPP__

