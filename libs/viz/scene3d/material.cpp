/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "viz/scene3d/material.hpp"

#include "viz/scene3d/helper/shading.hpp"

#include <viz/scene3d/layer.hpp>
#include <viz/scene3d/ogre.hpp>

#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgreTextureManager.h>

namespace sight::viz::scene3d
{

static const std::string EDGE_PASS    = "EdgePass";
static const std::string NORMALS_PASS = "NormalsPass";

//-----------------------------------------------------------------------------

const std::string material::DEFAULT_MATERIAL_TEMPLATE_NAME = "Default";

//------------------------------------------------------------------------------

material::material(const std::string& _name, const std::string& _template_name) :
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
}

//------------------------------------------------------------------------------

material::~material()
{
    auto material_name = m_material->getName();
    m_material.reset();
    Ogre::MaterialManager::getSingleton().remove(material_name, sight::viz::scene3d::RESOURCE_GROUP);
}

//------------------------------------------------------------------------------

void material::update_options_mode(int _options_mode)
{
    this->clean_transparency_techniques();
    // First remove the normals pass if there is already one
    this->remove_pass(NORMALS_PASS);

    const Ogre::Material::Techniques& techniques = m_material->getTechniques();

    if(_options_mode != data::material::standard)
    {
        for(auto* const current_technique : techniques)
        {
            // We need the first pass of the current technique in order to copy its rendering states in the normals pass
            Ogre::Pass* first_pass = current_technique->getPass(0);
            SIGHT_ASSERT("Pass is null", first_pass);

            const bool depth_only = viz::scene3d::helper::shading::is_depth_only_technique(*current_technique);

            if(viz::scene3d::helper::shading::is_geometric_technique(*current_technique) || depth_only)
            {
                // We copy the first pass, thus keeping all rendering states
                Ogre::Pass* normals_pass = current_technique->createPass();
                *normals_pass = *first_pass;
                normals_pass->setName(NORMALS_PASS);

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
    else
    {
        for(auto* const current_technique : techniques)
        {
            // We need the first pass of the current technique in order to copy its rendering states in the normals pass
            const Ogre::Pass* first_pass = current_technique->getPass(0);
            SIGHT_ASSERT("Pass is null", first_pass);

            if(first_pass->hasGeometryProgram())
            {
                Ogre::GpuProgramParametersSharedPtr gp = first_pass->getGeometryProgramParameters();

                if(gp && (gp->_findNamedConstantDefinition("u_billboardSize") != nullptr))
                {
                    gp->setNamedConstant("u_billboardSize", m_mesh_size);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void material::update_polygon_mode(int _polygon_mode)
{
    // This is necessary to load and compile the material, otherwise the following technique iterator
    // is null when we call this method on the first time (from starting() for instance)
    m_material->touch();

    this->clean_transparency_techniques();

    // First remove a previous normal pass if it exists
    this->remove_pass(EDGE_PASS);

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

            Ogre::Pass* edge_pass = tech->getPass(EDGE_PASS);
            if(edge_pass == nullptr)
            {
                // We copy the first pass, thus keeping all rendering states
                edge_pass  = tech->createPass();
                *edge_pass = *first_pass;
                edge_pass->setName(EDGE_PASS);

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

void material::update_shading_mode(
    data::material::shading_t _shading_mode,
    int _num_lights,
    bool _has_diffuse_texture,
    bool _use_texture_alpha
)
{
    const bool is_r2_vb = m_primitive_type != data::mesh::cell_type_t::triangle || m_has_primitive_color;

    // If we have UVs with R2VB, then for the sake of simplicity we generate UVs whatever the material is
    // Otherwise we would have to update the whole R2VB pipeline each time a texture is set/unset
    const bool need_diffuse_texture = is_r2_vb ? m_has_uv : m_has_uv && _has_diffuse_texture;

    const auto mode = static_cast<data::material::shading_t>(_shading_mode);

    const Ogre::String permutation = viz::scene3d::helper::shading::get_permutation(
        mode,
        need_diffuse_texture,
        m_has_vertex_color
    );
    const Ogre::String r2vb_gs_name = viz::scene3d::helper::shading::get_r2_vb_geometry_program_name(
        m_primitive_type,
        need_diffuse_texture,
        m_has_vertex_color,
        m_has_primitive_color
    );

    this->clean_transparency_techniques();

    // Iterate through each technique found in the material and switch the shading mode
    const Ogre::Material::Techniques& techniques = m_material->getTechniques();
    for(auto* const tech : techniques)
    {
        SIGHT_ASSERT("technique is not set", tech);

        const Ogre::Technique::Passes& passes = tech->getPasses();

        for(auto* const ogre_pass : passes)
        {
            // Nothing to do for edge and normal passes
            if(ogre_pass->getName() == EDGE_PASS || ogre_pass->getName() == NORMALS_PASS)
            {
                continue;
            }

            if(is_r2_vb)
            {
                // We need a geometry shader (primitive generation and per-primitive color)
                // and thus we rely on the render to vertex buffer pipeline

                ogre_pass->setVertexProgram("R2VB/" + permutation + "_VP");
                ogre_pass->setGeometryProgram(r2vb_gs_name);
                ogre_pass->setFragmentProgram("");

                if(m_has_primitive_color)
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
            else
            {
                if(m_template_name == DEFAULT_MATERIAL_TEMPLATE_NAME)
                {
                    // "Regular" pipeline
                    std::string vp_name = ogre_pass->getVertexProgramName();
                    vp_name = viz::scene3d::helper::shading::set_permutation_in_program_name(vp_name, permutation);
                    ogre_pass->setVertexProgram(vp_name);

                    std::string fp_name = ogre_pass->getFragmentProgramName();
                    fp_name = viz::scene3d::helper::shading::set_permutation_in_program_name(fp_name, permutation);
                    ogre_pass->setFragmentProgram(fp_name);

                    const bool color_pass = viz::scene3d::helper::shading::is_color_technique(*tech);

                    // Updates the u_hasTextureAlpha flag uniform according to the configuration of the texture adaptor
                    if(need_diffuse_texture && color_pass)
                    {
                        int use_texture_alpha = static_cast<int>(_use_texture_alpha);
                        ogre_pass->getFragmentProgramParameters()->setNamedConstant(
                            "u_useTextureAlpha",
                            use_texture_alpha
                        );
                    }

                    Ogre::GpuProgramParametersSharedPtr vp = ogre_pass->getVertexProgramParameters();

                    if(vp->_findNamedConstantDefinition("u_fNumLights") != nullptr)
                    {
                        vp->setNamedConstant("u_fNumLights", static_cast<float>(_num_lights));
                    }

                    if(!ogre_pass->getFragmentProgramName().empty())
                    {
                        Ogre::GpuProgramParametersSharedPtr fp = ogre_pass->getFragmentProgramParameters();

                        if(fp->_findNamedConstantDefinition("u_fNumLights") != nullptr)
                        {
                            fp->setNamedConstant("u_fNumLights", static_cast<float>(_num_lights));
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void material::update_rgba_mode(data::material::sptr _sight_material)
{
    //Set up Material colors
    data::color::csptr sight_ambient = _sight_material->ambient();
    data::color::csptr sight_diffuse = _sight_material->diffuse();

    const Ogre::ColourValue ambient(sight_ambient->red(), sight_ambient->green(),
                                    sight_ambient->blue(), sight_ambient->alpha());
    m_material->setAmbient(ambient);

    const Ogre::ColourValue diffuse(sight_diffuse->red(), sight_diffuse->green(),
                                    sight_diffuse->blue(), sight_diffuse->alpha());
    m_material->setDiffuse(diffuse);

    const Ogre::ColourValue specular(.2F, .2F, .2F, 1.F);
    m_material->setSpecular(specular);
    m_material->setShininess(25);
}

//------------------------------------------------------------------------------

void material::set_diffuse_texture(const Ogre::TexturePtr& _texture)
{
    this->clean_transparency_techniques();

    const Ogre::Material::Techniques& techniques = m_material->getTechniques();

    for(auto* const technique : techniques)
    {
        SIGHT_ASSERT("technique is not set", technique);

        if(viz::scene3d::helper::shading::is_color_technique(*technique))
        {
            Ogre::Pass* pass                       = technique->getPass(0);
            Ogre::TextureUnitState* tex_unit_state = pass->getTextureUnitState("diffuseTexture");

            if(tex_unit_state != nullptr)
            {
                if(_texture)
                {
                    tex_unit_state->setTexture(_texture);
                }
                else
                {
                    pass->removeTextureUnitState(pass->getTextureUnitStateIndex(tex_unit_state));
                }
            }
            else
            {
                if(_texture)
                {
                    tex_unit_state = pass->createTextureUnitState("diffuseTexture");
                    tex_unit_state->setTexture(_texture);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void material::set_template(const std::string& _template_name)
{
    const Ogre::MaterialPtr ogre_material = Ogre::MaterialManager::getSingleton().getByName(
        _template_name,
        RESOURCE_GROUP
    );

    SIGHT_ASSERT("Material '" + _template_name + "'' not found", ogre_material);

    // Then we copy these parameters in m_material.
    // We can now alter this new instance without changing the default material
    ogre_material.get()->copyDetailsTo(m_material);
}

//------------------------------------------------------------------------------

std::string material::name() const
{
    return m_material->getName();
}

//-----------------------------------------------------------------------------

void material::remove_pass(const std::string& _name)
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

void material::clean_transparency_techniques()
{
    SIGHT_ASSERT("Material is not set", m_material);

    const Ogre::Material::Techniques& techniques = m_material->getTechniques();

    std::vector<std::uint16_t> remove_technique_vector;

    std::uint16_t index = 0;

    for(auto* const technique : techniques)
    {
        SIGHT_ASSERT("technique is not set", technique);

        auto scheme = technique->getSchemeName();
        if(Ogre::StringUtil::startsWith(scheme, "CelShadingDepthPeeling", false)
           || Ogre::StringUtil::startsWith(scheme, "DepthPeeling", false)
           || Ogre::StringUtil::startsWith(scheme, "DualDepthPeeling", false)
           || Ogre::StringUtil::startsWith(scheme, "HybridTransparency", false)
           || Ogre::StringUtil::startsWith(scheme, "WeightedBlended", false))
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

} // namespace sight::viz::scene3d
