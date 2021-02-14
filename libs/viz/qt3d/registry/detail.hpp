/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "viz/qt3d/config.hpp"

#include <core/macros.hpp>
#include <core/util/FactoryRegistry.hpp>

#include <string>

namespace sight::viz::qt3d
{

class IWindowInteractor;

namespace registry
{

typedef std::string KeyType;

typedef sight::core::util::FactoryRegistry< SPTR(sight::viz::qt3d::IWindowInteractor) (), KeyType > Type;

VIZ_QT3D_API SPTR(Type) get();

} // namespace registry.

} // namespace sight::viz::qt3d.
