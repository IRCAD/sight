/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "viz/scene3d/compositor/material_mgr_listener.hpp"

#include "viz/scene3d/helper/shading.hpp"
#include "viz/scene3d/helper/technique.hpp"

#include <core/spy_log.hpp>

#include <OGRE/OgreHighLevelGpuProgram.h>
#include <OGRE/OgreHighLevelGpuProgramManager.h>
#include <OGRE/OgreTechnique.h>

namespace sight::viz::scene3d::compositor
{

// ----------------------------------------------------------------------------

material_mgr_listener::~material_mgr_listener()
= default;

// ----------------------------------------------------------------------------

Ogre::Technique* viz::scene3d::compositor::material_mgr_listener::handleSchemeNotFound(
    std::uint16_t /*_schemeIndex*/,
    const Ogre::String& _scheme_name,
    Ogre::Material* _original_material,
    std::uint16_t /*_lodIndex*/,
    const Ogre::Renderable*
    /*_renderable*/
)
{
    const auto mtl_name = _original_material->getName();

    // Volume rendering techniques don't handle OIT. Ignore them.
    if(Ogre::StringUtil::startsWith(mtl_name, "RTV_Mat") || Ogre::StringUtil::endsWith(mtl_name, "RayEntryPoints"))
    {
        return nullptr;
    }

    // Don't know what is this for exactly, but we don't care so we filter this out
    if(_scheme_name == "ShaderGeneratorDefaultScheme")
    {
        return nullptr;
    }

    auto tokens = Ogre::StringUtil::split(_scheme_name, "/");

    if(tokens.size() < 2)
    {
        return nullptr;
    }

    // DepthPeeling, DualDepthPeeling, WeightedBlended or HybridTransparency
    const auto algo_name = tokens[0];
    // peel, depthMap, transmittanceBlend, etc...
    const auto algo_pass_name = tokens[1];

    Ogre::Technique* new_tech     = nullptr;
    Ogre::Technique* default_tech = _original_material->getTechnique(0);
    Ogre::Technique* depth_tech   = _original_material->getTechnique("depth");
    if(depth_tech == nullptr)
    {
        SIGHT_ERROR(
            "Missing 'depth' technique for material '" + _original_material->getName()
            + "' Order Independent Transparency will probably not be supported."
        );
        depth_tech = default_tech;
    }

    // The R2VB material does not need to fill the OIT schemes, though Ogre get us here to know what to do
    // We simply return the main technique in this case
    const Ogre::Technique::Passes& default_tech_passes = default_tech->getPasses();
    for(auto* const pass : default_tech_passes)
    {
        if(Ogre::StringUtil::startsWith(pass->getGeometryProgramName(), "R2VB/"))
        {
            return default_tech;
        }
    }

    if(_scheme_name == "DepthPeeling/depthMap"
       || _scheme_name == "HybridTransparency/backDepth")
    {
        new_tech = viz::scene3d::helper::technique::copy_to_material(depth_tech, _scheme_name, _original_material);

        const Ogre::Technique::Passes& passes = new_tech->getPasses();
        for(auto* const pass : passes)
        {
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_REPLACE);
            pass->setFragmentProgram("DepthPeeling/depthMap_FP");

            if(algo_name == "HybridTransparency")
            {
                pass->setDepthFunction(Ogre::CMPF_GREATER_EQUAL);
            }
        }
    }
    else if(Ogre::StringUtil::startsWith(_scheme_name, "DepthPeeling/peel", false)
            || Ogre::StringUtil::startsWith(_scheme_name, "CelShadingDepthPeeling/peel", false)
            || Ogre::StringUtil::startsWith(_scheme_name, "HybridTransparency/peel", false))
    {
        new_tech = viz::scene3d::helper::technique::copy_to_material(default_tech, _scheme_name, _original_material);

        const Ogre::Technique::Passes& passes = new_tech->getPasses();
        for(auto* const pass : passes)
        {
            pass->setDepthCheckEnabled(true);
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_REPLACE);

            if(algo_name == "CelShadingDepthPeeling" && pass->getName() != "NormalsPass"
               && pass->getName() != "EdgePass")
            {
                auto vp_name  = pass->getVertexProgramName();
                auto new_name = viz::scene3d::helper::shading::set_technique_in_program_name(vp_name, algo_name);
                pass->setVertexProgram(new_name);
            }

            // replace fragment program and build it if needed
            const auto fp_base_name   = pass->getFragmentProgramName();
            const auto fp_source_name = pass->getFragmentProgram()->getSourceFile();
            auto new_name             = viz::scene3d::helper::shading::set_technique_in_program_name(
                fp_base_name,
                algo_name + "/peel"
            );
            sight::viz::scene3d::compositor::material_mgr_listener::ensure_fp_created(
                new_name,
                algo_name,
                algo_pass_name,
                fp_base_name,
                fp_source_name
            );
            pass->setFragmentProgram(new_name);

            auto num_tex_unit = pass->getNumTextureUnitStates();

            Ogre::TextureUnitState* tex_state = pass->createTextureUnitState();
            tex_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            tex_state->setTextureFiltering(Ogre::TFO_NONE);
            tex_state->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);

            // Modify texture input according to the requested pass
            if(algo_pass_name == "peelInit")
            {
                tex_state->setCompositorReference(algo_name, "peelInit", 0);
            }
            else if(algo_pass_name == "peelPing")
            {
                tex_state->setCompositorReference(algo_name, "pongBuffer", 1);
            }
            else if(algo_pass_name == "peelPong")
            {
                tex_state->setCompositorReference(algo_name, "pingBuffer", 1);
            }
            else
            {
                SIGHT_FATAL("DepthPeeling logical error");
            }

            auto params = pass->getFragmentProgramParameters();
            params->setNamedConstant("u_nearestDepthBuffer", int(num_tex_unit));
            params->setNamedAutoConstant("u_viewport", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);
            if(auto defs = params->getConstantDefinitions().map; defs.find("u_diffuse") != defs.end())
            {
                params->setNamedAutoConstant("u_diffuse", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
            }
        }
    }
    else if(_scheme_name == "WeightedBlended/occlusionMap"
            || _scheme_name == "HybridTransparency/occlusionMap")
    {
        new_tech = viz::scene3d::helper::technique::copy_to_material(default_tech, _scheme_name, _original_material);

        const Ogre::Technique::Passes& passes = new_tech->getPasses();
        for(auto* const pass : passes)
        {
            // replace fragment program and build it if needed
            const auto fp_base_name   = pass->getFragmentProgramName();
            const auto fp_source_name = pass->getFragmentProgram()->getSourceFile();
            auto new_name             =
                viz::scene3d::helper::shading::set_technique_in_program_name(fp_base_name, algo_name + "/occlusionMap");
            sight::viz::scene3d::compositor::material_mgr_listener::ensure_fp_created(
                new_name,
                algo_name,
                algo_pass_name,
                fp_base_name,
                fp_source_name
            );
            pass->setFragmentProgram(new_name);

            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_REPLACE);
        }
    }
    else if(_scheme_name == "WeightedBlended/weightBlend"
            || _scheme_name == "HybridTransparency/weightBlend")
    {
        new_tech = viz::scene3d::helper::technique::copy_to_material(default_tech, _scheme_name, _original_material);

        const Ogre::Technique::Passes& passes = new_tech->getPasses();
        for(auto* const pass : passes)
        {
            // replace fragment program and build it if needed
            const auto fp_base_name   = pass->getFragmentProgramName();
            const auto fp_source_name = pass->getFragmentProgram()->getSourceFile();
            auto new_name             = viz::scene3d::helper::shading::set_technique_in_program_name(
                fp_base_name,
                algo_name + "/weightBlend"
            );
            sight::viz::scene3d::compositor::material_mgr_listener::ensure_fp_created(
                new_name,
                algo_name,
                algo_pass_name,
                fp_base_name,
                fp_source_name
            );
            pass->setFragmentProgram(new_name);

            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_ADD);

            auto num_tex_unit = pass->getNumTextureUnitStates();
            auto params       = pass->getFragmentProgramParameters();

            if(algo_name == "HybridTransparency")
            {
                Ogre::TextureUnitState* tex_state = pass->createTextureUnitState();
                tex_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                tex_state->setTextureFiltering(Ogre::TFO_NONE);
                tex_state->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                tex_state->setCompositorReference(algo_name, "pingBuffer", 1);

                params->setNamedConstant("u_frontDepthBuffer", int(num_tex_unit++));
            }

            Ogre::TextureUnitState* tex_state = pass->createTextureUnitState();
            tex_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            tex_state->setTextureFiltering(Ogre::TFO_NONE);
            tex_state->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
            tex_state->setCompositorReference(algo_name, "occlusion", 0);

            params->setNamedConstant("u_occlusionDepthBuffer", int(num_tex_unit));
            params->setNamedAutoConstant("u_viewport", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);
            params->setNamedAutoConstant("u_near", Ogre::GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE);
            params->setNamedAutoConstant("u_far", Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
        }
    }
    else if(_scheme_name == "WeightedBlended/transmittanceBlend"
            || _scheme_name == "HybridTransparency/transmittanceBlend")
    {
        new_tech = viz::scene3d::helper::technique::copy_to_material(default_tech, _scheme_name, _original_material);

        const Ogre::Technique::Passes& passes = new_tech->getPasses();
        for(auto* const pass : passes)
        {
            // replace fragment program and build it if needed
            const auto fp_base_name   = pass->getFragmentProgramName();
            const auto fp_source_name = pass->getFragmentProgram()->getSourceFile();
            auto new_name             = viz::scene3d::helper::shading::set_technique_in_program_name(
                fp_base_name,
                algo_name + "/transmittanceBlend"
            );
            sight::viz::scene3d::compositor::material_mgr_listener::ensure_fp_created(
                new_name,
                algo_name,
                algo_pass_name,
                fp_base_name,
                fp_source_name
            );
            pass->setFragmentProgram(new_name);

            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBF_ZERO, Ogre::SBF_ONE_MINUS_SOURCE_COLOUR);

            auto num_tex_unit = pass->getNumTextureUnitStates();
            auto params       = pass->getFragmentProgramParameters();

            if(algo_name == "HybridTransparency")
            {
                Ogre::TextureUnitState* tex_state = pass->createTextureUnitState();
                tex_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                tex_state->setTextureFiltering(Ogre::TFO_NONE);
                tex_state->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                tex_state->setCompositorReference(algo_name, "pingBuffer", 1);
                params->setNamedConstant("u_frontDepthBuffer", int(num_tex_unit++));
            }

            Ogre::TextureUnitState* tex_state = pass->createTextureUnitState();
            tex_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            tex_state->setTextureFiltering(Ogre::TFO_NONE);
            tex_state->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
            tex_state->setCompositorReference(algo_name, "occlusion", 0);

            params->setNamedConstant("u_occlusionDepthBuffer", int(num_tex_unit));
            params->setNamedAutoConstant("u_viewport", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);

            if(auto defs = params->getConstantDefinitions().map; defs.find("u_diffuse") != defs.end())
            {
                params->setNamedAutoConstant("u_diffuse", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
            }
        }
    }
    else if(Ogre::StringUtil::startsWith(_scheme_name, "DualDepthPeeling/peelInit", false))
    {
        new_tech = viz::scene3d::helper::technique::copy_to_material(depth_tech, _scheme_name, _original_material);

        const Ogre::Technique::Passes& passes = new_tech->getPasses();
        for(auto* const pass : passes)
        {
            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_ADD);
            pass->setSceneBlendingOperation(Ogre::SBO_MAX);

            pass->setFragmentProgram("DualDepthPeeling/peelInit_FP");
        }
    }
    else if(Ogre::StringUtil::startsWith(_scheme_name, "DualDepthPeeling/peel", false))
    {
        new_tech = viz::scene3d::helper::technique::copy_to_material(default_tech, _scheme_name, _original_material);

        const Ogre::Technique::Passes& passes = new_tech->getPasses();
        for(auto* const pass : passes)
        {
            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_ADD);
            pass->setSceneBlendingOperation(Ogre::SBO_MAX);

            // replace fragment program and build it if needed
            const auto fp_base_name   = pass->getFragmentProgramName();
            const auto fp_source_name = pass->getFragmentProgram()->getSourceFile();
            auto new_name             = viz::scene3d::helper::shading::set_technique_in_program_name(
                fp_base_name,
                algo_name + "/peel"
            );
            sight::viz::scene3d::compositor::material_mgr_listener::ensure_fp_created(
                new_name,
                algo_name,
                algo_pass_name,
                fp_base_name,
                fp_source_name
            );
            pass->setFragmentProgram(new_name);

            std::string input_buffer;
            if(algo_pass_name == "peelPing")
            {
                input_buffer = "pongBuffer";
            }
            else if(algo_pass_name == "peelPong")
            {
                input_buffer = "pingBuffer";
            }
            else
            {
                SIGHT_FATAL("DualDepthPeeling logical error");
            }

            auto num_tex_unit = pass->getNumTextureUnitStates();

            // Modify texture input according to the requested pass
            for(Ogre::uint32 i = 0 ; i < 4 ; ++i)
            {
                Ogre::TextureUnitState* tex_state = pass->createTextureUnitState();
                tex_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                tex_state->setTextureFiltering(Ogre::TFO_NONE);
                tex_state->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                tex_state->setCompositorReference(algo_name, input_buffer, i == 3 ? 4 : i);
            }

            auto params = pass->getFragmentProgramParameters();
            params->setNamedConstant("u_nearestDepthBuffer", int(num_tex_unit));
            params->setNamedConstant("u_farthestDepthBuffer", int(num_tex_unit + 1));
            params->setNamedConstant("u_forwardColorBuffer", int(num_tex_unit + 2));
            params->setNamedConstant("u_forwardAlphasBuffer", int(num_tex_unit + 3));
            params->setNamedAutoConstant("u_viewport", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);

            if(auto defs = params->getConstantDefinitions().map; defs.find("u_diffuse") != defs.end())
            {
                params->setNamedAutoConstant("u_diffuse", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
            }
        }
    }
    else
    {
        SIGHT_INFO("not found : " << _scheme_name);
    }

    return new_tech;
}

// ----------------------------------------------------------------------------

Ogre::GpuProgramPtr material_mgr_listener::ensure_fp_created(
    const std::string& _name,
    const std::string& _algo_name,
    const std::string& _algo_pass_name,
    const std::string& _base_name,
    const std::string& _source_name
)
{
    // Determine shader source file and parameters
    viz::scene3d::helper::shading::gpu_program_parameters_t parameters;

    // Set specific shader according to the algo and the pass
    if(_algo_name == "DepthPeeling" || _algo_name == "CelShadingDepthPeeling")
    {
        parameters.emplace_back("preprocessor_defines", "DEPTH_PEELING=1");
    }
    else if(_algo_name == "DualDepthPeeling")
    {
        parameters.emplace_back("preprocessor_defines", "DUAL_DEPTH_PEELING=1");
    }
    else if(_algo_name == "HybridTransparency")
    {
        if(_algo_pass_name == "peel" || _algo_pass_name == "peelInit")
        {
            parameters.emplace_back("preprocessor_defines", "DEPTH_PEELING=1");
        }
        else
        {
            if(_algo_pass_name == "transmittanceBlend")
            {
                parameters.emplace_back("preprocessor_defines", "HYBRID=1,WBOIT_TRANSMIT=1");
            }
            else if(_algo_pass_name == "occlusionMap")
            {
                parameters.emplace_back("preprocessor_defines", "HYBRID=1,WBOIT_OCCLUSION=1");
            }
            else
            {
                parameters.emplace_back("preprocessor_defines", "HYBRID=1,WBOIT=1");
            }
        }
    }
    else if(_algo_name == "WeightedBlended")
    {
        if(_algo_pass_name == "transmittanceBlend")
        {
            parameters.emplace_back("preprocessor_defines", "WBOIT_TRANSMIT=1");
        }
        else if(_algo_pass_name == "occlusionMap")
        {
            parameters.emplace_back("preprocessor_defines", "WBOIT_OCCLUSION=1");
        }
        else
        {
            parameters.emplace_back("preprocessor_defines", "WBOIT=1");
        }
    }
    else
    {
        SIGHT_FATAL("Unreachable code");
    }

    return viz::scene3d::helper::shading::create_program_from(
        _name,
        _source_name,
        parameters,
        Ogre::GPT_FRAGMENT_PROGRAM,
        _base_name
    );
}

// ----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::compositor
