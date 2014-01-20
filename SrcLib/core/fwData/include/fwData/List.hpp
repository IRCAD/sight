/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_LIST_HPP_
#define _FWDATA_LIST_HPP_

#include <list>
#include <boost/shared_ptr.hpp>

#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/config.hpp"

fwCampAutoDeclareDataMacro((fwData)(List), FWDATA_API);

namespace fwData
{



/**
 * @class   List
 * @brief   This class defines a list of objects.
 *
 * List contains a list of ::fwData::Object.
 *
 * 
 * @date    2007-2009.
 */

class FWDATA_CLASS_API List : public Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (List)(::fwData::Object), (()), ::fwData::factory::New< List >) ;


    fwCampMakeFriendDataMacro((fwData)(List));

    typedef std::list< Object::sptr > ContainerType;

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
    FWDATA_API List(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~List();

    /// boost_foreach/stl compatibility
    /// @{
    typedef ContainerType::value_type value_type;
    typedef ContainerType::iterator iterator;
    typedef ContainerType::const_iterator const_iterator;
    typedef ContainerType::reverse_iterator reverse_iterator;
    typedef ContainerType::const_reverse_iterator const_reverse_iterator;
    typedef ContainerType::size_type size_type;

    typedef List Container;

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

    ReferenceType front(){ return m_attrContainer.front(); }
    ReferenceType back(){ return m_attrContainer.back(); }
    ConstReferenceType front() const{ return m_attrContainer.front(); }
    ConstReferenceType back() const{ return m_attrContainer.back(); }
    /// @}

    /// @brief get/set the list of ::fwData::Object
    /// @{
    ContainerType &getContainer(){ return m_attrContainer; };
    fwDataGetSetCRefMacro(Container, ContainerType);
    /// @}


    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

protected:

    ContainerType m_attrContainer;
};
}

#endif /* _FWDATA_LIST_HPP_ */

