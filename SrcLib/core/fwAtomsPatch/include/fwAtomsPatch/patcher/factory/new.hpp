/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_PATCHER_FACTORY_NEW_HPP__
#define __FWATOMSPATCH_PATCHER_FACTORY_NEW_HPP__

#include "fwAtomsPatch/config.hpp"
#include "fwAtomsPatch/patcher/registry/detail.hpp"

#include <string>

namespace fwAtomsPatch
{

namespace patcher
{

class IPatcher;

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
friend SPTR( CLASSNAME ) fwAtomsPatch::patcher::factory::New();

Key()
{
}
};

FWATOMSPATCH_API SPTR(::fwAtomsPatch::patcher::IPatcher) New(
    const ::fwAtomsPatch::patcher::registry::KeyType & classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) obj = std::make_shared< CLASSNAME >( Key() );

    return obj;
}

} // namespace factory

} // namespace patcher

} // namespace fwAtomsPatch

#endif /* __FWATOMSPATCH_PATCHER_FACTORY_NEW_HPP__ */

