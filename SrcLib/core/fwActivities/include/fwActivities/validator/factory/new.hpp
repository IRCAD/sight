/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_VALIDATOR_FACTORY_NEW_HPP__
#define __FWACTIVITIES_VALIDATOR_FACTORY_NEW_HPP__

#include "fwActivities/config.hpp"
#include "fwActivities/validator/registry/detail.hpp"

#include <string>

namespace fwActivities
{

class IValidator;

namespace validator
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
friend SPTR( CLASSNAME ) fwActivities::validator::factory::New();

Key()
{
}
};

FWACTIVITIES_API SPTR( ::fwActivities::IValidator ) New( const ::fwActivities::validator::registry::KeyType &
                                                         classname );

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) validator = std::make_shared< CLASSNAME >( Key() );
    return validator;
}

} // namespace factory
} // namespace validator
} // namespace fwActivities

#endif /* __FWACTIVITIES_VALIDATOR_FACTORY_NEW_HPP__ */

