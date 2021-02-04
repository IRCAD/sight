/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/viz/ogre/adaptor/STexture.hpp"
#include "modules/viz/ogre/config.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>

#include <data/Image.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>

#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreMaterial.h>

#include <viz/ogre/IAdaptor.hpp>
#include <viz/ogre/Material.hpp>
#include <viz/ogre/Mesh.hpp>
#include <viz/ogre/ogre.hpp>
#include <viz/ogre/R2VBRenderable.hpp>

#include <regex>

namespace sight::data
{
class Material;
}

namespace sight::modules::viz::ogre::adaptor
{

/**
 * @brief This adaptor adapts a data::Material, allowing to tweak material parameters.
 *
 * @section Slots Slots
 * - \b updateField(data::Object::FieldsContainerType): listen to the fields in the data::Material.
 * - \b swapTexture(): listen to the modules::viz::ogre::adaptor::STexture changes.
 * - \b addTexture(): called when a texture is added in the data::Material.
 * - \b removeTexture(): called when a texture is removed in the data::Material.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::sight::modules::viz::ogre::adaptor::SMaterial">
        <inout key="material" uid="..." />
        <config materialTemplate="materialTemplateName" materialName="meshMaterial" textureName="texName"
                shadingMode="gouraud" normalLength="0.1" representationMode="SURFACE" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b material [data::Material]: adapted material. The material may be modified to comply to the configuration
 *      of the adaptor.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the mesh's layer.
 * - \b materialTemplate (optional, string, default=""): name of the base Ogre material/
 * - \b materialName (optional, string, default=""): name of the Ogre material. This is necessary to bind a
 *      ::sight::modules::viz::ogre:SMesh or a ::sight::modules::viz::ogre:SModelSeries to this material;
 *      simply specify the same Ogre material in its configuration.
 * - \b textureName (optional, string, default=""): the Ogre texture name used the material. Use it if you want to
 *      reference a texture managed by an another modules::viz::ogre::adaptor::STexture.
 *  - \b shadingMode (optional, none/flat/gouraud/phong, default=phong): name of the used shading mode.
 *  - \b normalLength (optional, default=0.1): factor defining the length of the normals.
 *  - \b representationMode (optional, SURFACE/POINT/WIREFRAME/EDGE, default=SURFACE):
 *      representation mode as in data::Material.
 */
class MODULE_VIZ_OGRE_CLASS_API SMaterial final : public sight::viz::ogre::IAdaptor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SMaterial, ::sight::viz::ogre::IAdaptor)

    /**
     * @name Slots API
     * @{
     */
    MODULE_VIZ_OGRE_API static const core::com::Slots::SlotKeyType s_UPDATE_FIELD_SLOT;
    MODULE_VIZ_OGRE_API static const core::com::Slots::SlotKeyType s_SWAP_TEXTURE_SLOT;
    MODULE_VIZ_OGRE_API static const core::com::Slots::SlotKeyType s_ADD_TEXTURE_SLOT;
    MODULE_VIZ_OGRE_API static const core::com::Slots::SlotKeyType s_REMOVE_TEXTURE_SLOT;
    /** @} */

    /**
     * @name In-Out In-Out API
     * @{
     */
    MODULE_VIZ_OGRE_API static const std::string s_MATERIAL_INOUT;
    /** @} */

    /// Initializes slots.
    MODULE_VIZ_OGRE_API SMaterial() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_OGRE_API ~SMaterial() noexcept override;

    /// Gets Ogre associated material.
    MODULE_VIZ_OGRE_API ::Ogre::MaterialPtr getMaterial();

    /// Gets material name.
    MODULE_VIZ_OGRE_API std::string getMaterialName() const;

    /// Retrieves the associated texture name.
    MODULE_VIZ_OGRE_API void setTextureName(const std::string& _textureName);

    /// Sets material name.
    MODULE_VIZ_OGRE_API void setMaterialName(const std::string& _materialName);

    /// Sets material template name.
    MODULE_VIZ_OGRE_API void setMaterialTemplateName(const std::string& _materialName);

    /// Tells if there is a texture currently bound.
    MODULE_VIZ_OGRE_API bool hasDiffuseTexture() const;

    /// Gets the shading mode.
    MODULE_VIZ_OGRE_API const std::string& getShadingMode() const;

    /// Sets the shading mode.
    MODULE_VIZ_OGRE_API void setShadingMode(const std::string& _shadingMode);

    /// Set the renderable object.
    MODULE_VIZ_OGRE_API void setR2VBObject(sight::viz::ogre::R2VBRenderable* _r2vbObject);

    /// Gets the internal material code.
    MODULE_VIZ_OGRE_API sight::viz::ogre::Material* getMaterialFw() const;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_OGRE_API void configuring() override;

    /// Creates the material.
    MODULE_VIZ_OGRE_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Material::s_MODIFIED_SIG of s_MATERIAL_INOUT to s_UPDATE_SLOT
     * Connect data::Material::s_ADDED_FIELDS_SIG of s_MATERIAL_INOUT to s_UPDATE_FIELD_SLOT
     * Connect data::Material::s_CHANGED_FIELDS_SIG of s_MATERIAL_INOUT to s_UPDATE_FIELD_SLOT
     * Connect data::Material::s_ADDED_TEXTURE_SIG of s_MATERIAL_INOUT to s_ADD_TEXTURE_SLOT
     * Connect data::Material::s_REMOVED_TEXTURE_SIG of s_MATERIAL_INOUT to s_REMOVE_TEXTURE_SLOT
     */
    MODULE_VIZ_OGRE_API services::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Updates fixed function pipeline parameters.
    MODULE_VIZ_OGRE_API void updating() override;

    /// Release Ogre resources.
    MODULE_VIZ_OGRE_API void stopping() override;

private:

    /**
     * @brief SLOT: updates the material from the input data fields.
     * @param _fields fields to update, only "ogreMaterial" is taken into account.
     */
    void updateField(data::Object::FieldsContainerType _fields);

    /// SLOT: swaps the texture of the material.
    void swapTexture();

    /// SLOT: creates a texture adaptor when a texture is added to the material.
    /// This method is also called from the starting in order to create the texture adaptor if the material has a
    /// default texture.
    void createTextureAdaptor();

    /// SLOT: removes the texture adaptor when the texture is removed from the material.
    void removeTextureAdaptor();

    /// Creates shader parameters adaptors from resources.
    void createShaderParameterAdaptors();

    /// Defines the material name. It is auto generated.
    std::string m_materialName;

    /// Defines the default template name, given by xml configuration.
    /// It must refer an existing Ogre material which will be used in order to instanciate m_material
    std::string m_materialTemplateName { sight::viz::ogre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME };

    /// Contains the texture adaptor the material adaptor is listening to.
    modules::viz::ogre::adaptor::STexture::sptr m_texAdaptor { nullptr };

    /// Defines the texture name.
    std::string m_textureName;

    /// Stores supported schemes.
    std::vector< Ogre::String > m_schemesSupported;

    /// Handles connections with texture adaptor.
    core::com::helper::SigSlotConnection m_textureConnection;

    /// Defines the configured shading mode.
    std::string m_shadingMode;

    /// Defines the configured representation mode.
    std::string m_representationMode { "SURFACE" };

    /// Stores a map to convert from string to data::Material::RepresentationType (ex: "SURFACE" = SURFACE).
    std::map< std::string, data::Material::RepresentationType > m_representationDict;

    /// Contains the Ogre material.
    sight::viz::ogre::Material::uptr m_materialFw;

    /// Contains the renderable object.
    sight::viz::ogre::R2VBRenderable* m_r2vbObject { nullptr };
};

//------------------------------------------------------------------------------

inline ::Ogre::MaterialPtr SMaterial::getMaterial()
{
    return ::Ogre::MaterialManager::getSingleton().getByName(m_materialName, sight::viz::ogre::RESOURCE_GROUP);
}

//------------------------------------------------------------------------------

inline void SMaterial::setMaterialTemplateName(const std::string& _materialName)
{
    m_materialTemplateName = _materialName;
}

//------------------------------------------------------------------------------

inline void SMaterial::setMaterialName(const std::string& _materialName)
{
    m_materialName = _materialName;
}

//------------------------------------------------------------------------------

inline std::string SMaterial::getMaterialName() const
{
    return m_materialName;
}

//------------------------------------------------------------------------------

inline bool SMaterial::hasDiffuseTexture() const
{
    return (m_texAdaptor && m_texAdaptor->isValid());
}

//------------------------------------------------------------------------------

inline const std::string& SMaterial::getShadingMode() const
{
    return m_shadingMode;
}

//------------------------------------------------------------------------------

inline void SMaterial::setShadingMode(const std::string& _shadingMode)
{
    m_shadingMode = _shadingMode;
}

//------------------------------------------------------------------------------

inline void SMaterial::setR2VBObject(sight::viz::ogre::R2VBRenderable* _r2vbObject)
{
    m_r2vbObject = _r2vbObject;
}

//------------------------------------------------------------------------------

inline sight::viz::ogre::Material* SMaterial::getMaterialFw() const
{
    return m_materialFw.get();
}

//------------------------------------------------------------------------------

} // namespace sight::modules::viz::ogre::adaptor.
