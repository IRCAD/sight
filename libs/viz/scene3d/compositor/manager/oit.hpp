/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "viz/scene3d/compositor/types.hpp"

#include <OgreMaterialManager.h>

namespace sight::viz::scene3d::compositor::manager
{

/**
 * @brief This compositor manager for Order Independent Transparency fulfills two roles:
 * - it generates material techniques at runtime (according to material schemes)
 * - it generates compositor passes dynamically according to user settings (number of peel passes for instance)
 */
class SIGHT_VIZ_SCENE3D_CLASS_API oit : public Ogre::MaterialManager::Listener
{
public:

    /// Destructor
    ~oit() override = default;

    /// Callback called each time a scheme is not found
    Ogre::Technique* handleSchemeNotFound(
        std::uint16_t _scheme_index,
        const Ogre::String& _scheme_name,
        Ogre::Material* _original_material,
        std::uint16_t _lod_index,
        const Ogre::Renderable* _renderable
    ) override;

    /// Setup OIT current compositor
    static SIGHT_VIZ_SCENE3D_API std::string setup_transparency(
        Ogre::Viewport* _viewport,
        compositor::transparency_technique _technique,
        int _depth = 0
    );

private:

    /// Set number of ping pong peels for Depth Peeling compositor
    static void setup_depth_peeling(
        Ogre::CompositionTechnique* _compositor_tech,
        int _depth,
        const std::string& _core_compositor_name
    );

    /// Set number of ping pong peels for Dual Depth Peeling compositor
    static void setup_dual_depth_peeling(Ogre::CompositionTechnique* _compositor_tech, int _depth);

    /// Set number of Depth Peeling ping pong peels for Hybrid Transparency compositor
    /// - other peels computed with Weighted Blended OIT
    static void setup_hybrid_transparency(Ogre::CompositionTechnique* _compositor_tech, int _depth);

    /**
     * @brief Ensure that a given fragment program is created.
     *
     * @param[in] _name name of the program
     * @param[in] _algo_name name of the algorithm
     * @param[in] _algo_pass_name new suffix to use
     * @param[in] _base_name new suffix to use
     */
    static Ogre::GpuProgramPtr ensure_fp_created(
        const std::string& _name,
        const std::string& _algo_name,
        const std::string& _algo_pass_name,
        const std::string& _base_name,
        const std::string& _source_name
    );
};

} // namespace sight::viz::scene3d::compositor::manager
