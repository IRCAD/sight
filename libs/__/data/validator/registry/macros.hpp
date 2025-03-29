/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "data/validator/base.hpp"
#include "data/validator/registry/detail.hpp"

#include <boost/preprocessor/cat.hpp>

namespace sight::data::validator::registry
{

#define SIGHT_REGISTER_DATA_VALIDATOR(ValidatorClassname) \
        static sight::data::validator::base::registry<ValidatorClassname> \
        BOOST_PP_CAT(s__factory__record__, __LINE__)(BOOST_PP_STRINGIZE(ValidatorClassname));

} // namespace sight::data::validator::registry
