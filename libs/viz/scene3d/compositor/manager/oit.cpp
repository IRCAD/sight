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

#include "viz/scene3d/compositor/manager/oit.hpp"

#include "viz/scene3d/helper/shading.hpp"
#include "viz/scene3d/helper/technique.hpp"

#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreCompositionPass.h>
#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreTechnique.h>

namespace sight::viz::scene3d::compositor::manager
{

static const std::string SEPARATOR = "/";

struct algo
{
    static const inline std::string CEL_SHADING        = "CelShadingDepthPeeling";
    static const inline std::string DEPTH_PEELING      = "DepthPeeling";
    static const inline std::string DUAL_DEPTH_PEELING = "DualDepthPeeling";
    static const inline std::string HYBRID             = "HybridTransparency";
    static const inline std::string WEIGHTED_BLENDED   = "WeightedBlended";
};

struct passes
{
    static const inline std::string PEEL                = "peel";
    static const inline std::string PEEL_INIT           = PEEL + "_init";
    static const inline std::string PEEL_PING           = PEEL + "_ping";
    static const inline std::string PEEL_PONG           = PEEL + "_pong";
    static const inline std::string BLEND               = "blend";
    static const inline std::string DEPTH_MAP           = "depthMap";
    static const inline std::string OCCLUSION_MAP       = "occlusionMap";
    static const inline std::string WEIGHT_BLEND        = "weightBlend";
    static const inline std::string TRANSMITTANCE_BLEND = "transmittanceBlend";

    //------------------------------------------------------------------------------

    static constexpr std::string ping_pong(int _i)
    {
        return (_i % 2) != 0 ? passes::PEEL_PING : passes::PEEL_PONG;
    }
};

struct buffers
{
    static const inline std::string INIT           = "init_buffer";
    static const inline std::string PING           = "ping_buffer";
    static const inline std::string PONG           = "pong_buffer";
    static const inline std::string GBUFFER        = "gbuffer";
    static const inline std::string OCCLUSION      = "occlusion";
    static const inline std::string TRANSMITTANCE  = "transmittance";
    static const inline std::string WEIGHTED_COLOR = "weightedColor";
    static const inline std::string WBOIT_OUTPUT   = "WBOIT_output";

    //------------------------------------------------------------------------------

    static constexpr std::string ping_pong(int _i)
    {
        return (_i % 2) != 0 ? buffers::PING : buffers::PONG;
    }
};

//------------------------------------------------------------------------------

static constexpr std::string join(const std::string& _str1, const std::string& _str2)
{
    return _str1 + SEPARATOR + _str2;
}

// ----------------------------------------------------------------------------

Ogre::Technique* viz::scene3d::compositor::manager::oit::handleSchemeNotFound(
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

    auto tokens = Ogre::StringUtil::split(_scheme_name, SEPARATOR);

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
        if(Ogre::StringUtil::startsWith(pass->getGeometryProgramName(), "R2VB" + SEPARATOR))
        {
            return default_tech;
        }
    }

    const auto is_cel_shading        = algo_name == algo::CEL_SHADING;
    const auto is_depth_peeling      = algo_name == algo::DEPTH_PEELING;
    const auto is_hybrid             = algo_name == algo::HYBRID;
    const auto is_weighted_blended   = algo_name == algo::WEIGHTED_BLENDED;
    const auto is_dual_depth_peeling = algo_name == algo::DUAL_DEPTH_PEELING;

    if((is_depth_peeling || is_hybrid) && algo_pass_name == passes::DEPTH_MAP)
    {
        new_tech = viz::scene3d::helper::technique::copy_to_material(depth_tech, _scheme_name, _original_material);

        const Ogre::Technique::Passes& passes = new_tech->getPasses();
        for(auto* const pass : passes)
        {
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_REPLACE);
            pass->setFragmentProgram(join(algo::DEPTH_PEELING, "depthMap_FP"));

            if(is_hybrid)
            {
                pass->setDepthFunction(Ogre::CMPF_GREATER_EQUAL);
            }
        }
    }
    else if((is_cel_shading || is_depth_peeling || is_hybrid)
            && Ogre::StringUtil::startsWith(algo_pass_name, passes::PEEL, false))
    {
        new_tech = viz::scene3d::helper::technique::copy_to_material(default_tech, _scheme_name, _original_material);

        const Ogre::Technique::Passes& passes = new_tech->getPasses();
        for(auto* const pass : passes)
        {
            pass->setDepthCheckEnabled(true);
            pass->setCullingMode(Ogre::CULL_NONE);
            pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(Ogre::SBT_REPLACE);

            if(is_cel_shading && pass->getName() != "NormalsPass" && pass->getName() != "EdgePass")
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
                join(algo_name, passes::BLEND)
            );
            sight::viz::scene3d::compositor::manager::oit::ensure_fp_created(
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
            if(algo_pass_name == passes::PEEL_INIT)
            {
                tex_state->setCompositorReference(algo_name, buffers::INIT, 0);
            }
            else if(algo_pass_name == passes::PEEL_PING)
            {
                tex_state->setCompositorReference(algo_name, buffers::PONG, 1);
            }
            else if(algo_pass_name == passes::PEEL_PONG)
            {
                tex_state->setCompositorReference(algo_name, buffers::PING, 1);
            }
            else
            {
                SIGHT_FATAL("DepthPeeling logical error: unexpected pass name " << std::quoted(algo_pass_name));
            }

            auto params = pass->getFragmentProgramParameters();
            params->setNamedConstant("u_nearestDepthBuffer", int(num_tex_unit));
            params->setNamedAutoConstant("u_viewport", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);
            if(auto defs = params->getConstantDefinitions().map; defs.contains("u_diffuse"))
            {
                params->setNamedAutoConstant("u_diffuse", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
            }
        }
    }
    else if(is_hybrid || is_weighted_blended)
    {
        if(algo_pass_name == passes::OCCLUSION_MAP)
        {
            new_tech = viz::scene3d::helper::technique::copy_to_material(
                default_tech,
                _scheme_name,
                _original_material
            );

            const Ogre::Technique::Passes& passes = new_tech->getPasses();
            for(auto* const pass : passes)
            {
                // replace fragment program and build it if needed
                const auto fp_base_name   = pass->getFragmentProgramName();
                const auto fp_source_name = pass->getFragmentProgram()->getSourceFile();
                auto new_name             =
                    viz::scene3d::helper::shading::set_technique_in_program_name(
                        fp_base_name,
                        join(algo_name, passes::OCCLUSION_MAP)
                    );
                sight::viz::scene3d::compositor::manager::oit::ensure_fp_created(
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
        else if(algo_pass_name == passes::WEIGHT_BLEND)
        {
            new_tech = viz::scene3d::helper::technique::copy_to_material(
                default_tech,
                _scheme_name,
                _original_material
            );

            const Ogre::Technique::Passes& passes = new_tech->getPasses();
            for(auto* const pass : passes)
            {
                // replace fragment program and build it if needed
                const auto fp_base_name   = pass->getFragmentProgramName();
                const auto fp_source_name = pass->getFragmentProgram()->getSourceFile();
                auto new_name             = viz::scene3d::helper::shading::set_technique_in_program_name(
                    fp_base_name,
                    join(algo_name, passes::WEIGHT_BLEND)
                );
                sight::viz::scene3d::compositor::manager::oit::ensure_fp_created(
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

                if(is_hybrid)
                {
                    Ogre::TextureUnitState* tex_state = pass->createTextureUnitState();
                    tex_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                    tex_state->setTextureFiltering(Ogre::TFO_NONE);
                    tex_state->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                    tex_state->setCompositorReference(algo_name, buffers::PING, 1);

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
        else if(algo_pass_name == passes::TRANSMITTANCE_BLEND)
        {
            new_tech = viz::scene3d::helper::technique::copy_to_material(
                default_tech,
                _scheme_name,
                _original_material
            );

            const Ogre::Technique::Passes& passes = new_tech->getPasses();
            for(auto* const pass : passes)
            {
                // replace fragment program and build it if needed
                const auto fp_base_name   = pass->getFragmentProgramName();
                const auto fp_source_name = pass->getFragmentProgram()->getSourceFile();
                auto new_name             = viz::scene3d::helper::shading::set_technique_in_program_name(
                    fp_base_name,
                    join(algo_name, passes::TRANSMITTANCE_BLEND)
                );
                sight::viz::scene3d::compositor::manager::oit::ensure_fp_created(
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

                if(is_hybrid)
                {
                    Ogre::TextureUnitState* tex_state = pass->createTextureUnitState();
                    tex_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                    tex_state->setTextureFiltering(Ogre::TFO_NONE);
                    tex_state->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                    tex_state->setCompositorReference(algo_name, buffers::PING, 1);
                    params->setNamedConstant("u_frontDepthBuffer", int(num_tex_unit++));
                }

                Ogre::TextureUnitState* tex_state = pass->createTextureUnitState();
                tex_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                tex_state->setTextureFiltering(Ogre::TFO_NONE);
                tex_state->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                tex_state->setCompositorReference(algo_name, "occlusion", 0);

                params->setNamedConstant("u_occlusionDepthBuffer", int(num_tex_unit));
                params->setNamedAutoConstant("u_viewport", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);

                if(auto defs = params->getConstantDefinitions().map; defs.contains("u_diffuse"))
                {
                    params->setNamedAutoConstant("u_diffuse", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
                }
            }
        }
    }
    else if(is_dual_depth_peeling && algo_pass_name == passes::PEEL_INIT)
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

            pass->setFragmentProgram(join(algo::DUAL_DEPTH_PEELING, "peel_init_FP"));
        }
    }
    else if(is_dual_depth_peeling && Ogre::StringUtil::startsWith(algo_pass_name, passes::PEEL, false))
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
                join(algo_name, passes::PEEL)
            );
            sight::viz::scene3d::compositor::manager::oit::ensure_fp_created(
                new_name,
                algo_name,
                algo_pass_name,
                fp_base_name,
                fp_source_name
            );
            pass->setFragmentProgram(new_name);

            std::string input_buffer;
            if(algo_pass_name == passes::PEEL_PING)
            {
                input_buffer = buffers::PONG;
            }
            else if(algo_pass_name == passes::PEEL_PONG)
            {
                input_buffer = buffers::PING;
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

//-----------------------------------------------------------------------------

std::string oit::setup_transparency(
    Ogre::Viewport* _viewport,
    compositor::transparency_technique _technique,
    int _depth
)
{
    static const std::map<compositor::transparency_technique, std::string> s_COMPOSITOR_NAME =
    {
        {cellshading_depthpeeling, algo::CEL_SHADING},
        {depthpeeling, algo::DEPTH_PEELING},
        {dualdepthpeeling, algo::DUAL_DEPTH_PEELING},
        {weightedblendedoit, algo::WEIGHTED_BLENDED},
        {hybridtransparency, algo::HYBRID}
    };
    const std::string& core_compositor_name = s_COMPOSITOR_NAME.at(_technique);

    // Check if compositor already exists
    Ogre::CompositorChain* comp_chain = Ogre::CompositorManager::getSingleton().getCompositorChain(_viewport);

    const auto& comp_instances = comp_chain->getCompositorInstances();

    Ogre::CompositorInstance* compositor_instance = nullptr;

    for(auto* target_comp : comp_instances)
    {
        if(target_comp->getCompositor()->getName() == core_compositor_name)
        {
            compositor_instance = target_comp;
            break;
        }
    }

    // If we didn't retrieve the good compositor
    if(compositor_instance == nullptr)
    {
        static const std::string s_FINAL_CHAIN_COMPOSITOR = "FinalChainCompositor";

        Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();
        bool need_final_compositor_swap(false);

        // If the compositor chain already contains the final compositor, we have to remove it
        Ogre::CompositorChain* chain = compositor_manager.getCompositorChain(_viewport);
        if(const size_t pos = chain->getCompositorPosition(s_FINAL_CHAIN_COMPOSITOR);
           pos != Ogre::CompositorChain::NPOS)
        {
            chain->setCompositorEnabled(pos, false);
            chain->removeCompositor(pos);
            need_final_compositor_swap = true;
        }

        // Now, we can add the new compositor to the compositor chain
        compositor_instance = compositor_manager.addCompositor(_viewport, core_compositor_name, 0);
        compositor_manager.setCompositorEnabled(_viewport, core_compositor_name, true);

        // If the final compositor has been removed, we need to add it to the compositor chain
        if(need_final_compositor_swap)
        {
            compositor_manager.addCompositor(_viewport, s_FINAL_CHAIN_COMPOSITOR);
            compositor_manager.setCompositorEnabled(_viewport, s_FINAL_CHAIN_COMPOSITOR, true);
        }

        if(compositor_instance == nullptr)
        {
            SIGHT_ERROR(
                "Compositor " + core_compositor_name
                + " script is missing in resources (check your resources' paths)"
            );
        }
    }

    Ogre::CompositionTechnique* comp_tech = compositor_instance->getTechnique();
    SIGHT_ASSERT("Compositor technique null", comp_tech);
    switch(_technique)
    {
        case cellshading_depthpeeling:
        case depthpeeling:
            setup_depth_peeling(comp_tech, _depth, core_compositor_name);
            break;

        case dualdepthpeeling:
            setup_dual_depth_peeling(comp_tech, _depth);
            break;

        case hybridtransparency:
            setup_hybrid_transparency(comp_tech, _depth);
            break;

        case weightedblendedoit:
        case DEFAULT:
            break;
    }

    Ogre::CompositorManager::getSingleton().setCompositorEnabled(_viewport, core_compositor_name, true);

    return core_compositor_name;
}

//-----------------------------------------------------------------------------

void oit::setup_depth_peeling(
    Ogre::CompositionTechnique* _compositor_tech,
    int _depth,
    const std::string& _core_compositor_name
)
{
    // Check if depthpeeling technique already exists
    const int num_of_target_pass = static_cast<int>(_compositor_tech->getTargetPasses().size());

    // 4 is the first ping pong target
    const int first_ping_pong_target = 4;
    for(int i = first_ping_pong_target ; i < num_of_target_pass ; i++)
    {
        // When a target is discarded, the next one becomes the current one (eg : if I remove the 2nd target,
        // the 3rd target becomes the new 2nd target)
        _compositor_tech->removeTargetPass(first_ping_pong_target);
    }

    // Ping pong peel and blend
    for(int i = 0 ; i < _depth ; i++)
    {
        const std::string ping_pong_buffer = buffers::ping_pong(i);

        // Peel buffer
        {
            Ogre::CompositionTargetPass* dp_comp_target_peel = _compositor_tech->createTargetPass();
            dp_comp_target_peel->setOutputName(ping_pong_buffer);

            // No previous input
            dp_comp_target_peel->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

            // Clear pass
            {
                Ogre::CompositionPass* dp_comp_pass_clear = dp_comp_target_peel->createPass();
                dp_comp_pass_clear->setType(Ogre::CompositionPass::PT_CLEAR);
            }

            // Material scheme
            dp_comp_target_peel->setMaterialScheme(join(_core_compositor_name, passes::ping_pong(i)));

            // No shadow
            dp_comp_target_peel->setShadowsEnabled(false);

            // Render scene pass
            {
                Ogre::CompositionPass* dp_comp_pass_render_scene = dp_comp_target_peel->createPass();
                dp_comp_pass_render_scene->setType(Ogre::CompositionPass::PT_RENDERSCENE);
                dp_comp_pass_render_scene->setLastRenderQueue(rq::SURFACE_ID);
            }
        }

        // Blend buffer
        {
            Ogre::CompositionTargetPass* dp_comp_target_blend = _compositor_tech->createTargetPass();
            dp_comp_target_blend->setOutputName(buffers::GBUFFER);

            // No previous input
            dp_comp_target_blend->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

            // Render quad
            {
                Ogre::CompositionPass* dp_comp_pass_render_quad = dp_comp_target_blend->createPass();
                dp_comp_pass_render_quad->setType(Ogre::CompositionPass::PT_RENDERQUAD);
                dp_comp_pass_render_quad->setMaterialName(join(_core_compositor_name, passes::BLEND));
                dp_comp_pass_render_quad->setInput(0, ping_pong_buffer, 0);
                if(dp_comp_pass_render_quad->getNumInputs() == 3)
                {
                    dp_comp_pass_render_quad->setInput(1, ping_pong_buffer, 1);
                    dp_comp_pass_render_quad->setInput(2, ping_pong_buffer, 2);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void oit::setup_dual_depth_peeling(
    Ogre::CompositionTechnique* _compositor_tech,
    int _depth
)
{
    // Check if depthpeeling technique already exists
    const int num_of_target_pass = static_cast<int>(_compositor_tech->getTargetPasses().size());

    // 2 is the first ping pong target
    const int first_ping_pong_target = 2;
    for(int i = first_ping_pong_target ; i < num_of_target_pass ; i++)
    {
        // When a target is discarded, the next one becomes the current one (eg : if I remove the 2nd target,
        // the 3rd target becomes the new 2nd target)
        _compositor_tech->removeTargetPass(first_ping_pong_target);
    }

    // Ping pong peel and blend
    for(int i = 0 ; i < _depth ; i++)
    {
        const std::string ping_pong_buffer = buffers::ping_pong(i);

        // Peel buffer
        {
            Ogre::CompositionTargetPass* dp_comp_target_peel = _compositor_tech->createTargetPass();
            dp_comp_target_peel->setOutputName(ping_pong_buffer);

            // No previous input
            dp_comp_target_peel->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

            // Clear pass
            {
                Ogre::CompositionPass* dp_comp_pass_clear = dp_comp_target_peel->createPass();
                dp_comp_pass_clear->setType(Ogre::CompositionPass::PT_CLEAR);
                dp_comp_pass_clear->setClearColour(Ogre::ColourValue(-1.F, 0.F, 0.F, 0.F));
            }

            // Material scheme
            dp_comp_target_peel->setMaterialScheme(join(algo::DUAL_DEPTH_PEELING, passes::ping_pong(i)));

            // No shadow
            dp_comp_target_peel->setShadowsEnabled(false);

            // Render scene pass
            {
                Ogre::CompositionPass* dp_comp_pass_render_scene = dp_comp_target_peel->createPass();
                dp_comp_pass_render_scene->setType(Ogre::CompositionPass::PT_RENDERSCENE);
                dp_comp_pass_render_scene->setLastRenderQueue(rq::SURFACE_ID);
            }
        }

        // Blend buffer
        {
            Ogre::CompositionTargetPass* dp_comp_target_blend = _compositor_tech->createTargetPass();
            dp_comp_target_blend->setOutputName(buffers::GBUFFER);

            // No previous input
            dp_comp_target_blend->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

            // Render quad
            {
                Ogre::CompositionPass* dp_comp_pass_render_quad = dp_comp_target_blend->createPass();
                dp_comp_pass_render_quad->setType(Ogre::CompositionPass::PT_RENDERQUAD);
                dp_comp_pass_render_quad->setMaterialName(join(algo::DUAL_DEPTH_PEELING, passes::BLEND));
                dp_comp_pass_render_quad->setInput(0, ping_pong_buffer, 3);
                dp_comp_pass_render_quad->setInput(1, ping_pong_buffer, 5);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void oit::setup_hybrid_transparency(
    Ogre::CompositionTechnique* _compositor_tech,
    int _depth
)
{
    // Check if hybrid transparency technique already exists
    const int num_of_target_pass = static_cast<int>(_compositor_tech->getTargetPasses().size());

    // 3 is the first ping pong target
    const int first_ping_pong_target = 4;

    for(int i = first_ping_pong_target ; i < num_of_target_pass ; i++)
    {
        // When a target is discarded, the next one becomes the current one (eg : if I remove the 2nd target,
        // the 3rd target becomes the new 2nd target)
        _compositor_tech->removeTargetPass(first_ping_pong_target);
    }

    // Ping pong peel and blend
    for(int i = 0 ; i < (_depth / 2) * 2 ; i++)
    {
        const std::string ping_pong_buffer = buffers::ping_pong(i);

        // Peel buffer
        {
            Ogre::CompositionTargetPass* dp_comp_target_peel = _compositor_tech->createTargetPass();
            dp_comp_target_peel->setOutputName(ping_pong_buffer);

            // No previous input
            dp_comp_target_peel->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

            // Clear pass
            {
                Ogre::CompositionPass* dp_comp_pass_clear = dp_comp_target_peel->createPass();
                dp_comp_pass_clear->setType(Ogre::CompositionPass::PT_CLEAR);
            }

            // Material scheme
            dp_comp_target_peel->setMaterialScheme(join(algo::HYBRID, passes::ping_pong(i)));

            // No shadow
            dp_comp_target_peel->setShadowsEnabled(false);

            // Render scene pass
            {
                Ogre::CompositionPass* dp_comp_pass_render_scene = dp_comp_target_peel->createPass();
                dp_comp_pass_render_scene->setType(Ogre::CompositionPass::PT_RENDERSCENE);
                dp_comp_pass_render_scene->setLastRenderQueue(rq::SURFACE_ID);
            }
        }

        // Blend buffer
        {
            Ogre::CompositionTargetPass* dp_comp_target_blend = _compositor_tech->createTargetPass();
            dp_comp_target_blend->setOutputName(buffers::GBUFFER);

            // No previous input
            dp_comp_target_blend->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

            // Render quad
            {
                Ogre::CompositionPass* dp_comp_pass_render_quad = dp_comp_target_blend->createPass();
                dp_comp_pass_render_quad->setType(Ogre::CompositionPass::PT_RENDERQUAD);
                dp_comp_pass_render_quad->setMaterialName(join(algo::DEPTH_PEELING, passes::BLEND));
                dp_comp_pass_render_quad->setInput(0, ping_pong_buffer, 0);
            }
        }
    }

    // Occlusion buffer
    {
        Ogre::CompositionTargetPass* dp_comp_target_occlusion = _compositor_tech->createTargetPass();
        dp_comp_target_occlusion->setOutputName(buffers::OCCLUSION);

        // No previous input
        dp_comp_target_occlusion->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

        // Clear pass
        {
            Ogre::CompositionPass* dp_comp_pass_clear = dp_comp_target_occlusion->createPass();
            dp_comp_pass_clear->setType(Ogre::CompositionPass::PT_CLEAR);
            dp_comp_pass_clear->setClearColour(Ogre::ColourValue(1.F, 1.F, 1.F, 1.F));
        }

        // Material scheme
        dp_comp_target_occlusion->setMaterialScheme(join(algo::HYBRID, passes::OCCLUSION_MAP));

        // No shadow
        dp_comp_target_occlusion->setShadowsEnabled(false);

        // Render scene pass
        {
            Ogre::CompositionPass* dp_comp_pass_render_scene = dp_comp_target_occlusion->createPass();
            dp_comp_pass_render_scene->setType(Ogre::CompositionPass::PT_RENDERSCENE);
            dp_comp_pass_render_scene->setLastRenderQueue(rq::SURFACE_ID);
        }
    }

    // Weight blend buffer
    {
        Ogre::CompositionTargetPass* dp_comp_target_weight_blend = _compositor_tech->createTargetPass();
        dp_comp_target_weight_blend->setOutputName(buffers::WEIGHTED_COLOR);

        // No previous input
        dp_comp_target_weight_blend->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

        // Clear pass
        {
            Ogre::CompositionPass* dp_comp_pass_clear = dp_comp_target_weight_blend->createPass();
            dp_comp_pass_clear->setType(Ogre::CompositionPass::PT_CLEAR);
            dp_comp_pass_clear->setClearBuffers(Ogre::FBT_COLOUR);
        }

        // Material scheme
        dp_comp_target_weight_blend->setMaterialScheme(join(algo::HYBRID, passes::WEIGHT_BLEND));

        // No shadow
        dp_comp_target_weight_blend->setShadowsEnabled(false);

        // Render scene pass
        {
            Ogre::CompositionPass* dp_comp_pass_render_scene = dp_comp_target_weight_blend->createPass();
            dp_comp_pass_render_scene->setType(Ogre::CompositionPass::PT_RENDERSCENE);
            dp_comp_pass_render_scene->setLastRenderQueue(rq::SURFACE_ID);
        }
    }

    // Transmittance blend buffer
    {
        Ogre::CompositionTargetPass* dp_comp_target_transmittance = _compositor_tech->createTargetPass();
        dp_comp_target_transmittance->setOutputName(buffers::TRANSMITTANCE);

        // No previous input
        dp_comp_target_transmittance->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

        // Clear pass
        {
            Ogre::CompositionPass* dp_comp_pass_clear = dp_comp_target_transmittance->createPass();
            dp_comp_pass_clear->setType(Ogre::CompositionPass::PT_CLEAR);
            dp_comp_pass_clear->setClearColour(Ogre::ColourValue(1.F, 1.F, 1.F, 1.F));
        }

        // Material scheme
        dp_comp_target_transmittance->setMaterialScheme(join(algo::HYBRID, passes::TRANSMITTANCE_BLEND));

        // No shadow
        dp_comp_target_transmittance->setShadowsEnabled(false);

        // Render scene pass
        {
            Ogre::CompositionPass* dp_comp_pass_render_scene = dp_comp_target_transmittance->createPass();
            dp_comp_pass_render_scene->setType(Ogre::CompositionPass::PT_RENDERSCENE);
            dp_comp_pass_render_scene->setLastRenderQueue(rq::SURFACE_ID);
        }
    }

    // WBOIT blend buffer
    {
        Ogre::CompositionTargetPass* dp_comp_target_wboit_blend = _compositor_tech->createTargetPass();
        dp_comp_target_wboit_blend->setOutputName(buffers::WBOIT_OUTPUT);

        // No previous input
        dp_comp_target_wboit_blend->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

        // Render quad
        {
            Ogre::CompositionPass* dp_comp_pass_render_quad = dp_comp_target_wboit_blend->createPass();
            dp_comp_pass_render_quad->setType(Ogre::CompositionPass::PT_RENDERQUAD);
            dp_comp_pass_render_quad->setMaterialName(join(algo::HYBRID, "WBOITBlend"));
            dp_comp_pass_render_quad->setInput(0, buffers::WEIGHTED_COLOR);
            dp_comp_pass_render_quad->setInput(1, buffers::TRANSMITTANCE);
        }
    }

    // Blend final (Depth Peeling + WBOIT) buffer
    {
        Ogre::CompositionTargetPass* dp_comp_target_final_blend = _compositor_tech->createTargetPass();
        dp_comp_target_final_blend->setOutputName(buffers::GBUFFER);

        // No previous input
        dp_comp_target_final_blend->setInputMode(Ogre::CompositionTargetPass::IM_NONE);

        // Render quad
        {
            Ogre::CompositionPass* dp_comp_pass_render_quad = dp_comp_target_final_blend->createPass();
            dp_comp_pass_render_quad->setType(Ogre::CompositionPass::PT_RENDERQUAD);
            dp_comp_pass_render_quad->setMaterialName(join(algo::HYBRID, "BlendFinal"));
            dp_comp_pass_render_quad->setInput(0, buffers::WBOIT_OUTPUT);
        }
    }
}

// ----------------------------------------------------------------------------

Ogre::GpuProgramPtr oit::ensure_fp_created(
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
    if(_algo_name == algo::DEPTH_PEELING || _algo_name == algo::CEL_SHADING)
    {
        parameters.emplace_back("preprocessor_defines", "DEPTH_PEELING=1");
    }
    else if(_algo_name == algo::DUAL_DEPTH_PEELING)
    {
        parameters.emplace_back("preprocessor_defines", "DUAL_DEPTH_PEELING=1");
    }
    else if(_algo_name == algo::HYBRID)
    {
        if(_algo_pass_name == passes::PEEL || _algo_pass_name == passes::PEEL_INIT)
        {
            parameters.emplace_back("preprocessor_defines", "DEPTH_PEELING=1");
        }
        else
        {
            if(_algo_pass_name == passes::TRANSMITTANCE_BLEND)
            {
                parameters.emplace_back("preprocessor_defines", "HYBRID=1,WBOIT_TRANSMIT=1");
            }
            else if(_algo_pass_name == passes::OCCLUSION_MAP)
            {
                parameters.emplace_back("preprocessor_defines", "HYBRID=1,WBOIT_OCCLUSION=1");
            }
            else
            {
                parameters.emplace_back("preprocessor_defines", "HYBRID=1,WBOIT=1");
            }
        }
    }
    else if(_algo_name == algo::WEIGHTED_BLENDED)
    {
        if(_algo_pass_name == passes::TRANSMITTANCE_BLEND)
        {
            parameters.emplace_back("preprocessor_defines", "WBOIT_TRANSMIT=1");
        }
        else if(_algo_pass_name == passes::OCCLUSION_MAP)
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

} // namespace sight::viz::scene3d::compositor::manager
