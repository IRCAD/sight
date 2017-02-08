/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_BUILDER_FACTORY_NEW_HPP__
#define __FWACTIVITIES_BUILDER_FACTORY_NEW_HPP__

#include "fwActivities/builder/registry/detail.hpp"
#include "fwActivities/config.hpp"

#include <string>

namespace fwActivities
{

class IBuilder;

namespace builder
{
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
friend SPTR( CLASSNAME ) fwActivities::builder::factory::New();

Key()
{
}
};

FWACTIVITIES_API SPTR( ::fwActivities::IBuilder ) New( const ::fwActivities::builder::registry::KeyType & classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) builder = std::make_shared< CLASSNAME >( Key() );
    return builder;
}

} // namespace factory
} // namespace builder
} // namespace fwActivities

#endif /* __FWACTIVITIES_BUILDER_FACTORY_NEW_HPP__ */

