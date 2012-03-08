/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_DOWNCASTITERATOR_HPP_
#define _FWDATA_DOWNCASTITERATOR_HPP_

#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"

namespace fwData
{

/**
 * @brief the prupose of this function is to provide an UnaryFunction which is template whish is enable to downcast a base class to a subClass via shared_ptr
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
// BOOST => The type UnaryFunction must be Assignable, Copy Constructible, and the expression f(*i) must be valid where f is an object of
// type UnaryFunction, i is an object of type Iterator, and where the type of f(*i) must be
// result_of<UnaryFunction(iterator_traits<Iterator>::reference)>::type.
template < class SubClass>
struct DownCaster : public std::unary_function< ::fwTools::Object::sptr,  typename SubClass::sptr  >
{
    typename SubClass::sptr operator()( ::fwTools::Object::sptr  obj ) const // const is mandatory
    {
        return boost::dynamic_pointer_cast< SubClass >( obj );
    }
};

/**
 * @brief the prupose of this function is to provide an UnaryFunction which is template whish is enable to downcast a base class to a subClass via shared_ptr in a const way !
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
template < class SubClass>
struct DownConstCaster : public std::unary_function< ::fwTools::Object::sptr,  boost::shared_ptr< const SubClass>  >
{
    boost::shared_ptr< const SubClass> operator()( ::fwTools::Object::sptr  obj ) const // VAG const is mandatory
    {
        return boost::dynamic_pointer_cast< const SubClass >( obj );;
    }
};

template< class SubClass >
struct ContainerCaster
{
    typedef typename boost::transform_iterator< DownCaster<SubClass> ,      ::fwTools::Object::ChildContainer::iterator > iterator;
    typedef typename boost::transform_iterator< DownConstCaster<SubClass> , ::fwTools::Object::ChildContainer::const_iterator > const_iterator;

};

} // end namespace fwData

#endif // _FWDATA_DOWNCASTITERATOR_HPP_
