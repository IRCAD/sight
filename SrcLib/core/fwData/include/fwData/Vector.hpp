/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_VECTOR_HPP__
#define __FWDATA_VECTOR_HPP__

#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <vector>

namespace fwData
{
class Vector;
}

fwCampAutoDeclareDataMacro((fwData)(Vector), FWDATA_API);

namespace fwData
{

/**
 * @brief This class defines a vector of objects.
 *
 * Vector contains a vector of ::fwData::Object.
 */
class FWDATA_CLASS_API Vector : public Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Vector)(::fwData::Object), (()), ::fwData::factory::New< Vector >);
    fwCampMakeFriendDataMacro((fwData)(Vector));

    typedef std::vector< Object::sptr > ContainerType;

    typedef ContainerType::value_type ValueType;
    typedef ContainerType::reference ReferenceType;
    typedef ContainerType::const_reference ConstReferenceType;
    typedef ContainerType::iterator IteratorType;
    typedef ContainerType::const_iterator ConstIteratorType;
    typedef ContainerType::reverse_iterator ReverseIteratorType;
    typedef ContainerType::const_reverse_iterator ConstReverseIteratorType;
    typedef ContainerType::size_type SizeType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Vector(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Vector();

    /// boost_foreach/stl compatibility
    /// @{
    typedef ContainerType::value_type value_type;
    typedef ContainerType::iterator iterator;
    typedef ContainerType::const_iterator const_iterator;
    typedef ContainerType::reverse_iterator reverse_iterator;
    typedef ContainerType::const_reverse_iterator const_reverse_iterator;
    typedef ContainerType::size_type size_type;

    typedef Vector Container;

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

    ValueType front() const;
    ValueType back() const;

    ReferenceType operator[] ( size_type n );
    ConstReferenceType operator[] ( size_type n ) const;

    ReferenceType at ( SizeType n );
    ConstReferenceType at ( SizeType n ) const;
    /// @}

    /// @brief get/set the vector of ::fwData::Object
    /// @{
    ContainerType &getContainer();
    const ContainerType &getContainer () const;
    void setContainer (const ContainerType &val);
    /// @}

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    /// Method to initialize a ::fwData::Vector from a std::vector
    template< class DATATYPE >
    void setDataContainer( const std::vector< SPTR(DATATYPE) > & vec );

    /// Method to get a std::vector from ::fwData::Vector
    template< class DATATYPE >
    std::vector< SPTR(DATATYPE) > getDataContainer() const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when objects are added
    typedef ::fwCom::Signal< void (ContainerType) > AddedObjectsSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_ADDED_OBJECTS_SIG;

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

inline Vector::IteratorType Vector::begin()
{
    return m_container.begin();
}

//-----------------------------------------------------------------------------

inline Vector::IteratorType Vector::end()
{
    return m_container.end();
}

//-----------------------------------------------------------------------------

inline Vector::ConstIteratorType Vector::begin() const
{
    return m_container.begin();
}

//-----------------------------------------------------------------------------

inline Vector::ConstIteratorType Vector::end() const
{
    return m_container.end();
}

//-----------------------------------------------------------------------------

inline Vector::ReverseIteratorType Vector::rbegin()
{
    return m_container.rbegin();
}

//-----------------------------------------------------------------------------

inline Vector::ReverseIteratorType Vector::rend()
{
    return m_container.rend();
}

//-----------------------------------------------------------------------------

inline Vector::ConstReverseIteratorType Vector::rbegin() const
{
    return m_container.rbegin();
}

//-----------------------------------------------------------------------------

inline Vector::ConstReverseIteratorType Vector::rend() const
{
    return m_container.rend();
}

//-----------------------------------------------------------------------------

inline bool Vector::empty() const
{
    return m_container.empty();
}

//-----------------------------------------------------------------------------

inline Vector::SizeType Vector::size() const
{
    return m_container.size();
}


//-----------------------------------------------------------------------------

inline Vector::ValueType Vector::front() const
{
    return m_container.front();
}

//-----------------------------------------------------------------------------

inline Vector::ValueType Vector::back() const
{
    return m_container.back();
}

//-----------------------------------------------------------------------------

inline Vector::ReferenceType Vector::operator[](Vector::size_type n)
{
    return this->m_container[n];
}

//-----------------------------------------------------------------------------

inline Vector::ConstReferenceType Vector::operator[](Vector::size_type n) const
{
    return this->m_container[n];
}

//-----------------------------------------------------------------------------

inline Vector::ReferenceType Vector::at(Vector::SizeType n)
{
    return m_container.at(n);
}

//-----------------------------------------------------------------------------

inline Vector::ConstReferenceType Vector::at(Vector::SizeType n) const
{
    return m_container.at(n);
}

//-----------------------------------------------------------------------------

inline Vector::ContainerType &Vector::getContainer()
{
    return m_container;
}

//-----------------------------------------------------------------------------

inline const Vector::ContainerType &Vector::getContainer () const
{
    return m_container;
}

//-----------------------------------------------------------------------------

inline void Vector::setContainer (const Vector::ContainerType &val)
{
    m_container = val;
}

//-----------------------------------------------------------------------------

template< class DATATYPE >
inline void Vector::setDataContainer( const std::vector< SPTR(DATATYPE) > & vec )
{
    m_container.clear();
    std::copy( vec.begin(), vec.end(), std::back_inserter(this->getContainer()) );
}

//-----------------------------------------------------------------------------

template< class DATATYPE >
inline std::vector< SPTR(DATATYPE) > Vector::getDataContainer() const
{
    std::vector< SPTR(DATATYPE) > vec;
    vec.reserve( this->size() );
    SPTR(DATATYPE) castedData;
    for(const ::fwData::Object::sptr& data : this->getContainer() )
    {
        castedData = std::dynamic_pointer_cast<DATATYPE>( data );
        OSLM_ASSERT("DynamicCast "<< ::fwCore::TypeDemangler<DATATYPE>().getFullClassname()<<" failed", castedData);
        vec.push_back( castedData );
    }

    return vec;
}

//-----------------------------------------------------------------------------

} //namespace fwData

#endif /* __FWDATA_VECTOR_HPP__ */
