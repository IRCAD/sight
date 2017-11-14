/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SMATERIAL_HPP__
#define __VISUOGREADAPTOR_SMATERIAL_HPP__

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/STexture.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>

#include <fwRenderOgre/IAdaptor.hpp>

#include <boost/shared_ptr.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreMaterial.h>

#include <regex>

namespace fwData
{
class Material;
}

namespace visuOgreAdaptor
{

/**
 * @brief   Adapt a ::fwData::Material, allowing to tweak material parameters.
 *
 * @section Slots Slots
 * - \b updateField(::fwData::Object::FieldsContainerType) : Listen to the fields in the ::fwData::Material.
 * - \b swapTexture() : Listen to the ::visuOgreAdaptor::STexture changes.
 * - \b addTexture() : Called when a texture is added in the ::fwData::Material.
 * - \b removeTexture() : Called when a texture is removed in the ::fwData::Material.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SMaterial">
        <inout key="material" uid="..." />
        <config materialTemplate="materialTemplateName" materialName="meshMaterial" textureName="texName"
                shadingMode="gouraud" normalLength="0.1" />
    </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b material [::fwData::Material]: adapted material. The material may be modified to comply to the configuration
 * of the adaptor.
 * @subsection Configuration Configuration:
 *  - \b materialTemplate (mandatory) : name of the base Ogre material
 *  - \b materialName (optional) : name of the Ogre material. This is necessary to bind a ::visuOgreAdaptor:SMesh or a
 * ::visuOgreAdaptor:SModelSeries to this material; simply specify the same Ogre material in its configuration.
 *  - \b textureName (optional) : the Ogre texture name used the material. Use it if you want to reference a texture
 * managed by an another ::visuOgreAdaptor::STexture.
 *  - \b shadingMode (optional, none/flat/gouraud/phong, default=phong) : name of the used shading mode
 *  - \b normalLength (optional, default=0.1) : factor defining the length of the normals
 */
class VISUOGREADAPTOR_CLASS_API SMaterial : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SMaterial)(::fwRenderOgre::IAdaptor) )

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_FIELD_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_FIELD_SLOT;

    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SWAP_TEXTURE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_ADD_TEXTURE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_TEXTURE_SLOT;
    /** @} */

    /// Name of the default used as a base for the instance created by this adaptor
    VISUOGREADAPTOR_API static const std::string DEFAULT_MATERIAL_TEMPLATE_NAME;

    /// Constructor
    VISUOGREADAPTOR_API SMaterial() noexcept;

    /// Destructor
    VISUOGREADAPTOR_API virtual ~SMaterial() noexcept;

    /// Get Ogre associated material
    VISUOGREADAPTOR_API ::Ogre::MaterialPtr getMaterial();

    /// Get material name
    VISUOGREADAPTOR_API std::string getMaterialName() const;

    /// Retrieves the associated texture name
    VISUOGREADAPTOR_API void setTextureName(const std::string& textureName);

    /// Set material name
    void setMaterialName(const std::string& materialName);

    /// Set material template name
    void setMaterialTemplateName(const std::string& materialName);

    bool getHasMeshNormal() const;
    void setHasMeshNormal(bool hasMeshNormal);

    bool getHasVertexColor() const;
    void setHasVertexColor(bool hasMeshNormal);

    bool getHasPrimitiveColor() const;
    void setHasPrimitiveColor(bool hasMeshNormal, const std::string& textureName);

    void setPrimitiveType(::fwData::Mesh::CellTypesEnum _type);

    /// Tells if there is a texture currently bound
    bool hasDiffuseTexture() const;

    const std::string& getShadingMode() const;
    void setShadingMode(const std::string& _shadingMode);

    void setMeshBoundingBox(const ::Ogre::AxisAlignedBox& _bbox);

    /// Returns proposals to connect service slots to associated object signals
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Configure the parameters
    VISUOGREADAPTOR_API void configuring() override;

    /// Starting method under fixed function pipeline
    VISUOGREADAPTOR_API void starting() override;

    /// Stopping method
    VISUOGREADAPTOR_API void stopping() override;

    /// Updating method, updates fixed function pipeline parameters
    VISUOGREADAPTOR_API void updating() override;

private:

    /// Slot: called when the material's field changed
    void updateField( ::fwData::Object::FieldsContainerType fields);

    /// Slot: called when the texture is swapped in the texture adaptor
    void swapTexture();

    /// Slot: called to create a texture adaptor when a texture is added to the material.
    /// This method is also called from the starting in order to create the texture adaptor if the material has a
    /// default texture.
    void createTextureAdaptor();

    /// Slot: called to remove the texture adaptor when the texture is removed from the material
    void removeTextureAdaptor();

    /// Creates a new object from loaded shader
    ::fwData::Object::sptr createObjectFromShaderParameter(::Ogre::GpuConstantType type,
                                                           std::string paramName);

    /// Loads material parameters from ressources
    void loadMaterialParameters();

    /// Updates material options mode (standard, point normals or cells normals)
    void updateOptionsMode( int optionMode );

    /// Updates material polygon mode (surface, point or wireframe)
    void updatePolygonMode( int polygonMode );

    /// Update material shading mode (flat/gouraud/phong) in fixed function pipeline
    void updateShadingMode( int shadingMode );

    /// Update material color in fixed function pipeline
    void updateRGBAMode( ::fwData::Material::sptr fw_material );

    /// Generates a normal length according to the mesh's bounding box
    ::Ogre::Real computeNormalLength();

    /// Remove a rendering pass in all techniques on the current material
    void removePass(const std::string& _name);

    /// Remove all techniques related to order independent transparency support
    /// Each time we have to modify the shader programs, we clean everything
    /// and we let the MaterialMgrListener generate the techniques from the basic techniques defined in the .material
    void cleanTransparencyTechniques();

    /// Associated Ogre material
    ::Ogre::MaterialPtr m_material;
    /// Material name. It is auto generated.
    std::string m_materialName;

    /// Default template name, given by xml configuration.
    /// It must refer an existing Ogre material which will be used in order to instanciate m_material
    std::string m_materialTemplateName;

    /// The texture adaptor the material adaptor is listening to
    ::visuOgreAdaptor::STexture::sptr m_texAdaptor;
    std::string m_textureName;

    /// Defines if the associated mesh has a normal layer
    bool m_hasMeshNormal;

    /// Defines if the associated mesh has a a per vertex color layer
    bool m_hasVertexColor;

    /// Defines if the associated mesh has a a per primitive color layer
    bool m_hasPrimitiveColor;

    /// Primitive type of the associated mesh
    ::fwData::Mesh::CellTypesEnum m_primitiveType;

    /// Name of the texture used to store per-primitive color
    std::string m_perPrimitiveColorTextureName;

    /// Bounding box of the mesh
    ::Ogre::AxisAlignedBox m_meshBoundingBox;
    /// Factor used to ease the normals length
    ::Ogre::Real m_normalLengthFactor;

    std::vector< Ogre::String > m_schemesSupported;

    /// Signal/Slot connections with texture adaptor
    ::fwCom::helper::SigSlotConnection m_textureConnection;

    /// The configured shading mode
    std::string m_shadingMode;

    /// Current number of lights in the scene.
    int m_lightsNumber;
};

//------------------------------------------------------------------------------
// Inline functions

//------------------------------------------------------------------------------

inline ::Ogre::MaterialPtr SMaterial::getMaterial()
{
    return m_material;
}

//------------------------------------------------------------------------------

inline bool SMaterial::getHasMeshNormal() const
{
    return m_hasMeshNormal;
}

//------------------------------------------------------------------------------

inline void SMaterial::setHasMeshNormal(bool _hasMeshNormal)
{
    m_hasMeshNormal = _hasMeshNormal;
}

//------------------------------------------------------------------------------

inline bool SMaterial::getHasVertexColor() const
{
    return m_hasVertexColor;
}

//------------------------------------------------------------------------------

inline void SMaterial::setHasVertexColor(bool _hasVertexColor)
{
    m_hasVertexColor = _hasVertexColor;
}

//------------------------------------------------------------------------------

inline bool SMaterial::getHasPrimitiveColor() const
{
    return m_hasPrimitiveColor;
}

//------------------------------------------------------------------------------

inline void SMaterial::setHasPrimitiveColor(bool _hasPrimitiveColor, const std::string& _textureName)
{
    m_hasPrimitiveColor            = _hasPrimitiveColor;
    m_perPrimitiveColorTextureName = _textureName;
}

//------------------------------------------------------------------------------

inline void SMaterial::setPrimitiveType(::fwData::Mesh::CellTypesEnum _type)
{
    m_primitiveType = _type;
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

inline void SMaterial::setMeshBoundingBox(const Ogre::AxisAlignedBox& _bbox)
{
    m_meshBoundingBox = _bbox;
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SMATERIAL_HPP__
