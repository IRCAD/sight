/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwActivities/IValidator.hpp"
#include "fwActivities/validator/factory/new.hpp"


namespace fwActivities
{
namespace validator
{
namespace factory
{

::fwActivities::IValidator::sptr New( const ::fwActivities::validator::registry::KeyType & classname )
{
    return ::fwActivities::validator::registry::get()->create(classname);
}

} // namespace factory
} // namespace validator
} // namespace fwActivities


