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

#include "viz/scene3d/material/generic.hpp"

#include "viz/scene3d/helper/shading.hpp"

#include <viz/scene3d/layer.hpp>
#include <viz/scene3d/mesh.hpp>
#include <viz/scene3d/ogre.hpp>

#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>

namespace sight::viz::scene3d::material
{

//------------------------------------------------------------------------------

generic::generic(const std::string& _name, const std::string& _template_name) :
    m_material(Ogre::MaterialManager::getSingleton().create(_name, viz::scene3d::RESOURCE_GROUP)),
    m_template_name(_template_name)
{
    const Ogre::MaterialPtr ogre_material = Ogre::MaterialManager::getSingleton().getByName(
        _template_name,
        RESOURCE_GROUP
    );

    SIGHT_ASSERT("Material '" + _template_name + "'' not found", ogre_material);

    // Then we copy these parameters in m_material.
    // We can now alter this new instance without changing the default material
    ogre_material.get()->copyDetailsTo(m_material);

    // Update lighting values with material data defaults
    const auto ambient_default = data::material::defaults::AMBIENT;
    const Ogre::ColourValue ambient(ambient_default[0], ambient_default[1], ambient_default[2], ambient_default[3]);
    m_material->setAmbient(ambient);

    const auto diffuse_default = data::material::defaults::DIFFUSE;
    const Ogre::ColourValue diffuse(diffuse_default[0], diffuse_default[1], diffuse_default[2], diffuse_default[3]);
    m_material->setDiffuse(diffuse);

    const Ogre::ColourValue specular(.2F, .2F, .2F, 1.F);
    m_material->setSpecular(specular);
    m_material->setShininess(25);
}

//------------------------------------------------------------------------------

generic::~generic()
{
    auto material_name = m_material->getName();
    m_material.reset();
    Ogre::MaterialManager::getSingleton().remove(material_name, sight::viz::scene3d::RESOURCE_GROUP);
}

//------------------------------------------------------------------------------

void generic::set_layout(const viz::scene3d::mesh& _mesh)
{
    m_layout = _mesh.layout();

    // The r2vb pipeline outputs per-vertex color if we have per-primitive color
    // Thus for the rendering pipeline it is only viewed as per-vertex color
    m_layout = static_cast<bool>(_mesh.layout() & data::mesh::attribute::cell_colors)
               ? m_layout | data::mesh::attribute::point_colors
               : m_layout;
    m_layout = m_layout & ~data::mesh::attribute::cell_colors;
}

//------------------------------------------------------------------------------

void generic::set_polygon_mode(sight::data::material::representation_t _polygon_mode)
{
    // This is necessary to load and compile the material, otherwise the following technique iterator
    // is null when we call this method on the first time (from starting() for instance)
    m_material->touch();

    this->clean_generated_techniques();

    // First remove a previous normal pass if it exists
    this->remove_pass(material::generic::passes::EDGES);

    const Ogre::Material::Techniques& techniques = m_material->getTechniques();

    if(_polygon_mode == data::material::edge)
    {
        for(auto* const tech : techniques)
        {
            SIGHT_ASSERT("technique is not set", tech);

            Ogre::Pass* first_pass = tech->getPass(0);
            SIGHT_ASSERT("No pass found", first_pass);

            first_pass->setPolygonMode(Ogre::PM_SOLID);
            first_pass->setPointSpritesEnabled(false);

            Ogre::Pass* edge_pass = tech->getPass(material::generic::passes::EDGES);
            if(edge_pass == nullptr)
            {
                // We copy the first pass, thus keeping all rendering states
                edge_pass  = tech->createPass();
                *edge_pass = *first_pass;
                edge_pass->setName(material::generic::passes::EDGES);

                // Then we switch the vertex shader...
                edge_pass->setVertexProgram("Edge_VP");

                // ... and the fragment shader
                std::string fp_name = edge_pass->getFragmentProgramName();
                fp_name = viz::scene3d::helper::shading::set_permutation_in_program_name(fp_name, "Edge_Normal");
                edge_pass->setFragmentProgram(fp_name);

                edge_pass->setPolygonMode(Ogre::PM_WIREFRAME);
            }
        }
    }
    else
    {
        for(auto* const tech : techniques)
        {
            SIGHT_ASSERT("technique is not set", tech);

            const Ogre::Technique::Passes& passes = tech->getPasses();

            for(auto* const ogre_pass : passes)
            {
                switch(_polygon_mode)
                {
                    case data::material::surface:
                        ogre_pass->setPolygonMode(Ogre::PM_SOLID);
                        ogre_pass->setPointSpritesEnabled(false);
                        break;

                    case data::material::wireframe:
                        ogre_pass->setPolygonMode(Ogre::PM_WIREFRAME);
                        break;

                    case data::material::point:
                        ogre_pass->setPolygonMode(Ogre::PM_POINTS);
                        ogre_pass->setPointSpritesEnabled(false);
                        ogre_pass->setPointSize(1.F);
                        break;

                    default:
                        if(_polygon_mode != data::material::edge)
                        {
                            SIGHT_ASSERT("Unhandled material representation mode : " << _polygon_mode, false);
                        }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void generic::set_ambient_diffuse(const data::material::sptr& _material)
{
    //Set up Material colors
    data::color::csptr sight_ambient = _material->ambient();
    data::color::csptr sight_diffuse = _material->diffuse();

    const Ogre::ColourValue ambient(sight_ambient->red(), sight_ambient->green(),
                                    sight_ambient->blue(), sight_ambient->alpha());
    m_material->setAmbient(ambient);

    const Ogre::ColourValue diffuse(sight_diffuse->red(), sight_diffuse->green(),
                                    sight_diffuse->blue(), sight_diffuse->alpha());
    m_material->setDiffuse(diffuse);
}

//------------------------------------------------------------------------------

void generic::set_texture(
    const std::string& _sampler_name,
    const Ogre::TexturePtr& _texture,
    std::optional<Ogre::TextureFilterOptions> _filter_type,
    std::optional<Ogre::TextureAddressingMode> _address_mode
)
{
    bool found = false;
    for_all_techniques(
        [&](auto& _pass)
        {
            if(Ogre::TextureUnitState* const tex_unit = _pass.getTextureUnitState(_sampler_name))
            {
                found = true;
                if(_filter_type != std::nullopt)
                {
                    tex_unit->setTextureFiltering(_filter_type.value());
                }

                if(_address_mode != std::nullopt)
                {
                    tex_unit->setTextureAddressingMode(_address_mode.value());
                }

                if(_texture)
                {
                    tex_unit->setTexture(_texture);
                    tex_unit->setTextureName(_texture->getName(), _texture->getTextureType());
                }
            }
        });
    SIGHT_ASSERT("Texture " << std::quoted(_sampler_name) << " not found in any fragment program.", found);
}

//-----------------------------------------------------------------------------

void generic::remove_pass(const std::string& _name)
{
    SIGHT_ASSERT("Material is not set", m_material);

    const Ogre::Material::Techniques& techniques = m_material->getTechniques();
    for(auto* const technique : techniques)
    {
        SIGHT_ASSERT("technique is not set", technique);

        const Ogre::Technique::Passes& passes = technique->getPasses();
        std::vector<Ogre::Pass*> remove_pass_vector;

        // Collect the passes to remove
        for(auto* const ogre_pass : passes)
        {
            if(ogre_pass->getName() == _name)
            {
                remove_pass_vector.push_back(ogre_pass);
                continue;
            }
        }

        // Perform the removal
        for(auto* edge_pass : remove_pass_vector)
        {
            technique->removePass(edge_pass->getIndex());
        }
    }
}

//-----------------------------------------------------------------------------

void generic::clean_generated_techniques()
{
    SIGHT_ASSERT("Material is not set", m_material);

    const Ogre::Material::Techniques& techniques = m_material->getTechniques();

    std::vector<std::uint16_t> remove_technique_vector;

    std::uint16_t index = 0;

    for(auto* const technique : techniques)
    {
        SIGHT_ASSERT("technique is not set", technique);

        auto scheme = technique->getSchemeName();
        if(not Ogre::StringUtil::startsWith(scheme, "Default", false))
        {
            remove_technique_vector.push_back(index);
        }

        ++index;
    }

    // Remove in inverse order otherwise the index we stored becomes invalid ;-)
    /// @todo modernize that once clang supports std::ranges::reverse_view
    // NOLINTNEXTLINE(modernize-loop-convert)
    for(auto it = remove_technique_vector.rbegin() ; it != remove_technique_vector.rend() ; ++it)
    {
        m_material->removeTechnique(*it);
    }
}

} // namespace sight::viz::scene3d::material
