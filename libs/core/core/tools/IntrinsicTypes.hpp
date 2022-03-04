/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "core/tools/IntegerTypes.hpp"

#include <boost/mpl/push_back.hpp>

namespace sight::core::tools
{

#ifdef DEBUG
typedef boost::mpl::push_back<IntegerTypes, float>::type IntrinsicTypes;
#else
typedef boost::mpl::push_back<boost::mpl::push_back<IntegerTypes, float>::type, double>::type IntrinsicTypes;
#endif

} // end namespace sight::core::tools
