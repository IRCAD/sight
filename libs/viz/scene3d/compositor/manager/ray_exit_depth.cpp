/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "viz/scene3d/compositor/manager/ray_exit_depth.hpp"

#include "viz/scene3d/helper/shading.hpp"
#include "viz/scene3d/helper/technique.hpp"
#include "viz/scene3d/ogre.hpp"

#include <core/spy_log.hpp>

#include <OGRE/OgrePass.h>
#include <OGRE/OgreTechnique.h>

namespace sight::viz::scene3d::compositor::manager
{

//------------------------------------------------------------------------------

Ogre::Technique* ray_exit_depth::handleSchemeNotFound(
    std::uint16_t /*_schemeIndex*/,
    const Ogre::String& _scheme_name,
    Ogre::Material* _original_material,
    std::uint16_t /*_lodIndex*/,
    const Ogre::Renderable* /*_renderable*/
)
{
    Ogre::Technique* new_technique = nullptr;
    const auto mtl_name            = _original_material->getName();

    if(_scheme_name == "VolumeEntries_FrontFacesMin" && !Ogre::StringUtil::startsWith(mtl_name, "RTV_Mat"))
    {
        // Copy the first technique, set appropriate raster ops and add a RAY_EXIT_POINTS define in the fragment program
        // That should be sufficient for most materials that render with Transparency.inc.glsl
        static const std::string s_TECH_NAME = "FrontFacesMin";
        auto* color_tech                     = _original_material->getTechnique("");

        new_technique = viz::scene3d::helper::technique::copy_to_material(
            color_tech,
            _scheme_name,
            _original_material
        );

        const Ogre::Technique::Passes& passes = new_technique->getPasses();
        for(auto* const pass : passes)
        {
            const auto fp_base_name = pass->getFragmentProgramName();
            const auto fp_src_name  = pass->getFragmentProgram()->getSourceFile();
            auto new_name           = viz::scene3d::helper::shading::set_technique_in_program_name(
                fp_base_name,
                s_TECH_NAME
            );

            viz::scene3d::helper::shading::gpu_program_parameters_t parameters {
                {"preprocessor_defines", "RAY_EXIT_POINTS=1"}
            };

            auto program = viz::scene3d::helper::shading::create_program_from(
                new_name,
                fp_src_name,
                parameters,
                Ogre::GPT_FRAGMENT_PROGRAM,
                fp_base_name
            );
            pass->setGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM, program);

            pass->setSceneBlendingOperation(Ogre::SBO_MAX);

            auto params = pass->getFragmentProgramParameters();
            if(auto defs = params->getConstantDefinitions().map; defs.find("u_diffuse") != defs.end())
            {
                params->setNamedAutoConstant("u_diffuse", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
            }
        }

        new_technique->setDepthCheckEnabled(true);
        new_technique->setDepthWriteEnabled(true);
        new_technique->setDepthFunction(Ogre::CMPF_LESS);
        new_technique->setCullingMode(Ogre::CULL_NONE);
        new_technique->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
        new_technique->setDepthBias(1, 1);
        new_technique->setColourWriteEnabled(false, true, false, false);
        new_technique->setSceneBlending(Ogre::SBF_SOURCE_COLOUR, Ogre::SBF_DEST_COLOUR);
    }

    return new_technique;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::compositor::manager
