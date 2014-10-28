/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_VALIDATOR_REGISTRY_MACRO_HPP__
#define __FWACTIVITIES_VALIDATOR_REGISTRY_MACRO_HPP__

#include <boost/preprocessor/cat.hpp>

#include "fwActivities/IValidator.hpp"
#include "fwActivities/validator/registry/detail.hpp"

namespace fwActivities
{
namespace validator
{
namespace registry
{

#define fwActivitiesValidatorRegisterMacro( ValidatorClassname, ValidatorFunctorKey )   \
    static ::fwActivities::IValidator::Registrar< ValidatorClassname >                \
    BOOST_PP_CAT( s__factory__record__, __LINE__) ( ValidatorFunctorKey );

} // end namespace registry
} // end namespace validator
} // end namespace fwActivities

#endif /*__FWACTIVITIES_VALIDATOR_REGISTRY_MACRO_HPP__*/
