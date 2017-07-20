/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_TEXTURE_HPP__
#define __VISUVTKADAPTOR_TEXTURE_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

namespace fwData
{
class Material;
}

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor to map a texture on a mesh. This is done via ::visuVTKAdaptor::Material. However in the configuration
 *        we don't specify the material adaptor since it is automatically created by the ::visuVTKAdaptor::Mesh adaptor.
 *        Thus we rather try to specify the mesh adaptor.
 */
class VISUVTKADAPTOR_CLASS_API Texture : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (Texture)(::fwRenderVTK::IAdaptor) );

    /**
     * @name Slots API
     * @{
     */
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_APPLY_TEXTURE_SLOT;
    typedef ::fwCom::Slot< void ( SPTR(::fwData::Material) ) > ApplyTextureSlotType;
    /** @} */

    /// Constructor
    VISUVTKADAPTOR_API Texture() noexcept;
    /// Destructor
    VISUVTKADAPTOR_API virtual ~Texture() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    /// Calls doUpdate()
    VISUVTKADAPTOR_API void doStart();

    /**
     * @brief Configure the adaptor.
     * @code{.xml}
       <adaptor id="texAdaptor" class="::visuVTKAdaptor::Texture" objectId="imageKey" >
        <config texture="texture" autoRender="true" meshAdaptor="meshAdaptor" filtering="linear" wrapping="repeat" />
       </adaptor>
       @endcode
     * With :
     *  - \b texture (mandatory) : the vtkTexture to associate to the adaptor
     *  - \b autoRender (optional, "true" by default): if autoRender=true,  the scene is automatically rendered after
     *    doStart, doUpdate, doSwap and doStop if m_vtkPipelineModified=true.
     *  - \b filtering (optional) : filtering of the texture, "nearest" or "linear"
     *  - \b wrapping (optional) : wrapping of the texture, "clamp" or "repeat"
     *  - \b lighting (optional) : enable the lighting, "yes" or "no" default yes
     * And either of the three (adaptor is preferred if several are specified) :
     *  - \b meshAdaptor : the meshAdaptor referring to the mesh you want to map
     *  - \b mesh : a mesh data, in this case the first mesh adaptor linked to it is used
     *  - \b modelSeries : a model series data, the first mesh adaptor of all reconstructions are used
     */
    VISUVTKADAPTOR_API void doConfigure();

    /// Calls doUpdate()
    VISUVTKADAPTOR_API void doSwap();

    /// Updates the vtkTransform from the TransformationMatrix3D
    VISUVTKADAPTOR_API void doUpdate();

    /// Does nothing
    VISUVTKADAPTOR_API void doStop();

    /// Slot called when a texture must be applied on a material.
    void applyTexture( SPTR(::fwData::Material) _material);

    /// Contains all mesh adaptors that currently have this texture applied. Needed when image is updated.
    std::set< SPTR(::fwData::Material) > m_materialSet;

    /// How to filter this texture
    std::string m_filtering;

    /// How to wrap the texture
    std::string m_wrapping;

    /// Slot to call when a texture must be applied on a material.
    ApplyTextureSlotType::sptr m_slotApplyTexture;

    /// enable or not the lighting (default true)
    bool m_lighting;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_TEXTURE_HPP__
