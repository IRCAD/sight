/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include <sight/viz/scene3d/config.hpp>

#include <data/image.hpp>

#include <viz/scene3d/detail/resource_manager.hpp>

namespace sight::viz::scene3d::detail
{

//---------------------------------------------------------------------

class texture_loader
{
public:

    using return_t = Ogre::Vector2;
    static return_t load(const sight::data::image&, Ogre::Texture*);
};

//---------------------------------------------------------------------

/**
 * @brief  Implementation of texture resource manager.
 *
 * Currently, it is only used for grayscale images.
 */
using texture_manager = resource_manager<sight::data::image, Ogre::Texture, texture_loader>;

} // namespace sight::viz::scene3d::detail
