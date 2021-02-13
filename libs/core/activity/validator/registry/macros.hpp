/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "activity/IValidator.hpp"
#include "activity/validator/registry/detail.hpp"

#include <boost/preprocessor/cat.hpp>

namespace sight::activity
{
namespace validator
{
namespace registry
{

#define fwActivitiesValidatorRegisterMacro( ValidatorClassname, ValidatorFunctorKey )   \
    static activity::IValidator::Registrar< ValidatorClassname >                \
    BOOST_PP_CAT( s__factory__record__, __LINE__) ( ValidatorFunctorKey );

} // end namespace registry
} // end namespace validator
} // end namespace sight::activity
