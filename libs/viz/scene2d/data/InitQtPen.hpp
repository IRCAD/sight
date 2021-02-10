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

#include "viz/scene2d/config.hpp"

#include <QPen>

namespace sight::viz::scene2d
{
namespace data
{

class VIZ_SCENE2D_CLASS_API InitQtPen
{

public:

    /// Set a pen a color.
    VIZ_SCENE2D_API static void setPenColor(QPen& _pen, std::string _color);

    /// Set a pen a color with opacity.
    VIZ_SCENE2D_API static void setPenColor(QPen& _pen, std::string _color, float _opacity);
};

} // namespace data
} // namespace sight::viz::scene2d
