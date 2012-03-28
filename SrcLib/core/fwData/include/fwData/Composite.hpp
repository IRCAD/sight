/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_COMPOSITE_HPP_
#define _FWDATA_COMPOSITE_HPP_

#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/version.hpp>

// boost 1.47 issue with FOREACH
#if ( defined(__clang__) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) ) && BOOST_VERSION == 104700
#include <boost/foreach.hpp>
#endif

#include "fwData/config.hpp"

#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"

namespace fwData
{

/**
 * @class   Composite
 * @brief   This class defines a composite object.
 *
 * Composite contains a map of ::fwData::Object.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */

class FWDATA_CLASS_API Composite : public Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Composite)(::fwData::Object), (()), ::fwData::Factory::New< Composite >) ;

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

    mapped_type& operator[] ( KeyType n )
    {return this->m_attrContainer[n];}

    IteratorType find ( const KeyType& x ) { return m_attrContainer.find(x); }
    ConstIteratorType find ( const KeyType& x ) const { return m_attrContainer.find(x); }

    SizeType count ( const KeyType& x ) const { return m_attrContainer.count(x); }
    /// @}



    /// @brief get/set the map of std::string/::fwData::Object
    /// @{
    ContainerType &getContainer(){ return m_attrContainer; };
    fwDataGetSetCRefMacro(Container, ContainerType);
    /// @}

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Composite::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Composite::csptr _source );

    /// Method to initialize a ::fwData::Composite from a std::map< string, X >
    template< class DATATYPE >
    void setDataContainer( const std::map< std::string, SPTR(DATATYPE) > & map )
    {
        this->getContainer().clear();
        this->getContainer().insert( map.begin(), map.end() );
    }

    /// Method to get a std::map< string, X > from ::fwData::Composite
    template< class DATATYPE >
    std::map< std::string, SPTR(DATATYPE) > getDataContainer() const
    {
        std::map< std::string, SPTR(DATATYPE) > map;
        SPTR(DATATYPE) castedData;
        BOOST_FOREACH( ::fwData::Composite::value_type elem, *this )
        {
            castedData = ::boost::dynamic_pointer_cast<DATATYPE>( elem.second );
            OSLM_ASSERT("DynamicCast "<< ::fwCore::TypeDemangler<DATATYPE>().getFullClassname()<<" failed", castedData);
            map[elem.first] = castedData;
        }

        return map;
    }

protected:
    /// Constructor
    FWDATA_API Composite();

    /// Destructor
    FWDATA_API virtual ~Composite();

    ContainerType m_attrContainer;
};


} //namespace fwData


// boost 1.47 issue with FOREACH
#if ( defined(__clang__) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) ) && BOOST_VERSION == 104700
namespace boost { namespace foreach
{
    template<>
    struct is_noncopyable<
        ::fwData::Composite
    > : mpl::true_
    {};
}}
#endif


#endif /* _FWDATA_COMPOSITE_HPP_ */

