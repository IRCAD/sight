/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#define CAMP_COMPILATION

#include "data/detail/reflection/TransferFunction.hpp"

#include "data/reflection/mapper.hpp"

#include <core/reflection/UserObject.hpp>

SIGHT_IMPLEMENT_DATA_REFLECTION((sight) (data) (TransferFunction))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "sight::data")
    .base<sight::data::Object>()
    .property("level", &sight::data::TransferFunction::m_level)
    .property("window", &sight::data::TransferFunction::m_window)
    .property("name", &sight::data::TransferFunction::m_name)
    .property("background_color", &sight::data::TransferFunction::m_backgroundColor)
    .property("tf_data", &sight::data::TransferFunction::m_tfData)
    .property("interpolation_mode", &sight::data::TransferFunction::m_interpolationMode)
    .property("is_clamped", &sight::data::TransferFunction::m_isClamped)
    ;
}

SIGHT_IMPLEMENT_ENUM_REFLECTION((sight) (data) (TransferFunction) (InterpolationMode))
{
    builder
    .value("LINEAR", sight::data::TransferFunction::LINEAR)
    .value("NEAREST", sight::data::TransferFunction::NEAREST)
    ;
}
