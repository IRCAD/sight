/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMemory/IPolicy.hpp"
#include "fwMemory/policy/factory/new.hpp"
#include "fwMemory/policy/registry/detail.hpp"


namespace fwMemory
{
namespace policy
{
namespace factory
{

::fwMemory::IPolicy::sptr New( const ::fwMemory::policy::registry::KeyType & classname )
{
    return ::fwMemory::policy::registry::get()->create(classname);
}

} // namespace factory
} // namespace policy
} // namespace fwMemory


