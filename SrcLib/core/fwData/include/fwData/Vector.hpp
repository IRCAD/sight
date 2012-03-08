/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_VECTOR_HPP_
#define _FWDATA_VECTOR_HPP_

#include <map>
#include <boost/shared_ptr.hpp>

#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"
#include "fwData/config.hpp"

namespace fwData
{

typedef std::vector< Object::sptr > ObjectVectorType;

/**
 * @class   Vector
 * @brief   This class defines a vector of objects.
 *
 * Vector contains a vector of ::fwData::Object.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */

class FWDATA_CLASS_API Vector : public Object, private ObjectVectorType
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Vector)(::fwData::Object), (()), ::fwData::Factory::New< Vector >) ;

    typedef Vector Container;
    typedef ObjectVectorType ContainerType;


    using ObjectVectorType::reference;
    using ObjectVectorType::const_reference;
    using ObjectVectorType::iterator;
    using ObjectVectorType::const_iterator;
    using ObjectVectorType::size_type;
    using ObjectVectorType::difference_type;
    using ObjectVectorType::value_type;
    using ObjectVectorType::allocator_type;
    using ObjectVectorType::pointer;
    using ObjectVectorType::const_pointer;
    using ObjectVectorType::reverse_iterator;
    using ObjectVectorType::const_reverse_iterator;

    using ObjectVectorType::begin;
    using ObjectVectorType::end;
    using ObjectVectorType::rbegin;
    using ObjectVectorType::rend;

    using ObjectVectorType::size;
    using ObjectVectorType::max_size;
    using ObjectVectorType::resize;
    using ObjectVectorType::capacity;
    using ObjectVectorType::empty;
    using ObjectVectorType::reserve;

    using ObjectVectorType::operator[];
    using ObjectVectorType::at;
    using ObjectVectorType::front;
    using ObjectVectorType::back;

    using ObjectVectorType::assign;
    using ObjectVectorType::push_back;
    using ObjectVectorType::pop_back;
    using ObjectVectorType::insert;
    using ObjectVectorType::erase;
    using ObjectVectorType::swap;
    using ObjectVectorType::clear;

    using ObjectVectorType::get_allocator;

    /// @brief get the container of ::fwData::Object
    FWDATA_API Vector &getRefContainer();

    /// @brief get the container of ::fwData::Object
    FWDATA_API Vector const &getRefContainer() const;

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Vector::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Vector::csptr _source );

protected:
    /// Constructor
    FWDATA_API Vector();

    /// Destructor
    FWDATA_API virtual ~Vector();

};
}

#endif /* _FWDATA_VECTOR_HPP_ */

