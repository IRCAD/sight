/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreTechnique.h>

namespace sight::viz::scene3d::helper
{

class technique
{
public:

    /**
     * @brief Copies a technique to an existing material
     *
     * @param[in] _tech technique to copy
     * @param[in] _scheme_name new technique's scheme
     * @param[inout] _mtl material to which the technique is added
     * @return the newly created technique
     */
    static SIGHT_VIZ_SCENE3D_API Ogre::Technique* copy_to_material(
        const Ogre::Technique* _tech,
        const std::string& _scheme_name,
        Ogre::Material* _mtl
    );
};

} // namespace sight::viz::scene3d::helper
