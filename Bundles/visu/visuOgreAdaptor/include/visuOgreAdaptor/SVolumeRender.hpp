/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwDataTools/helper/TransferFunction.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/ui/VRWidget.hpp>
#include <fwRenderOgre/vr/PreIntegrationTable.hpp>
#include <fwRenderOgre/vr/RayTracingVolumeRenderer.hpp>
#include <fwRenderOgre/vr/SATVolumeIllumination.hpp>

#include <OGRE/OgreTexture.h>

#include <array>
#include <vector>

namespace visuOgreAdaptor
{

/**
 * @brief Adaptor for volume rendering.
 *
 * @section Slots Slots
 * - \b newImage(): Called when a new image is loaded.
 * - \b updateImage(): Called when the image is updated.
 * - \b toggleWidgets(bool): Toggles widget visibility.
 * - \b updateVisibility(bool): Shows or hides the volume.
 * - \b updateClippingBox(): Updates the cropping widget from the clipping matrix.
 * - \b setBoolParameter(bool, string): Calls a bool parameter slot according to the given key.
 *   - preIntegration: Toggle pre-integration.
 *   - ambientOcclusion: Toggle ambient occlusion.
 *   - colorBleeding: Toggle color bleeding.
 *   - shadows: Toggle soft shadows.
 *   - widgets: Toggle the clipping box widget's visibility.
 * - \b setIntParameter(int, string): Calls an int parameter slot according to the given key.
 *   - sampling: Sets the number of volume samples used by the renderer. More samples yield more details but slow down
 * rendering.
 *   - opacityCorrection: Sets the volume opacity correction factor.
 *   - satSizeRatio: Sets the SAT ratio and computes it again with the new corresponding size.
 *   - satShellsNumber: Sets the number of SAT shells and compute the SAT.
 *   - satShellRadius: Sets the SAT shell radius and computes the SAT.
 *   - satConeAngle: Sets the SAT cone angle and computes the SAT.
 *   - satConeSamples: Sets the SAT cone samples number and computes the SAT.
 * - \b setDoubleParameter(double, string): Calls a double parameter slot according to the given key.
 *   - aoFactor: Sets the ambient occlusion factor and computes the SAT.
 *   - colorBleedingFactor: Sets the color bleeding factor and computes the SAT.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SVolumeRender" >
        <inout key="image" uid="..." autoConnect="yes" />
        <inout key="tf" uid="..." optional="yes" />
        <inout key="clippingMatrix" uid="..." />
        <config layer="default"
                samples="1024" preintegration="yes" ao="no" colorBleeding="no" shadows="no"
                satSizeRatio="0.25" satShells="3" satShellRadius="7" satConeAngle="0.1" satConeSamples="50"
                aoFactor="0.5" colorBleedingFactor="0.5" autoresetcamera="yes" transform="..."/>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: input volume data.
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 * image's default transferFunction (CT-GreyLevel).
 * - \b clippingMatrix [::fwData::TransformationMatrix3D]: matrix used to clip the volume.
 *
 * @subsection Configuration Configuration
 * - \b layer (mandatory): id of the layer where this adaptor applies.
 * - \b samples (optional, default=512): maximum number of samples per ray or number of slices.
 * - \b preintegration (optional, yes/no, default=no): use pre-integration.
 * - \b widgets (optional, yes/no, default=yes): display VR widgets.
 * - \b ao (optional, true/false, default=false): Ambient occlusion usage.
 * - \b colorBleeding (optional, true/false, default=false): Color bleeding usage.
 * - \b shadows (optional, true/false, default=false): Soft shadows usage.
 * - \b satSizeRatio (optional, float, default=0.25): ratio used to determine the size of the SAT regarding of the
 * associated image size.
 * - \b satShells (optional, int, default=3): number of shells used to compute the volume illumination from the SAT.
 * - \b satShellRadius (optional, int, default=7): radius of the shells used to compute the volume illumination from the
 * SAT.
 * - \b satConeAngle (optional, float, default=0.1): angle used to define the soft shadows cones.
 * - \b satConeSamples (optional, float, default=50): number of samples along the soft shadows cones.
 * - \b aoFactor (optional, double, default=1.0): factor used to weight the ambient occlusion.
 * - \b colorBleedingFactor (optional, double, default=1.0): factor used to weight the color bleeding.
 * - \b autoresetcamera (optional, yes/no, default=yes): reset the camera at image update to view the whole volume.
 * - \b transform (optional): transform applied to the adaptor's scene node
 */
class VISUOGREADAPTOR_CLASS_API SVolumeRender : public ::fwRenderOgre::IAdaptor,
                                                public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceClassDefinitionsMacro( (SVolumeRender)(::fwRenderOgre::IAdaptor) )

    /// Constructor.
    VISUOGREADAPTOR_API SVolumeRender() noexcept;

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SVolumeRender() noexcept;

private:

    /// Volume rendering effects.
    typedef enum
    {
        VR_AMBIENT_OCCLUSION,
        VR_COLOR_BLEEDING,
        VR_SHADOWS
    } VREffectType;

    /// Starts the service initializes scene objects.
    virtual void starting() override;

    /// Cleans up memory.
    virtual void stopping() override;

    /// Does nothing.
    virtual void updating() override;

    /// Configures the service.
    virtual void configuring() override;

    /// Retrieves the current transfer function.
    virtual void swapping(const KeyType& key) override;

    /// Returns proposals to connect service slots to associated object signals.
    virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Updates the transfer function applied to the volume.
    void updateVolumeTF();

    /// Sets a new image.
    void newImage();

    /// Reset the camera coordinates.
    void resetCameraPosition(const ::fwData::Image::csptr& image);

    /// Updates renderer and the GPU volume texture with the new input image data.
    void updateImage();

    /// Updates the sampling.
    void updateSampling(int nbSamples);

    /// Sets the opacity correction.
    void updateOpacityCorrection(int opacityCorrection);

    /// Sets the ambient occlusion factor.
    void updateAOFactor(double aoFactor);

    /// Sets the color bleeding factor.
    void updateColorBleedingFactor(double colorBleedingFactor);

    /// Sets the SAT size ration.
    void updateSatSizeRatio(int sizeRatio);

    /// Sets the SAT shells number.
    void updateSatShellsNumber(int shellsNumber);

    /// Sets the SAT shells radius.
    void updateSatShellRadius(int shellRadius);

    /// Sets the SAT cone angle.
    void updateSatConeAngle(int coneAngle);

    /// Sets the SAT cone samples.
    void updateSatConeSamples(int nbConeSamples);

    /// Enables/disables the pre integration table.
    void togglePreintegration(bool preintegration);

    /// Enables/disables the ambient occlision.
    void toggleAmbientOcclusion(bool ambientOcclusion);

    /// Enables/disables the color bleeding.
    void toggleColorBleeding(bool colorBleeding);

    /// Enables/disables the volume self-shadowing.
    void toggleShadows(bool shadows);

    /// Displays/Hides the widget.
    void toggleWidgets(bool visible);

    /// Sets the focal distance.
    void setFocalDistance(int focalDistance);

    /// Updates a bool parameter.
    void setBoolParameter(bool val, std::string key);

    /// Updates a int parameter.
    void setIntParameter(int val, std::string key);

    /// Updates a double parameter.
    void setDoubleParameter(double val, std::string key);

    /// Sets the volume to be visible or not.
    void updateVisibility(bool visibility);

    /// Creates widgets and connects its slots to interactor signals.
    void createWidget();

    /// Removes the widgets from the interactor and deletes it.
    void destroyWidget();

    /// Computes the volume illumination and applies it to the ray tracing renderer.
    void updateVolumeIllumination();

    /// Updates or creates the illumination volume according to the given VR effect.
    void toggleVREffect(VREffectType vrEffect);

    /// Updates the clipping box position from the inout clipping matrix.
    void updateClippingBox();

    /// Updates the inout clipping matrix from the clipping box positions.
    void updateClippingTM3D();

    /// Helper to manage the volume TF.
    ::fwDataTools::helper::TransferFunction m_helperVolumeTF;

    /// Renders the volume.
    ::fwRenderOgre::vr::RayTracingVolumeRenderer* m_volumeRenderer { nullptr };

    /// Double 3D Image texture.
    ::Ogre::TexturePtr m_double3DOgreTexture[2];

    /// TF texture used for rendering.
    ::fwRenderOgre::TransferFunction::sptr m_gpuVolumeTF;

    /// Pre-integration table.
    ::fwRenderOgre::vr::PreIntegrationTable m_preIntegrationTable;

    /// This object's scene manager.
    ::Ogre::SceneManager* m_sceneManager { nullptr };

    /// This object's scene node.
    ::Ogre::SceneNode* m_volumeSceneNode { nullptr };

    /// Camera used for rendering.
    ::Ogre::Camera* m_camera { nullptr };

    /// Widgets used for clipping.
    ::fwRenderOgre::ui::VRWidget::sptr m_widget;

    /// Sampling rate.
    std::uint16_t m_nbSamples { 512 };

    /// Use pre-integration.
    bool m_preIntegratedRendering { false };

    /// Sets usage of ambient occlusion.
    bool m_ambientOcclusion { false };

    /// Sets usage of color bleeding.
    bool m_colorBleeding { false };

    /// Sets usage of soft shadows.
    bool m_shadows { false };

    /// Toggles widget visibility.
    bool m_widgetVisibilty { true };

    /// Illumination volume used to render shadows and ambient occlusion.
    std::shared_ptr< ::fwRenderOgre::vr::SATVolumeIllumination> m_illum;

    /// Ratio used to determine the size of the SAT regarding of the associated image size.
    float m_satSizeRatio {0.25f };

    /// Number of shells used to compute the volume illumination from the SAT.
    int m_satShells { 4 };

    /// Radius of the shells used to compute the volume illumination from the SAT.
    int m_satShellRadius { 4 };

    /// Angle used to define the soft shadows cones.
    float m_satConeAngle { 0.1f };

    /// Number of samples along the soft shadows cones.
    int m_satConeSamples { 50 };

    /// Factor parameter used to weight the ambient occlusion.
    double m_aoFactor { 1. };

    /// Factor parameter used to weight the color bleeding.
    double m_colorBleedingFactor { 1. };

    /// Sets whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera { true };

    /// Handle connections between the layer and the volume renderer.
    ::fwCom::helper::SigSlotConnection m_volumeConnection;

    /// Use of the first texture.
    bool m_useTextureOne { true };
};

} // visuOgreAdaptor
