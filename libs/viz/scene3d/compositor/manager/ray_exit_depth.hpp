/************************************************************************
 *
 * Copyright (C) 2021-2025 IRCAD France
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

#include <OgreMaterialManager.h>

namespace sight::viz::scene3d::compositor::manager
{

//-----------------------------------------------------------------------------

/**
 * @brief Listener used in mixed surface/volume rendering.
 * Creates volume entry point computation techniques for meshes.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API ray_exit_depth final : public Ogre::MaterialManager::Listener
{
public:

    /// Constructor.
    SIGHT_VIZ_SCENE3D_API ray_exit_depth() = default;

    /// Destructor.
    SIGHT_VIZ_SCENE3D_API ~ray_exit_depth() final = default;

    /// Called when an entry point scheme isn't found, creates the missing technique.
    SIGHT_VIZ_SCENE3D_API Ogre::Technique* handleSchemeNotFound(
        std::uint16_t _scheme_index,
        const Ogre::String& _scheme_name,
        Ogre::Material* _original_material,
        std::uint16_t _lod_index,
        const Ogre::Renderable* _renderable
    ) final;
};

} // namespace sight::viz::scene3d::compositor::manager
