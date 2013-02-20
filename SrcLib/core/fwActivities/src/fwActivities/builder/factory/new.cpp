/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwActivities/IBuilder.hpp"
#include "fwActivities/builder/factory/new.hpp"


namespace fwActivities
{
namespace builder
{
namespace factory
{

::fwActivities::IBuilder::sptr New( const ::fwActivities::builder::registry::KeyType & classname )
{
    return ::fwActivities::builder::registry::get()->create(classname);
}

} // namespace factory
} // namespace builder
} // namespace fwActivities


