/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_VECTOR_HPP_
#define _FWDATA_VECTOR_HPP_

#include <vector>
#include <boost/shared_ptr.hpp>

#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"
#include "fwData/config.hpp"

namespace fwData
{

/**
 * @class   Vector
 * @brief   This class defines a vector of objects.
 *
 * Vector contains a vector of ::fwData::Object.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */

class FWDATA_CLASS_API Vector : public Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Vector)(::fwData::Object), (()), ::fwData::Factory::New< Vector >) ;



    typedef std::vector< Object::sptr > ContainerType;

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

    typedef Vector Container;

    IteratorType begin() { return m_attrContainer.begin(); }
    IteratorType end()   { return m_attrContainer.end(); }
    ConstIteratorType begin() const { return m_attrContainer.begin(); }
    ConstIteratorType end()   const { return m_attrContainer.end(); }

    ReverseIteratorType rbegin() { return m_attrContainer.rbegin(); }
    ReverseIteratorType rend()   { return m_attrContainer.rend(); }
    ConstReverseIteratorType rbegin() const { return m_attrContainer.rbegin(); }
    ConstReverseIteratorType rend()   const { return m_attrContainer.rend(); }

    bool empty() const { return m_attrContainer.empty(); }
    SizeType size() const { return m_attrContainer.size(); }

    ValueType front(){ return m_attrContainer.front(); }
    ValueType back(){ return m_attrContainer.back(); }

    ReferenceType operator[] ( size_type n )
    {return this->m_attrContainer[n];}
    ConstReferenceType operator[] ( size_type n ) const
    {return this->m_attrContainer[n];}

    ReferenceType at ( SizeType n ) {return m_attrContainer.at(n);}
    ConstReferenceType at ( SizeType n ) const {return m_attrContainer.at(n);}
    /// @}

    /// @brief get/set the vector of ::fwData::Object
    /// @{
    ContainerType &getContainer(){ return m_attrContainer; };
    fwDataGetSetCRefMacro(Container, ContainerType);
    /// @}


    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Vector::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Vector::csptr _source );

    /// Method to initialize a ::fwData::Vector from a std::vector
    template< class DATATYPE >
    void setDataContainer( const std::vector< SPTR(DATATYPE) > & vec )
    {
        this->m_attrContainer.clear();
        std::copy( vec.begin(), vec.end(), std::back_inserter(this->getContainer()) );
    }

    /// Method to get a std::vector from ::fwData::Vector
    template< class DATATYPE >
    std::vector< SPTR(DATATYPE) > getDataContainer() const
    {
        std::vector< SPTR(DATATYPE) > vec;
        vec.reserve( this->size() );
        SPTR(DATATYPE) castedData;
        BOOST_FOREACH( ::fwData::Object::sptr data, this->getContainer() )
        {
            castedData = ::boost::dynamic_pointer_cast<DATATYPE>( data );
            OSLM_ASSERT("DynamicCast "<< ::fwCore::TypeDemangler<DATATYPE>().getFullClassname()<<" failed", castedData);
            vec.push_back( castedData );
        }

        return vec;
    }

protected:
    /// Constructor
    FWDATA_API Vector();

    /// Destructor
    FWDATA_API virtual ~Vector();

    ContainerType m_attrContainer;
};
} //namespace fwData

#endif /* _FWDATA_VECTOR_HPP_ */

