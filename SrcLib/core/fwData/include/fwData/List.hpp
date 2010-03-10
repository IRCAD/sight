/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_LIST_HPP_
#define _FWDATA_LIST_HPP_

#include <map>
#include <boost/shared_ptr.hpp>
#include "fwData/config.hpp"

#include <fwData/Object.hpp>

namespace fwData
{

typedef std::list< Object::sptr > ObjectListType;

/**
 * @class   List
 * @brief   This class defines a vector of objects.
 *
 * List contains a map of ::fwData::Object.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */

class FWDATA_CLASS_API List : public Object, private ObjectListType
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (List)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< List >) ;

    typedef List Container;
    typedef ObjectListType ContainerType;


    using ObjectListType::reference;
    using ObjectListType::const_reference;
    using ObjectListType::iterator;
    using ObjectListType::const_iterator;
    using ObjectListType::size_type;
    using ObjectListType::difference_type;
    using ObjectListType::value_type;
    using ObjectListType::allocator_type;
    using ObjectListType::pointer;
    using ObjectListType::const_pointer;
    using ObjectListType::reverse_iterator;
    using ObjectListType::const_reverse_iterator;

    using ObjectListType::begin;
    using ObjectListType::end;
    using ObjectListType::rbegin;
    using ObjectListType::rend;

    using ObjectListType::empty;
    using ObjectListType::size;
    using ObjectListType::max_size;
    using ObjectListType::resize;

    using ObjectListType::front;
    using ObjectListType::back;

    using ObjectListType::assign;
    using ObjectListType::push_front;
    using ObjectListType::pop_front;
    using ObjectListType::push_back;
    using ObjectListType::pop_back;
    using ObjectListType::insert;
    using ObjectListType::erase;
    using ObjectListType::swap;
    using ObjectListType::clear;

    using ObjectListType::splice;
    using ObjectListType::remove;
    using ObjectListType::remove_if;
    using ObjectListType::unique;
    using ObjectListType::merge;
    using ObjectListType::sort;
    using ObjectListType::reverse;

    using ObjectListType::get_allocator;



    /// Constructor
    FWDATA_API List();

    /// Destructor
    FWDATA_API virtual ~List();

    /// @brief get the container of ::fwData::Object
    FWDATA_API List &getRefContainer();

    /// @brief get the container of ::fwData::Object
    FWDATA_API List const &getRefContainer() const;

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( List::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( List::csptr _source );

};
}

#endif /* _FWDATA_LIST_HPP_ */

