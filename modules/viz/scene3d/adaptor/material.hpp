/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#pragma once

#include "modules/viz/scene3d/adaptor/texture.hpp"

#include <data/image.hpp>
#include <data/material.hpp>
#include <data/mesh.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/material/generic.hpp>
#include <viz/scene3d/material/standard.hpp>
#include <viz/scene3d/mesh.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/r2vb_renderable.hpp>

#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreMaterial.h>

#include <regex>

namespace sight::data
{

class Material;

} // namespace sight::data

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor adapts a data::material, allowing to tweak material parameters.
 *
 * @section Slots Slots
 * - \b updateField(data::object::fields_container_t): listen to the fields in the data::material.
 * - \b swapTexture(): listen to the module::viz::scene3d::adaptor::texture changes.
 * - \b addTexture(): called when a texture is added in the data::material.
 * - \b removeTexture(): called when a texture is removed in the data::material.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::material">
        <inout key="material" uid="..." />
        <config materialTemplate="materialTemplateName" materialName="meshMaterial" textureName="texName"
                shadingMode="phong" normalLength="0.1" representationMode="SURFACE" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b material [sight::data::material]: adapted material. The material may be modified to comply to the configuration
 *      of the adaptor.
 *
 * @subsection Configuration Configuration:
 * - \b materialTemplate (optional, string, default=""): name of the base Ogre material/
 * - \b materialName (optional, string, default=""): name of the Ogre material. This is necessary to bind a
 *      sight::module::viz::scene3d:mesh or a sight::module::viz::scene3d:model_series to this material;
 *      simply specify the same Ogre material in its configuration.
 * - \b textureName (optional, string, default=""): the Ogre texture name used the material. Use it if you want to
 *      reference a texture managed by an another module::viz::scene3d::adaptor::texture.
 *  - \b shadingMode (optional, none/flat/phong, default=phong): name of the used shading mode.
 *  - \b normalLength (optional, default=0.1): factor defining the length of the normals.
 *  - \b representationMode (optional, SURFACE/POINT/WIREFRAME/EDGE, default=SURFACE):
 *      representation mode as in data::material.
 */
class material final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(material, sight::viz::scene3d::adaptor);

    struct signals
    {
        using changed_t = core::com::signal<void (Ogre::MaterialPtr)>;
        static inline const core::com::signals::key_t CHANGED = "changed";
    };

    /**
     * @name Slots API
     * @{
     */
    static const core::com::slots::key_t UPDATE_FIELD_SLOT;
    static const core::com::slots::key_t SWAP_TEXTURE_SLOT;
    static const core::com::slots::key_t ADD_TEXTURE_SLOT;
    static const core::com::slots::key_t REMOVE_TEXTURE_SLOT;
    /** @} */

    /**
     * @name In-Out In-Out API
     * @{
     */
    static const std::string MATERIAL_INOUT;
    /** @} */

    /// Initializes slots.
    material() noexcept;

    /// Destroys the adaptor.
    ~material() noexcept override = default;

    /// Configures the adaptor without using the XML configuration.
    void configure(
        const std::string& _id,
        const std::string& _name,
        sight::viz::scene3d::render::sptr _service,
        const std::string& _layer,
        const std::string& _shading_mode = "",
        const std::string& _template     = sight::viz::scene3d::material::standard::TEMPLATE
    );

    /// Gets Ogre associated material.
    Ogre::MaterialPtr get_material();

    /// Gets material name.
    std::string get_material_name() const;

    /// Retrieves the associated texture name.
    void set_texture_name(const std::string& _texture_name);

    /// Sets material name.
    void set_material_name(const std::string& _material_name);

    /// Sets material template name.
    void set_material_template_name(const std::string& _material_name);

    /// Tells if there is a texture currently bound.
    bool has_diffuse_texture() const;

    /// Gets the shading mode.
    const std::string& get_shading_mode() const;

    /// Sets the shading mode.
    void set_shading_mode(const std::string& _shading_mode);

    /// Gets the representation mode.
    data::material::representation_t representation_mode() const;

    /// Sets the representation mode.
    void set_representation_mode(data::material::representation_t _representation_mode);

    /// Gets the internal material code.
    sight::viz::scene3d::material::generic* get_material_impl() const;

protected:

    /// Configures the adaptor.
    void configuring() override;

    /// Creates the material.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::material::MODIFIED_SIG of s_MATERIAL_INOUT to service::slots::UPDATE
     * Connect data::material::ADDED_FIELDS_SIG of s_MATERIAL_INOUT to UPDATE_FIELD_SLOT
     * Connect data::material::CHANGED_FIELDS_SIG of s_MATERIAL_INOUT to UPDATE_FIELD_SLOT
     * Connect data::material::ADDED_TEXTURE_SIG of s_MATERIAL_INOUT to ADD_TEXTURE_SLOT
     * Connect data::material::REMOVED_TEXTURE_SIG of s_MATERIAL_INOUT to REMOVE_TEXTURE_SLOT
     */
    service::connections_t auto_connections() const override;

    /// Updates fixed function pipeline parameters.
    void updating() override;

    /// Release Ogre resources.
    void stopping() override;

private:

    /**
     * @brief SLOT: updates the material from the input data fields.
     * @param _fields fields to update, only "ogreMaterial" is taken into account.
     */
    void update_field(data::object::fields_container_t _fields);

    /// SLOT: swaps the texture of the material.
    void swap_texture();

    /// SLOT: creates a texture adaptor when a texture is added to the material.
    /// This method is also called from the starting in order to create the texture adaptor if the material has a
    /// default texture.
    void create_texture_adaptor();

    /// SLOT: removes the texture adaptor when the texture is removed from the material.
    void remove_texture_adaptor();

    /// Creates shader parameters adaptors from resources.
    void create_shader_parameter_adaptors();

    /// Defines the material name. It is auto generated.
    std::string m_material_name;

    /// Defines the default template name, given by xml configuration.
    /// It must refer an existing Ogre material which will be used in order to instantiate m_material
    std::string m_material_template_name {sight::viz::scene3d::material::standard::TEMPLATE};

    /// Contains the texture adaptor the material adaptor is listening to.
    module::viz::scene3d::adaptor::texture::sptr m_tex_adaptor {nullptr};

    /// Defines the texture name.
    std::string m_texture_name;

    /// Stores supported schemes.
    std::vector<Ogre::String> m_schemes_supported;

    /// Handles connections with texture adaptor.
    core::com::helper::sig_slot_connection m_texture_connection;

    /// Defines the configured shading mode.
    std::string m_shading_mode;

    /// Defines the configured representation mode.
    sight::data::material::representation_t m_representation_mode {sight::data::material::representation_t::surface};

    /// Implementation when we instantiate the standard material
    sight::viz::scene3d::material::standard::uptr m_standard_material_impl;

    /// Implementation of the material.
    sight::viz::scene3d::material::generic::uptr m_material_impl;

    data::ptr<data::material, data::access::inout> m_material_data {this, MATERIAL_INOUT};
};

//------------------------------------------------------------------------------

inline Ogre::MaterialPtr material::get_material()
{
    return Ogre::MaterialManager::getSingleton().getByName(m_material_name, sight::viz::scene3d::RESOURCE_GROUP);
}

//------------------------------------------------------------------------------

inline void material::set_material_template_name(const std::string& _material_name)
{
    m_material_template_name = _material_name;
}

//------------------------------------------------------------------------------

inline void material::set_material_name(const std::string& _material_name)
{
    m_material_name = _material_name;
}

//------------------------------------------------------------------------------

inline std::string material::get_material_name() const
{
    return m_material_name;
}

//------------------------------------------------------------------------------

inline bool material::has_diffuse_texture() const
{
    return m_tex_adaptor && m_tex_adaptor->is_valid();
}

//------------------------------------------------------------------------------

inline const std::string& material::get_shading_mode() const
{
    return m_shading_mode;
}

//------------------------------------------------------------------------------

inline void material::set_shading_mode(const std::string& _shading_mode)
{
    m_shading_mode = _shading_mode;
}

//------------------------------------------------------------------------------

inline data::material::representation_t material::representation_mode() const
{
    return m_representation_mode;
}

//------------------------------------------------------------------------------

inline void material::set_representation_mode(data::material::representation_t _representation_mode)
{
    m_representation_mode = _representation_mode;
}

//------------------------------------------------------------------------------

inline sight::viz::scene3d::material::generic* material::get_material_impl() const
{
    if(m_material_impl != nullptr)
    {
        return m_material_impl.get();
    }
    else
    {
        return m_standard_material_impl.get();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
