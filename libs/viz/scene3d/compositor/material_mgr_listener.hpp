/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include <OgreMaterialManager.h>

namespace sight::viz::scene3d::compositor
{

/**
 * @brief Generates transparency techniques at runtime.
 */
class VIZ_SCENE3D_CLASS_API material_mgr_listener : public Ogre::MaterialManager::Listener
{
public:

    /// Destructor
    ~material_mgr_listener() override;

    /// Callback called each time a scheme is not found
    Ogre::Technique* handleSchemeNotFound(
        std::uint16_t _scheme_index,
        const Ogre::String& _scheme_name,
        Ogre::Material* _original_material,
        std::uint16_t _lod_index,
        const Ogre::Renderable* _renderable
    ) override;

private:

    /**
     * @brief Ensure that a given fragment program is created.
     *
     * @param[in] _name name of the program
     * @param[in] _algoName name of the algorithm
     * @param[in] _algoPassName new suffix to use
     * @param[in] _baseName new suffix to use
     */
    static Ogre::GpuProgramPtr ensure_fp_created(
        const std::string& _name,
        const std::string& _algo_name,
        const std::string& _algo_pass_name,
        const std::string& _base_name,
        const std::string& _source_name
    );
};

} // namespace sight::viz::scene3d::compositor
