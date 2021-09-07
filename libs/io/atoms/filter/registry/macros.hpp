/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/atoms/filter/registry/detail.hpp"

#include <boost/preprocessor/cat.hpp>

namespace sight::io::atoms::filter
{

namespace registry
{

#define  fwAtomsFilterRegisterMacro(classname, objectKey) \
    static sight::io::atoms::filter::IFilter::Registry<classname> \
    BOOST_PP_CAT(s__factory__record__object__, __LINE__)(objectKey);

} // namespace registry

} // namespace sight::io::atoms::filter
