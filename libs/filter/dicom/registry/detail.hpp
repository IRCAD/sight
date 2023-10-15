/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "filter/dicom/config.hpp"

#include <core/factory_registry.hpp>
#include <core/macros.hpp>

#include <string>

namespace sight::filter::dicom
{

class filter;

namespace registry
{

typedef std::string key_t;

typedef core::factory_registry<SPTR(sight::filter::dicom::filter)(), key_t> Type;

FILTER_DICOM_API SPTR(Type) get();

} // namespace registry

} // namespace sight::filter::dicom
