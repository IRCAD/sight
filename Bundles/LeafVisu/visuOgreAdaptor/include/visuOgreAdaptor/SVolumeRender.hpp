/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
#define __VISUOGREADAPTOR_SVOLUMERENDER_HPP__

#include <array>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/vr/PreIntegrationTable.hpp>
#include <fwRenderOgre/TransferFunction.hpp>
#include <fwRenderOgre/ui/VRWidget.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgrePolygon.h>
#include <OGRE/OgreTexture.h>

#include <vector>

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/vr/SATVolumeIllumination.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief Adaptor for volume rendering.
 *
 * @section Slots Slots
 * - \b newImage(): Called when a new image is loaded.
 * - \b updateSampling(int): Called when the sampling is changed and updates the volume renderer accordingly.
 * - \b updateSatSizeRatio(int) : Called when the SAT ratio is changed and computes it again with the new corresponding
 *      size.
 * - \b updateSatShellsNumber(int) : Called when the number of SAT shells is changed and compute the SAT again.
 * - \b updateSatShellRadius(int) : Called when the SAT shell radius is changed and computes the SAT again.
 * - \b updateSatConeAngle(int) : Called when the SAT cone angle is changed and computes the SAT again.
 * - \b updateSatConeSamples(int) : Called when the SAT cone samples number is changed and computes the SAT again.
 * - \b togglePreintegration(bool): Toggle pre-integration.
 * - \b toggleVoumeIllumination(bool): Toggle volume illumination.
 * - \b toggleWidgets(bool): Toggles widget visibility.
 * - \b resizeViewport(int, int): Called when the size of the viewport changes.
 * - \b setFocalDistance(int): Called to modify focal length (only useful for stereoscopic 3D).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <adaptor uid="volumeRender" class="::visuOgreAdaptor::SVolumeRender" objectId="image">
         <config renderer="default"
                 preintegration="yes" mode="slice" volumeIllumination="no"
                 satSizeRatio="0.25" satShells="3" satShellRadius="7" satConeAngle="0.1" satConeSamples="50"
                 selectedTFKey="SelectedTF" tfSelectionFwID="TFSelections" />
    </adaptor>
   @endcode
 * With :
 * - \b renderer (optional): defines the renderer displaying the volume.
 * - \b preintegration (optional, yes/no, default=no): use pre-integration.
 * - \b widgets (optional, yes/no, default=yes): display VR widgets.
 * - \b mode (optional, slice/raycasting, default=raycasting): Rendering mode.
 * - \b volumeIllumination (optional, yes/no, default=no): Volume Illumination usage (ambient occlusion + color
 *      bleeding). Only available with raycasting render mode.
 * - \b selectedTFKey (mandatory): TF key.
 * - \b tfSelectionFwID (mandatory): TF selection.
 * Only if the raycasting render mode is activated :
 * - \b volumeIllumination (optional, yes/no, default=no): Volume Illumination usage (ambient occlusion + color
 *      bleeding).
 * - \b satSizeRatio (optional, float, default=0.25): ratio used to determine the size of the SAT regarding of the
 *      associated image size.
 * - \b satShells (optional, int, default=3): number of shells used to compute the volume illumination from the SAT.
 * - \b satShellRadius (optional, int, default=7): radius of the shells used to compute the volume illumination from the
 *      SAT.
 * - \b satConeAngle (optional, float, default=0.1): angle used to define the soft shadows cones.
 * - \b satConeSamples (optional, float, default=50): number of samples along the soft shadows cones.
 */
class VISUOGREADAPTOR_CLASS_API SVolumeRender : public ::fwRenderOgre::IAdaptor,
                                                public ::fwRenderOgre::ITransformable,
                                                public ::fwComEd::helper::MedicalImageAdaptor
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SVolumeRender)(::fwRenderOgre::IAdaptor) );

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_NEW_IMAGE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SAMPLING_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SAT_SIZE_RATIO_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SAT_SHELLS_NUMBER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SAT_SHELL_RADIUS_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SAT_CONE_ANGLE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SAT_CONE_SAMPLES_NUMBER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_PREINTEGRATION_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_VOLUME_ILLUMINATION_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_WIDGETS_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_RESIZE_VIEWPORT_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_FOCAL_DISTANCE_SLOT;
    /** @} */

    /// Constructor.
    VISUOGREADAPTOR_API SVolumeRender() throw();

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SVolumeRender() throw();

protected:

    /// Starts the service initializes scene objects.
    VISUOGREADAPTOR_API virtual void doStart() throw ( ::fwTools::Failed );

    /// Cleans up memory.
    VISUOGREADAPTOR_API virtual void doStop() throw ( ::fwTools::Failed );

    /// Does nothing.
    VISUOGREADAPTOR_API virtual void doSwap() throw ( ::fwTools::Failed );

    /// Does nothing.
    VISUOGREADAPTOR_API virtual void doUpdate() throw ( ::fwTools::Failed );

    /// Configures the service
    VISUOGREADAPTOR_API virtual void doConfigure() throw ( ::fwTools::Failed );

    /// Slot called on TF update.
    VISUOGREADAPTOR_API virtual void updatingTFPoints();

    /// Slot called on TF window update.
    VISUOGREADAPTOR_API virtual void updatingTFWindowing(double window, double level);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connects fwData::Image::s_MODIFIED_SIG to this::s_NEWIMAGE_SLOT
     */
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsType getObjSrvConnections() const;

private:

    void newImage();
    void updateSampling(int nbSamples);
    void updateSatSizeRatio(int sizeRatio);
    void updateSatShellsNumber(int shellsNumber);
    void updateSatShellRadius(int shellRadius);
    void updateSatConeAngle(int coneAngle);
    void updateSatConeSamplesNumber(int nbConeSamples);
    void togglePreintegration(bool preintegration);
    void toggleVolumeIllumination(bool volumeIllumination);
    void toggleWidgets(bool visible);
    void resizeViewport(int w, int h);
    void setFocalDistance(int focalDistance);

    /// Creates widgets and connects its slots to interactor signals.
    void initWidgets();

    /// Computes the volume illumination and applies it to the ray tracing renderer
    void updateVolumeIllumination();

    /// Rendering mode.
    enum
    {
        VR_MODE_SLICE,
        VR_MODE_RAY_TRACING
    } m_renderingMode;

    /// Renders the volume.
    ::fwRenderOgre::vr::IVolumeRenderer* m_volumeRenderer;

    /// 3D Image texture.
    ::Ogre::TexturePtr m_3DOgreTexture;

    /// TF texture used for rendering.
    ::fwRenderOgre::TransferFunction m_gpuTF;

    /// Pre-integration table.
    ::fwRenderOgre::vr::PreIntegrationTable m_preIntegrationTable;

    /// This object's scene manager.
    ::Ogre::SceneManager* m_sceneManager;

    /// This object's scene node.
    ::Ogre::SceneNode* m_volumeSceneNode;

    /// Camera used for rendering.
    ::Ogre::Camera* m_camera;

    /// Widgets used for clipping.
    ::fwRenderOgre::ui::VRWidget::sptr m_widgets;

    /// Sampling rate.
    uint16_t m_nbSlices;

    /// Use pre-integration.
    bool m_preIntegratedRendering;

    /// Use volume illumination
    bool m_volumeIllumination;

    /// Toggles widget visibility.
    bool m_widgetVisibilty;

    /// Illumination volume used to render shadows and ambient occlusion.
    ::fwRenderOgre::vr::SATVolumeIllumination* m_illum;

    /// Ratio used to determine the size of the SAT regarding of the associated image size.
    float m_satSizeRatio;

    /// Number of shells used to compute the volume illumination from the SAT.
    int m_satShells;

    /// Radius of the shells used to compute the volume illumination from the SAT.
    int m_satShellRadius;

    /// Angle used to define the soft shadows cones.
    float m_satConeAngle;

    /// Number of samples along the soft shadows cones.
    int m_satConeSamples;

    /// Handle connections between the layer and the volume renderer.
    ::fwServices::helper::SigSlotConnection m_volumeConnection;
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
