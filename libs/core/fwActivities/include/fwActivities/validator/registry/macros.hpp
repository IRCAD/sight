/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWACTIVITIES_VALIDATOR_REGISTRY_MACROS_HPP__
#define __FWACTIVITIES_VALIDATOR_REGISTRY_MACROS_HPP__

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

#endif /*__FWACTIVITIES_VALIDATOR_REGISTRY_MACROS_HPP__*/
