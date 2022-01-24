/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "viz/scene3d/compositor/MaterialMgrListener.hpp"

#include "viz/scene3d/helper/Shading.hpp"
#include "viz/scene3d/helper/Technique.hpp"

#include <core/spyLog.hpp>

#include <OGRE/OgreHighLevelGpuProgram.h>
#include <OGRE/OgreHighLevelGpuProgramManager.h>
#include <OGRE/OgreTechnique.h>

namespace sight::viz::scene3d
{

namespace compositor
{

// ----------------------------------------------------------------------------

MaterialMgrListener::~MaterialMgrListener()
{
}

// ----------------------------------------------------------------------------

Ogre::Technique* viz::scene3d::compositor::MaterialMgrListener::handleSchemeNotFound(
    unsigned short /*_schemeIndex*/,
    const Ogre::String& _schemeName,
    Ogre::Material* _originalMaterial,
    unsigned short /*_lodIndex*/,
    const Ogre::Renderable*
    /*_renderable*/
)
{
    const auto mtlName = _originalMaterial->getName();

    // Volume rendering techniques don't handle OIT. Ignore them.
    if(Ogre::StringUtil::startsWith(mtlName, "RTV_Mat") || Ogre::StringUtil::endsWith(mtlName, "RayEntryPoints"))
    {
        return nullptr;
    }

    // Don't know what is this for exactly, but we don't care so we filter this out
    if(_schemeName == "ShaderGeneratorDefaultScheme")
    {
        return nullptr;
    }

    auto tokens = Ogre::StringUtil::split(_schemeName, "/");

    if(tokens.size() < 2)
    {
        return nullptr;
    }

    // DepthPeeling, DualDepthPeeling, WeightedBlended or HybridTransparency
    const auto algoName = tokens[0];
    // peel, depthMap, transmittanceBlend, etc...
    const auto algoPassName = tokens[1];

    Ogre::Technique* newTech     = nullptr;
    Ogre::Technique* defaultTech = _originalMaterial->getTechnique(0);
    Ogre::Technique* depthTech   = _originalMaterial->getTechnique("depth");
    if(depthTech == nullptr)
    {
        SIGHT_ERROR(
            "Missing 'depth' technique for material '" + _originalMaterial->getName()
            + "' Order Independent Transparency will probably not be supported."
        );
        depthTech = defaultTech;
    }

    // The R2VB material does not need to fill the OIT schemes, though Ogre get us here to know what to do
    // We simply return the main technique in this case
    const Ogre::Technique::Passes& defaultTechPasses = defaultTech->getPasses();
    for(const auto pass : defaultTechPasses)
    {
        if(Ogre::StringUtil::startsWith(pass->getGeometryProgramName(), "R2VB/"))
        {
            return defaultTech;
        }
    }

    if(_schemeName == "DepthPeeling/depthMap"
       || _schemeName == "HybridTransparency/backDepth")
    {
        newTech = viz::scene3d::helper::Technique::copyToMaterial(depthTech, _schemeName, _originalMaterial);

        const Ogre::Technique::Passes& passes = newTech->getPasses();
        for(const auto pass : passes)
        {
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_REPLACE);
            pass->setFragmentProgram("DepthPeeling/depthMap_FP");

            if(algoName == "HybridTransparency")
            {
                pass->setDepthFunction(Ogre::CMPF_GREATER_EQUAL);
            }
        }
    }
    else if(Ogre::StringUtil::startsWith(_schemeName, "DepthPeeling/peel", false)
            || Ogre::StringUtil::startsWith(_schemeName, "CelShadingDepthPeeling/peel", false)
            || Ogre::StringUtil::startsWith(_schemeName, "HybridTransparency/peel", false))
    {
        newTech = viz::scene3d::helper::Technique::copyToMaterial(defaultTech, _schemeName, _originalMaterial);

        const Ogre::Technique::Passes& passes = newTech->getPasses();
        for(const auto pass : passes)
        {
            pass->setDepthCheckEnabled(true);
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_REPLACE);

            if(algoName == "CelShadingDepthPeeling" && pass->getName() != "NormalsPass"
               && pass->getName() != "EdgePass")
            {
                auto vpName  = pass->getVertexProgramName();
                auto newName = viz::scene3d::helper::Shading::setTechniqueInProgramName(vpName, algoName);
                pass->setVertexProgram(newName);
            }

            // replace fragment program and build it if needed
            const auto fpName       = pass->getFragmentProgramName();
            const auto fpSourceName = pass->getFragmentProgram()->getSourceFile();
            auto newName            = viz::scene3d::helper::Shading::setTechniqueInProgramName(
                fpName,
                algoName + "/peel"
            );
            this->ensureFPCreated(newName, algoName, algoPassName, fpName, fpSourceName);
            pass->setFragmentProgram(newName);

            auto numTexUnit                  = pass->getNumTextureUnitStates();
            Ogre::TextureUnitState* texState = pass->createTextureUnitState();
            texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            texState->setTextureFiltering(Ogre::TFO_NONE);
            texState->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);

            // Modify texture input according to the requested pass
            if(algoPassName == "peelInit")
            {
                texState->setCompositorReference(algoName, "peelInit", 0);
            }
            else if(algoPassName == "peelPing")
            {
                texState->setCompositorReference(algoName, "pongBuffer", 1);
            }
            else if(algoPassName == "peelPong")
            {
                texState->setCompositorReference(algoName, "pingBuffer", 1);
            }
            else
            {
                SIGHT_FATAL("DepthPeeling logical error");
            }

            auto params = pass->getFragmentProgramParameters();
            params->setNamedConstant("u_nearestDepthBuffer", numTexUnit);
            params->setNamedAutoConstant("u_viewport", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);
            if(auto defs = params->getConstantDefinitions().map; defs.find("u_diffuse") != defs.end())
            {
                params->setNamedAutoConstant("u_diffuse", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
            }
        }
    }
    else if(_schemeName == "WeightedBlended/occlusionMap"
            || _schemeName == "HybridTransparency/occlusionMap")
    {
        newTech = viz::scene3d::helper::Technique::copyToMaterial(defaultTech, _schemeName, _originalMaterial);

        const Ogre::Technique::Passes& passes = newTech->getPasses();
        for(const auto pass : passes)
        {
            // replace fragment program and build it if needed
            const auto fpName       = pass->getFragmentProgramName();
            const auto fpSourceName = pass->getFragmentProgram()->getSourceFile();
            auto newName            =
                viz::scene3d::helper::Shading::setTechniqueInProgramName(fpName, algoName + "/occlusionMap");
            this->ensureFPCreated(newName, algoName, algoPassName, fpName, fpSourceName);
            pass->setFragmentProgram(newName);

            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_REPLACE);
        }
    }
    else if(_schemeName == "WeightedBlended/weightBlend"
            || _schemeName == "HybridTransparency/weightBlend")
    {
        newTech = viz::scene3d::helper::Technique::copyToMaterial(defaultTech, _schemeName, _originalMaterial);

        const Ogre::Technique::Passes& passes = newTech->getPasses();
        for(const auto pass : passes)
        {
            // replace fragment program and build it if needed
            const auto fpName       = pass->getFragmentProgramName();
            const auto fpSourceName = pass->getFragmentProgram()->getSourceFile();
            auto newName            = viz::scene3d::helper::Shading::setTechniqueInProgramName(
                fpName,
                algoName + "/weightBlend"
            );
            this->ensureFPCreated(newName, algoName, algoPassName, fpName, fpSourceName);
            pass->setFragmentProgram(newName);

            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_ADD);

            auto numTexUnit = pass->getNumTextureUnitStates();
            auto params     = pass->getFragmentProgramParameters();

            if(algoName == "HybridTransparency")
            {
                Ogre::TextureUnitState* texState = pass->createTextureUnitState();
                texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                texState->setTextureFiltering(Ogre::TFO_NONE);
                texState->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                texState->setCompositorReference(algoName, "pingBuffer", 1);

                params->setNamedConstant("u_frontDepthBuffer", numTexUnit++);
            }

            Ogre::TextureUnitState* texState = pass->createTextureUnitState();
            texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            texState->setTextureFiltering(Ogre::TFO_NONE);
            texState->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
            texState->setCompositorReference(algoName, "occlusion", 0);

            params->setNamedConstant("u_occlusionDepthBuffer", numTexUnit);
            params->setNamedAutoConstant("u_viewport", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);
            params->setNamedAutoConstant("u_near", Ogre::GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE);
            params->setNamedAutoConstant("u_far", Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
        }
    }
    else if(_schemeName == "WeightedBlended/transmittanceBlend"
            || _schemeName == "HybridTransparency/transmittanceBlend")
    {
        newTech = viz::scene3d::helper::Technique::copyToMaterial(defaultTech, _schemeName, _originalMaterial);

        const Ogre::Technique::Passes& passes = newTech->getPasses();
        for(const auto pass : passes)
        {
            // replace fragment program and build it if needed
            const auto fpName       = pass->getFragmentProgramName();
            const auto fpSourceName = pass->getFragmentProgram()->getSourceFile();
            auto newName            = viz::scene3d::helper::Shading::setTechniqueInProgramName(
                fpName,
                algoName + "/transmittanceBlend"
            );
            this->ensureFPCreated(newName, algoName, algoPassName, fpName, fpSourceName);
            pass->setFragmentProgram(newName);

            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBF_ZERO, Ogre::SBF_ONE_MINUS_SOURCE_COLOUR);

            auto numTexUnit = pass->getNumTextureUnitStates();
            auto params     = pass->getFragmentProgramParameters();

            if(algoName == "HybridTransparency")
            {
                Ogre::TextureUnitState* texState = pass->createTextureUnitState();
                texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                texState->setTextureFiltering(Ogre::TFO_NONE);
                texState->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                texState->setCompositorReference(algoName, "pingBuffer", 1);
                params->setNamedConstant("u_frontDepthBuffer", numTexUnit++);
            }

            Ogre::TextureUnitState* texState = pass->createTextureUnitState();
            texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            texState->setTextureFiltering(Ogre::TFO_NONE);
            texState->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
            texState->setCompositorReference(algoName, "occlusion", 0);

            params->setNamedConstant("u_occlusionDepthBuffer", numTexUnit);
            params->setNamedAutoConstant("u_viewport", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);

            if(auto defs = params->getConstantDefinitions().map; defs.find("u_diffuse") != defs.end())
            {
                params->setNamedAutoConstant("u_diffuse", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
            }
        }
    }
    else if(Ogre::StringUtil::startsWith(_schemeName, "DualDepthPeeling/peelInit", false))
    {
        newTech = viz::scene3d::helper::Technique::copyToMaterial(depthTech, _schemeName, _originalMaterial);

        const Ogre::Technique::Passes& passes = newTech->getPasses();
        for(const auto pass : passes)
        {
            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_ADD);
            pass->setSceneBlendingOperation(Ogre::SBO_MAX);

            pass->setFragmentProgram("DualDepthPeeling/peelInit_FP");
        }
    }
    else if(Ogre::StringUtil::startsWith(_schemeName, "DualDepthPeeling/peel", false))
    {
        newTech = viz::scene3d::helper::Technique::copyToMaterial(defaultTech, _schemeName, _originalMaterial);

        const Ogre::Technique::Passes& passes = newTech->getPasses();
        for(const auto pass : passes)
        {
            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_ADD);
            pass->setSceneBlendingOperation(Ogre::SBO_MAX);

            // replace fragment program and build it if needed
            const auto fpName       = pass->getFragmentProgramName();
            const auto fpSourceName = pass->getFragmentProgram()->getSourceFile();
            auto newName            = viz::scene3d::helper::Shading::setTechniqueInProgramName(
                fpName,
                algoName + "/peel"
            );
            this->ensureFPCreated(newName, algoName, algoPassName, fpName, fpSourceName);
            pass->setFragmentProgram(newName);

            std::string inputBuffer;
            if(algoPassName == "peelPing")
            {
                inputBuffer = "pongBuffer";
            }
            else if(algoPassName == "peelPong")
            {
                inputBuffer = "pingBuffer";
            }
            else
            {
                SIGHT_FATAL("DualDepthPeeling logical error");
            }

            auto numTexUnit = pass->getNumTextureUnitStates();

            // Modify texture input according to the requested pass
            for(size_t i = 0 ; i < 4 ; ++i)
            {
                Ogre::TextureUnitState* texState = pass->createTextureUnitState();
                texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                texState->setTextureFiltering(Ogre::TFO_NONE);
                texState->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                texState->setCompositorReference(algoName, inputBuffer, i == 3 ? 4 : i);
            }

            auto params = pass->getFragmentProgramParameters();
            params->setNamedConstant("u_nearestDepthBuffer", numTexUnit);
            params->setNamedConstant("u_farthestDepthBuffer", numTexUnit + 1);
            params->setNamedConstant("u_forwardColorBuffer", numTexUnit + 2);
            params->setNamedConstant("u_forwardAlphasBuffer", numTexUnit + 3);
            params->setNamedAutoConstant("u_viewport", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);

            if(auto defs = params->getConstantDefinitions().map; defs.find("u_diffuse") != defs.end())
            {
                params->setNamedAutoConstant("u_diffuse", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
            }
        }
    }
    else
    {
        SIGHT_INFO("not found : " << _schemeName);
    }

    return newTech;
}

// ----------------------------------------------------------------------------

Ogre::GpuProgramPtr MaterialMgrListener::ensureFPCreated(
    const std::string& _name,
    const std::string& _algoName,
    const std::string& _algoPassName,
    const std::string& _baseName,
    const std::string& _sourceName
)
{
    // Determine shader source file and parameters
    viz::scene3d::helper::Shading::GpuProgramParametersType parameters;

    // Set specific shader according to the algo and the pass
    if(_algoName == "DepthPeeling")
    {
        parameters.push_back({"preprocessor_defines", "DEPTH_PEELING=1"});
    }
    else if(_algoName == "DualDepthPeeling")
    {
        parameters.push_back({"preprocessor_defines", "DUAL_DEPTH_PEELING=1"});
    }
    else if(_algoName == "HybridTransparency")
    {
        if(_algoPassName == "peel" || _algoPassName == "peelInit")
        {
            parameters.push_back({"preprocessor_defines", "DEPTH_PEELING=1"});
        }
        else
        {
            if(_algoPassName == "transmittanceBlend")
            {
                parameters.push_back({"preprocessor_defines", "HYBRID=1,WBOIT_TRANSMIT=1"});
            }
            else if(_algoPassName == "occlusionMap")
            {
                parameters.push_back({"preprocessor_defines", "HYBRID=1,WBOIT_OCCLUSION=1"});
            }
            else
            {
                parameters.push_back({"preprocessor_defines", "HYBRID=1,WBOIT=1"});
            }
        }
    }
    else if(_algoName == "WeightedBlended")
    {
        if(_algoPassName == "transmittanceBlend")
        {
            parameters.push_back({"preprocessor_defines", "WBOIT_TRANSMIT=1"});
        }
        else if(_algoPassName == "occlusionMap")
        {
            parameters.push_back({"preprocessor_defines", "WBOIT_OCCLUSION=1"});
        }
        else
        {
            parameters.push_back({"preprocessor_defines", "WBOIT=1"});
        }
    }
    else if(_algoName == "CelShadingDepthPeeling")
    {
        parameters.push_back({"preprocessor_defines", "DEPTH_PEELING=1"});
    }
    else
    {
        SIGHT_FATAL("Unreachable code");
    }

    return viz::scene3d::helper::Shading::createProgramFrom(
        _name,
        _sourceName,
        parameters,
        Ogre::GPT_FRAGMENT_PROGRAM,
        _baseName
    );
}

// ----------------------------------------------------------------------------

} // namespace compositor

} // namespace sight::viz::scene3d
