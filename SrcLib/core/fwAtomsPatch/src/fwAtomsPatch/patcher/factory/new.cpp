/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtomsPatch/patcher/factory/new.hpp"

#include "fwAtomsPatch/patcher/IPatcher.hpp"

namespace fwAtomsPatch
{

namespace patcher
{

namespace factory
{

SPTR(::fwAtomsPatch::patcher::IPatcher) New( const ::fwAtomsPatch::patcher::registry::KeyType & classname )
{
    return ::fwAtomsPatch::patcher::registry::get()->create(classname);
}

} // namespace factory

} // namespace patcher

} // namespace fwAtomsPatch


