/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "viz/scene3d/compositor/listener/auto_stereo.hpp"

#include "viz/scene3d/helper/camera.hpp"
#include "viz/scene3d/helper/shading.hpp"
#include "viz/scene3d/helper/technique.hpp"

#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreGpuProgramManager.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMatrix4.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreTechnique.h>

namespace sight::viz::scene3d::compositor::listener
{

//-----------------------------------------------------------------------------

auto_stereo_compositor_listener::auto_stereo_compositor_listener(std::uint8_t _viewpoint_number) :
    m_viewpoint_number(_viewpoint_number)
{
}

//-----------------------------------------------------------------------------

auto_stereo_compositor_listener::~auto_stereo_compositor_listener()
{
    auto& mtl_manager = Ogre::MaterialManager::getSingleton();
    // We need to clean the VR techniques because we want to set the correct textures
    // Cleaning the texture forces the listener to be triggered and then to create the techniques with the new textures
    for(auto& tech_mat_pair : m_created_techniques)
    {
        Ogre::MaterialPtr mtl = mtl_manager.getByName(tech_mat_pair.second, RESOURCE_GROUP);

        if(mtl == nullptr)
        {
            // The material is already deleted and so is the technique.
            continue;
        }

        Ogre::Technique* tech = tech_mat_pair.first;

        const Ogre::Material::Techniques techniques = mtl->getTechniques();

        std::vector<std::uint16_t> remove_technique_vector;

        std::uint16_t index = 0;
        for(auto* const technique : techniques)
        {
            if(tech == technique)
            {
                remove_technique_vector.push_back(index);
            }

            ++index;
        }

        // Remove in inverse order otherwise the index we stored becomes invalid ;-)
        std::for_each(
            remove_technique_vector.rbegin(),
            remove_technique_vector.rend(),
            [mtl](auto _tech_id)
            {
                mtl->removeTechnique(_tech_id);
            });
    }
}

//------------------------------------------------------------------------------

Ogre::Technique* auto_stereo_compositor_listener::handleSchemeNotFound(
    std::uint16_t /*_schemeIndex*/,
    const Ogre::String& _scheme_name,
    Ogre::Material* _original_material,
    std::uint16_t /*_lodIndex*/,
    const Ogre::Renderable* /*_renderable*/
)
{
    Ogre::Technique* new_tech = nullptr;
    if(_scheme_name.find("auto_stereo") != std::string::npos)
    {
        const std::string pass_id_str = _scheme_name.substr(_scheme_name.size() - 1);

        Ogre::Technique* matching_tech = nullptr;

        if(Ogre::StringUtil::startsWith(_scheme_name, "VolumeEntries"))
        {
            // Volume entries technique names follow this pattern : VolumeEntries<auto_stereo>_<technique><viewport>
            const std::size_t tech_name_pos  = _scheme_name.find('_') + 1;
            const std::size_t tech_name_size = _scheme_name.size() - 1 - tech_name_pos;
            const std::string tech_name      = _scheme_name.substr(tech_name_pos, tech_name_size);

            auto entry_points_mtl = Ogre::MaterialManager::getSingleton().getByName("RayEntryPoints", RESOURCE_GROUP);

            matching_tech = entry_points_mtl->getTechnique(tech_name);
        }
        else
        {
            matching_tech = _original_material->getTechnique(0);
        }

        new_tech = viz::scene3d::helper::technique::copy_to_material(matching_tech, _scheme_name, _original_material);

        auto* const pass = new_tech->getPass(0);
        {
            const auto vp_base_name        = pass->getVertexProgramName();
            const auto vp_source_file_name = pass->getVertexProgram()->getSourceFile();
            const auto vp_new_name         = vp_base_name + "+auto_stereo";

            viz::scene3d::helper::shading::gpu_program_parameters_t parameters;
            parameters.emplace_back("preprocessor_defines", "AUTOSTEREO=1");

            viz::scene3d::helper::shading::create_program_from(
                vp_new_name,
                vp_source_file_name,
                parameters,
                Ogre::GPT_VERTEX_PROGRAM,
                vp_base_name
            );

            pass->setVertexProgram(vp_new_name);
            auto vp_params = pass->getVertexProgramParameters();

            // We use a shared parameters block to upload the custom projection matrices
            const std::string proj_param_name = "ProjectionMatrixParam/" + pass_id_str;

            auto& gpu_program_mgr            = Ogre::GpuProgramManager::getSingleton();
            const auto& shared_parameter_map = gpu_program_mgr.getAvailableSharedParameters();

            if(shared_parameter_map.find(proj_param_name) == shared_parameter_map.end())
            {
                auto proj_param = Ogre::GpuProgramManager::getSingleton().createSharedParameters(proj_param_name);

                // define the shared param structure
                proj_param->addConstantDefinition("u_proj", Ogre::GCT_MATRIX_4X4);
            }

            vp_params->addSharedParameters(proj_param_name);
            vp_params->setNamedAutoConstant("u_worldView", Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
        }

        const auto fp_base_name = pass->getFragmentProgramName();
        if(Ogre::StringUtil::startsWith(fp_base_name, "RTV_FP"))
        {
            const auto fp_source_file_name = pass->getFragmentProgram()->getSourceFile();
            const auto fp_new_name         = fp_base_name + "+auto_stereo";

            viz::scene3d::helper::shading::gpu_program_parameters_t parameters;
            parameters.emplace_back("preprocessor_defines", "AUTOSTEREO=1");

            viz::scene3d::helper::shading::create_program_from(
                fp_new_name,
                fp_source_file_name,
                parameters,
                Ogre::GPT_FRAGMENT_PROGRAM,
                fp_base_name
            );

            pass->setFragmentProgram(fp_new_name);
            auto fp_params = pass->getFragmentProgramParameters();

            // We use a shared parameters block to upload the custom projection matrices
            const std::string inv_proj_param_name = "InverseProjectionMatrixParam/" + pass_id_str;
            const std::string proj_param_name     = "ProjectionMatrixParam/" + pass_id_str;

            auto& gpu_program_mgr            = Ogre::GpuProgramManager::getSingleton();
            const auto& shared_parameter_map = gpu_program_mgr.getAvailableSharedParameters();

            if(shared_parameter_map.find(proj_param_name) == shared_parameter_map.end())
            {
                auto params = Ogre::GpuProgramManager::getSingleton().createSharedParameters(proj_param_name);
                params->addConstantDefinition("u_proj", Ogre::GCT_MATRIX_4X4);
            }

            if(shared_parameter_map.find(inv_proj_param_name) == shared_parameter_map.end())
            {
                auto params = Ogre::GpuProgramManager::getSingleton().createSharedParameters(inv_proj_param_name);
                params->addConstantDefinition("u_invProj", Ogre::GCT_MATRIX_4X4);
            }

            fp_params->addSharedParameters(proj_param_name);
            fp_params->addSharedParameters(inv_proj_param_name);
            fp_params->setNamedAutoConstant("u_worldView", Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
            fp_params->setNamedAutoConstant(
                "u_invWorldView",
                Ogre::GpuProgramParameters::ACT_INVERSE_WORLDVIEW_MATRIX
            );

            Ogre::TextureUnitState* tex_unit_state = pass->getTextureUnitState("entryPoints");

            SIGHT_ASSERT("No texture named 'entryPoints' in " + _original_material->getName(), tex_unit_state);
            tex_unit_state->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);

            const auto comp_name = "VolumeEntries" + std::to_string(m_viewpoint_number);
            tex_unit_state->setCompositorReference(comp_name, std::string(comp_name) + "texture" + pass_id_str);
        }

        m_created_techniques.emplace_back(new_tech, _original_material->getName());
    }

    return new_tech;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::compositor::listener
