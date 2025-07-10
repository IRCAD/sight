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

#include "viz/scene3d/material/r2vb.hpp"

#include "viz/scene3d/helper/shading.hpp"

#include <viz/scene3d/layer.hpp>
#include <viz/scene3d/mesh.hpp>
#include <viz/scene3d/ogre.hpp>

#include <OgreTechnique.h>
#include <OgreTextureManager.h>

namespace sight::viz::scene3d::material
{

//------------------------------------------------------------------------------

r2vb::r2vb(const std::string& _name, const std::string& _template_name) :
    generic(_name, _template_name)
{
}

//------------------------------------------------------------------------------

void r2vb::set_shading(data::material::shading_t _shading_mode)
{
    // If we have UVs with R2VB, then for the sake of simplicity we generate UVs whatever the material is
    // Otherwise we would have to update the whole R2VB pipeline each time a texture is set/unset
    const bool need_diffuse_texture = data::mesh::has<data::mesh::attribute::point_tex_coords>(m_layout);

    const auto mode = _shading_mode;

    const Ogre::String permutation = viz::scene3d::helper::shading::get_permutation(
        mode,
        need_diffuse_texture,
        data::mesh::has<data::mesh::attribute::point_colors>(m_layout)
    );
    const Ogre::String r2vb_gs_name = viz::scene3d::helper::shading::get_r2vb_geometry_program_name(
        m_primitive_type,
        need_diffuse_texture,
        data::mesh::has<data::mesh::attribute::point_colors>(m_layout),
        data::mesh::has<data::mesh::attribute::cell_colors>(m_layout)
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
               || ogre_pass->getName() == material::generic::passes::NORMALS
               || ogre_pass->getName() == material::generic::passes::SELECTED)
            {
                // TODO_FB: useful ?
                continue;
            }

            // We need a geometry shader (primitive generation and per-primitive color)
            // and thus we rely on the render to vertex buffer pipeline

            ogre_pass->setVertexProgram("R2VB/" + permutation + "_VP");
            ogre_pass->setGeometryProgram(r2vb_gs_name);
            ogre_pass->setFragmentProgram("");

            if(data::mesh::has<data::mesh::attribute::cell_colors>(m_layout))
            {
                const auto result = Ogre::TextureManager::getSingleton().createOrRetrieve(
                    m_per_primitive_color_texture_name,
                    viz::scene3d::RESOURCE_GROUP,
                    true
                );

                SIGHT_ASSERT("texture should have been created before in mesh", !result.second);

                const Ogre::TexturePtr tex = Ogre::dynamic_pointer_cast<Ogre::Texture>(result.first);

                const std::string tex_unit_name        = "PerPrimitiveColor";
                Ogre::TextureUnitState* tex_unit_state = ogre_pass->getTextureUnitState(tex_unit_name);

                if(tex_unit_state == nullptr)
                {
                    SIGHT_DEBUG("create unit state: " << m_per_primitive_color_texture_name);

                    tex_unit_state = ogre_pass->createTextureUnitState();
                    tex_unit_state->setName(tex_unit_name);
                    tex_unit_state->setTexture(tex);
                    tex_unit_state->setTextureFiltering(Ogre::TFO_NONE);
                    tex_unit_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

                    const auto unit_state_count = ogre_pass->getNumTextureUnitStates();

                    // Unit state is set to 10 in the material file, but the real index is set here
                    // Ogre packs texture unit indices so we can't use spare indices
                    ogre_pass->getGeometryProgramParameters()->setNamedConstant(
                        "u_colorPrimitiveTexture",
                        int(unit_state_count - 1)
                    );
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void r2vb::set_layout(const viz::scene3d::mesh& _mesh)
{
    m_layout                           = _mesh.layout();
    m_per_primitive_color_texture_name = _mesh.per_primitive_color_texture_name();
}

} // namespace sight::viz::scene3d::material
