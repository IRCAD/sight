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

#include "viz/scene3d/material/standard.hpp"

#include "viz/scene3d/helper/shading.hpp"

#include <viz/scene3d/layer.hpp>
#include <viz/scene3d/mesh.hpp>

#include <OgreTechnique.h>

namespace sight::viz::scene3d::material
{

//------------------------------------------------------------------------------

standard::standard(const std::string& _name) :
    generic(_name, standard::TEMPLATE)
{
}

//------------------------------------------------------------------------------

void standard::update_options_mode(int _options_mode)
{
    // Clean everything, thus we don't need to filter out OIT generated passes
    this->clean_generated_techniques();

    // First remove the normals pass if there is already one
    this->remove_pass(material::generic::passes::NORMALS);

    const Ogre::Material::Techniques& techniques = m_material->getTechniques();

    if(_options_mode != data::material::standard)
    {
        for(auto* const current_technique : techniques)
        {
            // We need the first pass of the current technique in order to copy its rendering states in the normals pass
            Ogre::Pass* first_pass = current_technique->getPass(0);
            SIGHT_ASSERT("Pass is null", first_pass);

            const bool depth_only = current_technique->getName() == "depth";

            // We copy the first pass, thus keeping all rendering states
            Ogre::Pass* normals_pass = current_technique->createPass();
            *normals_pass = *first_pass;
            normals_pass->setName(material::generic::passes::NORMALS);

            // Vertex shader
            normals_pass->setVertexProgram("Normals_VP");

            std::string gp_name = depth_only ? "DepthPeeling/depthMap/" : "";
            gp_name += (_options_mode == data::material::normals)
                       ? "VerticesNormals_GP"
                       : "CellsNormals_GP";

            normals_pass->setGeometryProgram(gp_name);

            if(!depth_only)
            {
                std::string fp_name = normals_pass->getFragmentProgramName();
                fp_name = viz::scene3d::helper::shading::set_permutation_in_program_name(fp_name, "Edge_Normal");
                normals_pass->setFragmentProgram(fp_name);
            }

            // Updates the normal length according to the bounding box's size
            normals_pass->getGeometryProgramParameters()->setNamedConstant("u_sceneSize", m_mesh_size);
        }
    }
}

//------------------------------------------------------------------------------

void standard::set_shading(
    data::material::shading_t _shading_mode,
    int _num_lights,
    bool _has_diffuse_texture,
    bool _use_texture_alpha
)
{
    const bool need_diffuse_texture = data::mesh::has<data::mesh::attribute::point_tex_coords>(m_layout)
                                      && _has_diffuse_texture;

    const auto mode = _shading_mode;

    const Ogre::String permutation = viz::scene3d::helper::shading::get_permutation(
        mode,
        need_diffuse_texture,
        data::mesh::has<data::mesh::attribute::point_colors>(m_layout)
    );

    this->clean_generated_techniques();

    // Iterate through each technique found in the material and switch the shading mode
    const Ogre::Material::Techniques& techniques = m_material->getTechniques();
    for(auto* const tech : techniques)
    {
        SIGHT_ASSERT("technique is not set", tech);

        const Ogre::Technique::Passes& passes = tech->getPasses();

        for(auto* const ogre_pass : passes)
        {
            // Nothing to do for edge and normal passes
            if(ogre_pass->getName() == material::generic::passes::EDGES
               || ogre_pass->getName() == material::generic::passes::NORMALS)
            {
                continue;
            }

            // "Regular" pipeline
            std::string vp_name = ogre_pass->getVertexProgramName();
            vp_name = viz::scene3d::helper::shading::set_permutation_in_program_name(vp_name, permutation);
            ogre_pass->setVertexProgram(vp_name);

            std::string fp_name = ogre_pass->getFragmentProgramName();
            fp_name = viz::scene3d::helper::shading::set_permutation_in_program_name(fp_name, permutation);
            ogre_pass->setFragmentProgram(fp_name);

            Ogre::GpuProgramParametersSharedPtr vp = ogre_pass->getVertexProgramParameters();

            if(vp->_findNamedConstantDefinition("u_fNumLights") != nullptr)
            {
                vp->setNamedConstant("u_fNumLights", static_cast<float>(_num_lights));
            }

            if(ogre_pass->hasFragmentProgram())
            {
                Ogre::GpuProgramParametersSharedPtr fp = ogre_pass->getFragmentProgramParameters();

                if(fp->_findNamedConstantDefinition("u_fNumLights") != nullptr)
                {
                    fp->setNamedConstant("u_fNumLights", static_cast<float>(_num_lights));
                }

                // Updates the u_hasTextureAlpha flag uniform according to the configuration of the texture adaptor
                if(need_diffuse_texture)
                {
                    if(fp->_findNamedConstantDefinition("u_useTextureAlpha") != nullptr)
                    {
                        int use_texture_alpha = static_cast<int>(_use_texture_alpha);
                        fp->setNamedConstant("u_useTextureAlpha", use_texture_alpha);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void standard::set_layout(const viz::scene3d::mesh& _mesh)
{
    auto bbox = _mesh.bounds();
    m_mesh_size = bbox.getSize().length();

    generic::set_layout(_mesh);
}

} // namespace sight::viz::scene3d::material
