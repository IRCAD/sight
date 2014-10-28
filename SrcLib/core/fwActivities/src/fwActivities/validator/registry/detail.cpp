/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/util/LazyInstantiator.hpp>

#include "fwActivities/validator/registry/detail.hpp"


namespace fwActivities
{
namespace validator
{
namespace registry
{

struct FwActivitiesValidatorRegistryInstantiatorTag {} ;

SPTR(Type) get()
{
    typedef ::fwCore::util::LazyInstantiator< Type, FwActivitiesValidatorRegistryInstantiatorTag > InstantiatorType;
    return InstantiatorType::getInstance();
}

} // namespace registry
} // namespace validator
} // namespace fwActivities


