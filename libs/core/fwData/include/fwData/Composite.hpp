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
#include "fwData/Object.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <map>

namespace fwData
{
class Composite;
}

fwCampAutoDeclareDataMacro((fwData)(Composite), FWDATA_API);

namespace fwData
{

/**
 * @brief This class defines a composite object.
 *
 * Composite contains a map of ::fwData::Object.
 */

class FWDATA_CLASS_API Composite : public Object
{
public:
    fwCoreClassMacro(Composite, ::fwData::Object, ::fwData::factory::New< Composite >);
    fwCampMakeFriendDataMacro((fwData)(Composite));

    typedef std::map< std::string, ::fwData::Object::sptr > ContainerType;

    typedef ContainerType::key_type KeyType;
    typedef ContainerType::mapped_type MappedType;
    typedef ContainerType::value_type ValueType;
    typedef ContainerType::iterator IteratorType;
    typedef ContainerType::const_iterator ConstIteratorType;
    typedef ContainerType::reverse_iterator ReverseIteratorType;
    typedef ContainerType::const_reverse_iterator ConstReverseIteratorType;
    typedef ContainerType::size_type SizeType;

    /// boost_foreach/stl compatibility
    /// @{
    typedef ContainerType::key_type key_type;
    typedef ContainerType::mapped_type mapped_type;
    typedef ContainerType::value_type value_type;
    typedef ContainerType::iterator iterator;
    typedef ContainerType::const_iterator const_iterator;
    typedef ContainerType::reverse_iterator reverse_iterator;
    typedef ContainerType::const_reverse_iterator const_reverse_iterator;
    typedef ContainerType::size_type size_type;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Composite( ::fwData::Object::Key key );

    /// Destructor
    FWDATA_API virtual ~Composite();

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

    mapped_type& operator[] ( KeyType n );

    IteratorType find ( const KeyType& x );
    ConstIteratorType find ( const KeyType& x ) const;

    SizeType count ( const KeyType& x ) const;
    /// @}

    /// @brief get/set the map of std::string/::fwData::Object
    /// @{
    ContainerType& getContainer();
    const ContainerType& getContainer () const;
    void setContainer (const ContainerType& val);
    /// @}

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// Method to initialize a ::fwData::Composite from a std::map< string, X >
    template< class DATATYPE >
    void setDataContainer( const std::map< std::string, SPTR(DATATYPE) >& map );

    /// Method to get a std::map< string, X > from ::fwData::Composite
    template< class DATATYPE >
    std::map< std::string, SPTR(DATATYPE) > getDataContainer() const;

    /**
     * @brief Returns object in composite associated with the key.
     *  If no such object exists, a null object is returned.
     *
     *  @param key the key of the object to find
     *
     *  @return requested object in composite associated with the key
     */
    template< class DATATYPE >
    SPTR(DATATYPE) at(const std::string& key);

    /**
     * @brief Returns object in composite associated with the key.
     *  If no such object exists, a null object is returned.
     *
     *  @param key the key of the object to find
     *
     *  @return requested object in composite associated with the key
     */
    template< class DATATYPE >
    CSPTR(DATATYPE) at(const std::string& key) const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when objects are added
    typedef ::fwCom::Signal< void (ContainerType) > AddedObjectsSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_ADDED_OBJECTS_SIG;

    /// Type of signal when objects are changed (newObjects, oldObjects)
    typedef ::fwCom::Signal< void (ContainerType, ContainerType) > ChangedObjectsSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_CHANGED_OBJECTS_SIG;

    /// Type of signal when objects are removed
    typedef ::fwCom::Signal< void (ContainerType) > RemovedObjectsSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_REMOVED_OBJECTS_SIG;
    /**
     * @}
     */

protected:
    ContainerType m_container;
};

//-----------------------------------------------------------------------------

inline Composite::IteratorType Composite::begin()
{
    return m_container.begin();
}

//-----------------------------------------------------------------------------

inline Composite::IteratorType Composite::end()
{
    return m_container.end();
}

//-----------------------------------------------------------------------------

inline Composite::ConstIteratorType Composite::begin() const
{
    return m_container.begin();
}

//-----------------------------------------------------------------------------

inline Composite::ConstIteratorType Composite::end() const
{
    return m_container.end();
}

//-----------------------------------------------------------------------------

inline Composite::ReverseIteratorType Composite::rbegin()
{
    return m_container.rbegin();
}

//-----------------------------------------------------------------------------

inline Composite::ReverseIteratorType Composite::rend()
{
    return m_container.rend();
}

//-----------------------------------------------------------------------------

inline Composite::ConstReverseIteratorType Composite::rbegin() const
{
    return m_container.rbegin();
}

//-----------------------------------------------------------------------------

inline Composite::ConstReverseIteratorType Composite::rend() const
{
    return m_container.rend();
}

//-----------------------------------------------------------------------------

inline bool Composite::empty() const
{
    return m_container.empty();
}

//-----------------------------------------------------------------------------

inline Composite::SizeType Composite::size() const
{
    return m_container.size();
}

//-----------------------------------------------------------------------------

inline Composite::mapped_type& Composite::operator[](Composite::KeyType n)
{
    return this->m_container[n];
}

//-----------------------------------------------------------------------------

inline Composite::IteratorType Composite::find(const Composite::KeyType& x)
{
    return m_container.find(x);
}

//-----------------------------------------------------------------------------

inline Composite::ConstIteratorType Composite::find(const Composite::KeyType& x) const
{
    return m_container.find(x);
}

//-----------------------------------------------------------------------------

inline Composite::SizeType Composite::count(const Composite::KeyType& x) const
{
    return m_container.count(x);
}

//-----------------------------------------------------------------------------

inline Composite::ContainerType& Composite::getContainer()
{
    return m_container;
}

//-----------------------------------------------------------------------------

inline const Composite::ContainerType& Composite::getContainer () const
{
    return m_container;
}

//-----------------------------------------------------------------------------

inline void Composite::setContainer (const Composite::ContainerType& val)
{
    m_container = val;
}

//-----------------------------------------------------------------------------

template< class DATATYPE >
inline void Composite::setDataContainer( const std::map< std::string, SPTR(DATATYPE) >& map )
{
    this->getContainer().clear();
    this->getContainer().insert( map.begin(), map.end() );
}

//-----------------------------------------------------------------------------

template< class DATATYPE >
inline std::map< std::string, SPTR(DATATYPE) > Composite::getDataContainer() const
{
    std::map< std::string, SPTR(DATATYPE) > map;
    SPTR(DATATYPE) castData;
    for( ::fwData::Composite::value_type elem : *this )
    {
        castData = std::dynamic_pointer_cast<DATATYPE>( elem.second );
        OSLM_ASSERT("DynamicCast "<< ::fwCore::TypeDemangler<DATATYPE>().getClassname()<<" failed", castData);
        map[elem.first] = castData;
    }

    return map;
}

//-----------------------------------------------------------------------------

template< class DATATYPE >
SPTR(DATATYPE) Composite::at(const std::string& key)
{
    SPTR(DATATYPE) castData;
    ::fwData::Composite::iterator iter = this->find(key);
    if(iter != this->end())
    {
        castData = std::dynamic_pointer_cast<DATATYPE>(iter->second);
        SLM_TRACE_IF("DynamicCast "+ ::fwCore::TypeDemangler<DATATYPE>().getClassname()+" failed", !castData);
    }
    else
    {
        SLM_TRACE( "Object '" + key + "' not found.");
    }
    return castData;
}

//-----------------------------------------------------------------------------

template< class DATATYPE >
CSPTR(DATATYPE) Composite::at(const std::string& key) const
{
    CSPTR(DATATYPE) castData;
    ::fwData::Composite::const_iterator iter = this->find(key);
    if(iter != this->end())
    {
        castData = std::dynamic_pointer_cast<DATATYPE>(iter->second);
        SLM_TRACE_IF("DynamicCast "+ ::fwCore::TypeDemangler<DATATYPE>().getClassname()+" failed", !castData);
    }
    else
    {
        SLM_TRACE( "Object '" + key + "' not found.");
    }
    return castData;
}

//-----------------------------------------------------------------------------

} //namespace fwData
