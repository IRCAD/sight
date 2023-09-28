/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "viz/scene3d/compositor/listener/RayExitDepth.hpp"

#include "viz/scene3d/helper/Shading.hpp"
#include "viz/scene3d/helper/Technique.hpp"
#include "viz/scene3d/ogre.hpp"

#include <core/spy_log.hpp>

#include <OGRE/OgrePass.h>
#include <OGRE/OgreTechnique.h>

namespace sight::viz::scene3d::compositor::listener
{

//------------------------------------------------------------------------------

Ogre::Technique* RayExitDepthListener::handleSchemeNotFound(
    std::uint16_t /*_schemeIndex*/,
    const Ogre::String& _schemeName,
    Ogre::Material* _originalMaterial,
    std::uint16_t /*_lodIndex*/,
    const Ogre::Renderable* /*_renderable*/
)
{
    Ogre::Technique* newTechnique = nullptr;
    const auto mtlName            = _originalMaterial->getName();

    if(_schemeName == "VolumeEntries_FrontFacesMin" && !Ogre::StringUtil::startsWith(mtlName, "RTV_Mat"))
    {
        // Copy the first technique, set appropriate raster ops and add a RAY_EXIT_POINTS define in the fragment program
        // That should be sufficient for most materials that render with Transparency.inc.glsl
        static const std::string techName = "FrontFacesMin";
        auto* colorTech                   = _originalMaterial->getTechnique("");

        newTechnique = viz::scene3d::helper::Technique::copyToMaterial(
            colorTech,
            _schemeName,
            _originalMaterial
        );

        const Ogre::Technique::Passes& passes = newTechnique->getPasses();
        for(auto* const pass : passes)
        {
            const auto fpName       = pass->getFragmentProgramName();
            const auto fpSourceName = pass->getFragmentProgram()->getSourceFile();
            auto newName            = viz::scene3d::helper::Shading::setTechniqueInProgramName(fpName, techName);

            viz::scene3d::helper::Shading::GpuProgramParametersType parameters {
                {"preprocessor_defines", "RAY_EXIT_POINTS=1"}
            };

            auto program = viz::scene3d::helper::Shading::createProgramFrom(
                newName,
                fpSourceName,
                parameters,
                Ogre::GPT_FRAGMENT_PROGRAM,
                fpName
            );
            pass->setGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM, program);

            pass->setSceneBlendingOperation(Ogre::SBO_MAX);

            auto params = pass->getFragmentProgramParameters();
            if(auto defs = params->getConstantDefinitions().map; defs.find("u_diffuse") != defs.end())
            {
                params->setNamedAutoConstant("u_diffuse", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
            }
        }

        newTechnique->setDepthCheckEnabled(true);
        newTechnique->setDepthWriteEnabled(true);
        newTechnique->setDepthFunction(Ogre::CMPF_LESS);
        newTechnique->setCullingMode(Ogre::CULL_NONE);
        newTechnique->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
        newTechnique->setDepthBias(1, 1);
        newTechnique->setColourWriteEnabled(false, true, false, false);
        newTechnique->setSceneBlending(Ogre::SBF_SOURCE_COLOUR, Ogre::SBF_DEST_COLOUR);
    }

    return newTechnique;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::compositor::listener
