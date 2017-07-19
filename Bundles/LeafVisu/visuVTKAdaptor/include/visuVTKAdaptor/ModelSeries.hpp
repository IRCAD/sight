/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_MODELSERIES_HPP__
#define __VISUVTKADAPTOR_MODELSERIES_HPP__

#include <vector>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{


/**
 * @brief This adaptor shows ModelSeries. Creates adaptors for each reconstruction in model.
 * @class ModelSeries
 */
class VISUVTKADAPTOR_CLASS_API ModelSeries : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ModelSeries)(::fwRenderVTK::IVtkAdaptorService) );

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal< void ( SPTR(::fwData::Material) ) > TextureAppliedSignalType;
    VISUVTKADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_TEXTURE_APPLIED_SIG;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_NORMAL_MODE_SLOT;
    /// normal mode (0: none, 1: point, 2: cell), reconstruction fwID
    typedef ::fwCom::Slot<void (std::uint8_t, std::string)> UpdateNormalModeSlotType;

    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SHOW_RECONSTRUCTIONS_SLOT;
    typedef ::fwCom::Slot<void (bool)> ShowReconstructionsSlotType;
    /**
     * @}
     */

    VISUVTKADAPTOR_API ModelSeries() noexcept;

    VISUVTKADAPTOR_API virtual ~ModelSeries() noexcept;

    void setClippingPlanes(::fwRenderVTK::SRender::VtkObjectIdType id)
    {
        m_clippingPlanes = id;
    }

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect ModelSeries::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    /// redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate();
    VISUVTKADAPTOR_API void doStop();

    ::fwRenderVTK::SRender::VtkObjectIdType m_clippingPlanes;

    /**
     * @name Slots methods
     * @{
     */

    /**
     * Slot: used to update normal display for the adaptor on object with givent uid 'recID'
     * (0: none, 1: point, 2: cell)
     */
    void updateNormalMode(std::uint8_t mode, std::string recID);

    /// Slot: show(or hide) reconstructions
    void showReconstructions(bool show);
    /**
     * @}
     */


private:
    bool m_autoResetCamera;
    std::string m_textureAdaptorUID;

    /// Signal to emit when a texture must be applied on a material.
    TextureAppliedSignalType::sptr m_sigTextureApplied;


    /**
     * @name Slots attributes
     * @{
     */
    /// Slot to update normal diplay (0: none, 1: point, 2: cell)
    UpdateNormalModeSlotType::sptr m_slotUpdateNormalMode;

    /// Slot to show(or hide) reconstructions
    ShowReconstructionsSlotType::sptr m_slotShowReconstructions;
    /**
     * @}
     */
};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_MODELSERIES_HPP__
