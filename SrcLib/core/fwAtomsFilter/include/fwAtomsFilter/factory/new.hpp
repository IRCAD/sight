/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSFILTER_FACTORY_NEW_HPP__
#define __FWATOMSFILTER_FACTORY_NEW_HPP__

#include "fwAtomsFilter/config.hpp"
#include "fwAtomsFilter/registry/detail.hpp"

#include <string>

namespace fwAtomsFilter
{

class IFilter;

namespace factory
{

template<class CLASSNAME > SPTR( CLASSNAME )  New();

/**
 * @brief Key class used to restrict access to Object construction.
 * See http://www.drdobbs.com/184402053
 */
class Key
{
template<typename CLASSNAME>
friend SPTR( CLASSNAME ) fwAtomsFilter::factory::New();

Key()
{
}
};

FWATOMSFILTER_API SPTR(::fwAtomsFilter::IFilter) New(
    const ::fwAtomsFilter::registry::KeyType & classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) obj = std::make_shared< CLASSNAME >( Key() );

    return obj;
}

} // namespace factory

} // namespace fwAtomsFilter

#endif /* __FWATOMSFILTER_FACTORY_NEW_HPP__ */

