/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_COMPOSITE_HPP_
#define _FWDATA_COMPOSITE_HPP_

#include <map>
#include <boost/shared_ptr.hpp>
#include "fwData/config.hpp"

#include <fwData/Object.hpp>

namespace fwData
{

typedef std::map< std::string, Object::sptr > ObjectMapType;

/**
 * @class   Composite
 * @brief   This class defines a composite object.
 *
 * Composite contains a map of ::fwData::Object.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */

class FWDATA_CLASS_API Composite : public Object, private ObjectMapType
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Composite)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< Composite >) ;

    typedef Composite Container;
    typedef ObjectMapType ContainerType;

    using ObjectMapType::key_type;
    using ObjectMapType::mapped_type;
    using ObjectMapType::value_type;
    using ObjectMapType::key_compare;
    using ObjectMapType::value_compare;
    using ObjectMapType::allocator_type;
    using ObjectMapType::reference;
    using ObjectMapType::const_reference;
    using ObjectMapType::iterator;
    using ObjectMapType::const_iterator;
    using ObjectMapType::size_type;
    using ObjectMapType::difference_type;
    using ObjectMapType::pointer;
    using ObjectMapType::const_pointer;
    using ObjectMapType::reverse_iterator;
    using ObjectMapType::const_reverse_iterator;


    using ObjectMapType::begin;
    using ObjectMapType::end;
    using ObjectMapType::rbegin;
    using ObjectMapType::rend;

    using ObjectMapType::empty;
    using ObjectMapType::size;
    using ObjectMapType::max_size;

    using ObjectMapType::operator[];

    using ObjectMapType::insert;
    using ObjectMapType::erase;
    using ObjectMapType::swap;
    using ObjectMapType::clear;

    using ObjectMapType::key_comp;
    using ObjectMapType::value_comp;

    using ObjectMapType::find;
    using ObjectMapType::count;
    using ObjectMapType::lower_bound;
    using ObjectMapType::upper_bound;
    using ObjectMapType::equal_range;

    using ObjectMapType::get_allocator;

    /// Constructor
    FWDATA_API Composite();

    /// Destructor
    FWDATA_API virtual ~Composite();

    /// @brief get the container of ::fwData::Object
    FWDATA_API Composite &getRefMap();
    /// @brief get the container of ::fwData::Object
    FWDATA_API Composite const &getRefMap() const;

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Composite::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Composite::csptr _source );

    //FWDATA_API Composite & operator=( const Composite & _composite );

};


} //namespace fwData

#endif /* _FWDATA_COMPOSITE_HPP_ */

