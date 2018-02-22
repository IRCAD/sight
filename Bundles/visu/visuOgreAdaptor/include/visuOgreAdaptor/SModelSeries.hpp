/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

namespace fwData
{
class Material;
}

namespace visuOgreAdaptor
{

/**
 * @brief   This adaptor shows a modelSeries. It creates an adaptor for each reconstruction in the model.
 *
 * @section Slots Slots
 * - \b showReconstructions(bool): update all reconstructions visibility.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuOgreAdaptor::SModelSeries">
            <in key="model" uid="..." />
            <config transform="transform" material="mat" autoresetcamera="autoresetcamera" dynamic="no" />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b model [::fwData::ModelSeries]: adapted model series.
 * @subsection Configuration Configuration:
 *  - \b transform (mandatory) : the transformation matrix to associate to the adaptor.
 *  - \b material : the name of the base Ogre material to pass to the mesh adaptors.
 *  - \b autoresetcamera (optional, default="yes"): reset the camera when this mesh is modified, "yes" or "no".
 *  - \b dynamic (optional, default=no) : if the modelSeries topolgy is likely to be updated frequently. This is a
 * performance hint that will choose a specific GPU memory pool accordingly.
 *  - \b dynamicVertices (optional, default=no) : if the modelSeries geometry is likely to be updated frequently. This
 * is a performance hint that will choose a specific GPU memory pool accordingly.
 */
class VISUOGREADAPTOR_CLASS_API SModelSeries : public ::fwRenderOgre::IAdaptor,
                                               public ::fwRenderOgre::ITransformable
{

public:

    fwCoreServiceClassDefinitionsMacro( (SModelSeries)(::fwRenderOgre::IAdaptor) )

    /// Constructor.
    VISUOGREADAPTOR_API SModelSeries() noexcept;

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SModelSeries() noexcept;

    /// Returns proposals to connect service slots to associated object signals
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /**
     * @name Slots API
     * @{
     */
    static const ::fwCom::Slots::SlotKeyType s_SHOW_RECONSTRUCTIONS_SLOT;
    /**
     * @}
     */

protected:
    /// Creates a Transform Service, then updates.
    VISUOGREADAPTOR_API void starting() override;
    /// Configure the parameter
    VISUOGREADAPTOR_API void configuring() override;
    /// Redraws all (stops then restarts sub services)
    VISUOGREADAPTOR_API void updating() override;
    /// Closes connections and unregisters service.
    VISUOGREADAPTOR_API void stopping() override;

private:
    /// Slot: update all reconstructions visibility.
    void showReconstructions(bool _show);
    /// Slot: update all reconstructions visibility using "ShowReconstructions" field.
    void showReconstructionsOnFieldChanged();

    /// Defines if the camera must be reset automatically
    bool m_autoResetCamera;
    /// Texture adaptor's UID
    std::string m_textureAdaptorUID;
    /// Material name
    std::string m_materialTemplateName;
    /// Defines if the model series is dynamic
    bool m_isDynamic;
    /// Defines if the model series' vertices are dynamic
    bool m_isDynamicVertices;
    /// Signal/Slot connections with this service
    ::fwCom::helper::SigSlotConnection m_connections;
    /// Mask for picking requests
    std::uint32_t m_queryFlags {0};
};

//------------------------------------------------------------------------------

} // visuOgreAdaptor
