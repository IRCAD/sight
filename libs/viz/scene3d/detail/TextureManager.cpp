/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "viz/scene3d/detail/TextureManager.hpp"

#include "viz/scene3d/ogre.hpp"
#include "viz/scene3d/Utils.hpp"

#include <viz/scene3d/ogre.hpp>

namespace sight::viz::scene3d::detail
{

// ----------------------------------------------------------------------------

TextureLoader::return_t TextureLoader::load(const sight::data::Image& _image, Ogre::Texture* _texture)
{
    sight::viz::scene3d::Utils::loadGrayscaleImage(_texture, _image);

    return true;
}

// ----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::detail
