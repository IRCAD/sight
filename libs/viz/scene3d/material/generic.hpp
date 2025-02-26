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

#pragma once

#include <sight/viz/scene3d/config.hpp>

#include <data/material.hpp>
#include <data/mesh.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreTechnique.h>

namespace sight::viz::scene3d
{

class mesh;

}

namespace sight::viz::scene3d::material
{

/**
 * @brief   Wrapper around an Ogre::Material class.
 *
 * It simplifies several aspects:
 * - instantiation and removal of the material resource
 * - binding of uniform values
 * - binding of textures
 *
 * It is highly recommended to use this wrapper in adaptors instead of dealing directly with Ogre::Material,
 * especially if you have to set uniforms or textures. Indeed, some internal logic, like the OIT (Order Independent
 * Transparency), requires special care regarding these actions. This wrapper saves you from these considerations.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API generic
{
public:

    using uptr = std::unique_ptr<generic>;

    struct passes
    {
        static const inline std::string EDGES   = "EdgePass";
        static const inline std::string NORMALS = "NormalsPass";
    };

    /// Constructor
    SIGHT_VIZ_SCENE3D_API generic(const std::string& _name, const std::string& _template_name);

    /// Destructor
    SIGHT_VIZ_SCENE3D_API virtual ~generic();

    /// @brief Set the vertex input layout (normals, colors, uvs, etc...)
    /// @param _layout input layout, as a mask
    void set_layout(data::mesh::attribute _layout);

    /// @brief Set the vertex input layout (normals, colors, uvs, etc...)
    /// @param _mesh mesh instance from which to get the vertex layout
    SIGHT_VIZ_SCENE3D_API virtual void set_layout(const sight::viz::scene3d::mesh& _mesh);

    /// @brief Set material polygon mode (surface, point or wireframe)
    /// @param _polygon_mode surface, point or wireframe
    SIGHT_VIZ_SCENE3D_API void set_polygon_mode(sight::data::material::representation_t _polygon_mode);

    /// @brief Set material colors from a material data
    /// @param _material material from which to get the diffuse and ambient colors
    SIGHT_VIZ_SCENE3D_API void set_ambient_diffuse(const data::material::sptr& _material);

    /// Return the unique name of the Ogre material resource
    std::string name() const;

    /// Return the Ogre material resource instance itself
    Ogre::MaterialPtr material() const;

    /// @brief Set a texture and the associated unit state
    /// @param _sampler_name name of the sampler, as defined in the .material script
    /// @param _texture pointer to the Ogre texture resource
    /// @param _filter_type filtering (nearest, linear, etc...)
    /// @param _address_mode address mode (wrap, clamp, etc...)
    SIGHT_VIZ_SCENE3D_API void set_texture(
        const std::string& _sampler_name,
        const Ogre::TexturePtr& _texture,
        std::optional<Ogre::TextureFilterOptions> _filter_type   = {},
        std::optional<Ogre::TextureAddressingMode> _address_mode = {});

    /// @brief Set a vertex shader uniform
    /// @tparam T type of the uniform, as supported by Ogre::GpuProgramParameters
    /// @param _uniform_name name of the uniform, as defined in the vertex shader source
    /// @param _value uniform value
    template<typename T>
    void set_vertex_uniform(const std::string& _uniform_name, const T& _value);

    /// @brief Set a geometry shader uniform
    /// @tparam T type of the uniform, as supported by Ogre::GpuProgramParameters
    /// @param _uniform_name name of the uniform, as defined in the geometry shader source
    /// @param _value uniform value
    template<typename T>
    void set_geometry_uniform(const std::string& _uniform_name, const T& _value);

    /// @brief Set a fragment shader uniform
    /// @tparam T type of the uniform, as supported by Ogre::GpuProgramParameters
    /// @param _uniform_name name of the uniform, as defined in the fragment shader source
    /// @param _value uniform value
    template<typename T>
    void set_fragment_uniform(const std::string& _uniform_name, const T& _value);

protected:

    /// Remove a rendering pass in all techniques on the current material
    void remove_pass(const std::string& _name);

    /// Remove all techniques generated by listeners such as order independent transparency manager
    /// Each time we have to modify the shader programs, we clean everything
    /// and we let the managers generate the techniques from the basic techniques defined in the .material
    void clean_generated_techniques();

    /// Helper function to execute a function for all techniques of this material
    void for_all_techniques(const std::function<void(Ogre::Pass&)>);

    /// Ogre material resource
    Ogre::MaterialPtr m_material;

    /// Name of the material template
    std::string m_template_name;

    /// Input vertex layout
    sight::data::mesh::attribute m_layout {data::mesh::attribute::point_normals};
};

//------------------------------------------------------------------------------

inline void generic::set_layout(data::mesh::attribute _layout)
{
    m_layout = _layout;
}

//------------------------------------------------------------------------------

inline std::string generic::name() const
{
    return m_material->getName();
}

//------------------------------------------------------------------------------

inline Ogre::MaterialPtr generic::material() const
{
    return m_material;
}

//------------------------------------------------------------------------------

template<typename T>
void generic::set_vertex_uniform(const std::string& _uniform_name, const T& _value)
{
    [[maybe_unused]] bool found = false;
    for_all_techniques(
        [&](auto& _pass)
        {
            if(_pass.hasVertexProgram())
            {
                auto vp_params = _pass.getVertexProgramParameters();
                if(vp_params->_findNamedConstantDefinition(_uniform_name) != nullptr)
                {
                    vp_params->setNamedConstant(_uniform_name, _value);
                    found = true;
                }
            }
        });
    SIGHT_ASSERT("Uniform " << std::quoted(_uniform_name) << " not found in any vertex program.", found);
}

//------------------------------------------------------------------------------

template<typename T>
void generic::set_geometry_uniform(const std::string& _uniform_name, const T& _value)
{
    [[maybe_unused]] bool found = false;
    for_all_techniques(
        [&](auto& _pass)
        {
            if(_pass.hasGeometryProgram())
            {
                auto gp_params = _pass.getGeometryProgramParameters();
                if(gp_params->_findNamedConstantDefinition(_uniform_name) != nullptr)
                {
                    gp_params->setNamedConstant(_uniform_name, _value);
                    found = true;
                }
            }
        });
    SIGHT_ASSERT("Uniform " << std::quoted(_uniform_name) << " not found in any geometry program.", found);
}

//------------------------------------------------------------------------------

template<typename T>
void generic::set_fragment_uniform(const std::string& _uniform_name, const T& _value)
{
    [[maybe_unused]] bool found = false;
    for_all_techniques(
        [&](auto& _pass)
        {
            if(_pass.hasFragmentProgram())
            {
                auto fp_params = _pass.getFragmentProgramParameters();
                if(fp_params->_findNamedConstantDefinition(_uniform_name) != nullptr)
                {
                    fp_params->setNamedConstant(_uniform_name, _value);
                    found = true;
                }
            }
        });
    SIGHT_ASSERT("Uniform " << std::quoted(_uniform_name) << " not found in any fragment program.", found);
}

//------------------------------------------------------------------------------

inline void generic::for_all_techniques(const std::function<void(Ogre::Pass&)> _fn)
{
    const Ogre::Material::Techniques& techniques = m_material->getTechniques();

    for(const auto* const tech : techniques)
    {
        SIGHT_ASSERT("The technique is null", tech);

        Ogre::Pass* const pass = tech->getPass(0);
        SIGHT_ASSERT("The pass is null.", pass);
        _fn(*pass);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::material
