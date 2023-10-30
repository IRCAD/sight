/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "viz/scene3d/config.hpp"

#include <data/transfer_function.hpp>

#include <viz/scene3d/detail/resource_manager.hpp>

namespace sight::viz::scene3d::detail
{

//---------------------------------------------------------------------

class tf_loader
{
public:

    using return_t = Ogre::Vector3;
    static return_t load(const sight::data::transfer_function&, Ogre::Texture*);
};

//---------------------------------------------------------------------

using tf_manager = resource_manager<sight::data::transfer_function, Ogre::Texture, tf_loader>;

//---------------------------------------------------------------------

} // namespace sight::viz::scene3d::detail
