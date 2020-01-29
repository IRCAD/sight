/************************************************************************
 *
 * Copyright (C) 2016-2020 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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
#include <fwRenderOgre/IGraphicsWorker.hpp>
#include <fwRenderOgre/interactor/ClippingBoxInteractor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/vr/IllumAmbientOcclusionSAT.hpp>
#include <fwRenderOgre/vr/PreIntegrationTable.hpp>
#include <fwRenderOgre/vr/RayTracingVolumeRenderer.hpp>

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
 * - \b bufferImage(): Called when the image buffer is modified, copies it into the texture buffer.
 * - \b updateVisibility(bool): Shows or hides the volume.
 * - \b updateClippingBox(): Updates the cropping widget from the clipping matrix.
 * - \b setBoolParameter(bool, string): Calls a bool parameter slot according to the given key.
 *  - preIntegration: Toggles the pre-integration.
 *  - ambientOcclusion: Toggles the ambient occlusion.
 *  - colorBleeding: Toggles the color bleeding.
 *  - shadows: Toggles soft shadows.
 *  - widgets: Toggles the clipping box widget's visibility.
 * - \b setIntParameter(int, string): Calls an int parameter slot according to the given key.
 *  - sampling: Sets the number of volume samples used by the renderer. More samples yield more details but slow down
 * rendering.
 *  - opacityCorrection: Sets the volume opacity correction factor.
 *  - satSizeRatio: Sets the SAT ratio and computes it again with the new corresponding size.
 *  - satShellsNumber: Sets the number of SAT shells and compute the SAT.
 *  - satShellRadius: Sets the SAT shell radius and computes the SAT.
 *  - satConeAngle: Sets the SAT cone angle and computes the SAT.
 *  - satConeSamples: Sets the SAT cone samples number and computes the SAT.
 * - \b setDoubleParameter(double, string): Calls a double parameter slot according to the given key.
 *  - aoFactor: Sets the ambient occlusion factor and computes the SAT.
 *  - colorBleedingFactor: Sets the color bleeding factor and computes the SAT.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SVolumeRender" >
        <inout key="image" uid="..." autoConnect="yes" />
        <inout key="tf" uid="..." optional="yes" />
        <inout key="clippingMatrix" uid="..." />
        <config layer="default"
                samples="1024" preintegration="yes" dynamic="false" ao="no" colorBleeding="no" shadows="no"
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
 * - \b dynamic (optional, true/false, default=false): enables background buffering for dynamic images.
 * - \b widgets (optional, yes/no, default=yes): display VR widgets.
 * - \b widgetPriority (optional, int, default=2): interaction priority of the widget.
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
class VISUOGREADAPTOR_CLASS_API SVolumeRender final : public ::fwRenderOgre::IAdaptor,
                                                      public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceMacro(SVolumeRender, ::fwRenderOgre::IAdaptor)

    /// Creates slots.
    VISUOGREADAPTOR_API SVolumeRender() noexcept;

    /// Destroys the service.
    VISUOGREADAPTOR_API virtual ~SVolumeRender() noexcept;

private:

    /// Volume rendering effects.
    typedef enum
    {
        VR_AMBIENT_OCCLUSION,
        VR_COLOR_BLEEDING,
        VR_SHADOWS
    } VREffectType;

    /// Configures the service.
    virtual void configuring() override final;

    /// Starts the service and initializes scene objects.
    virtual void starting() override final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Image::s_MODIFIED_SIG of s_IMAGE_INOUT to ::visuOgreAdaptor::SVolumeRender::s_NEW_IMAGE_SLOT
     * Connect ::fwData::Image::s_BUFFER_MODIFIED_SIG of s_IMAGE_INOUT to
     *::visuOgreAdaptor::SVolumeRender::s_BUFFER_IMAGE_SLOT
     * Connect ::fwData::Image::s_MODIFIED_SIG of s_CLIPPING_MATRIX_INOUT to
     *::visuOgreAdaptor::SVolumeRender::s_UPDATE_CLIPPING_BOX_SLOT
     */
    virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override final;

    /// Does nothing.
    virtual void updating() override final;

    /**
     * @brief Notifies that the TF is swapped.
     * @param _key key of the swapped data.
     */
    virtual void swapping(const KeyType& _key) override;

    /// Cleans up scene objects.
    virtual void stopping() override;

    /// Updates the transfer function applied to the volume.
    void updateVolumeTF();

    /// Sets a new image.
    void newImage();

    /**
     * @brief Resets the camera coordinates.
     * @param _image data of the input volume.
     */
    void resetCameraPosition(const ::fwData::Image::csptr& _image);

    /// Updates renderer and the GPU volume texture with the new input image data.
    void updateImage();

    /// Starts a parallel task to copy the updated image buffer into the texture buffer.
    void bufferImage();

    /**
     * @brief Updates the sampling.
     * @param _nbSamples number of sample.
     *
     * @pre _nbSamples must fit in a 16 bit unsigned int.
     */
    void updateSampling(int _nbSamples);

    /**
     * @brief Sets the opacity correction.
     * @param _opacityCorrection value of the opacity correction.
     */
    void updateOpacityCorrection(int _opacityCorrection);

    /**
     * @brief Sets the ambient occlusion factor.
     * @param _aoFactor value of the ambient occlusion factor.
     */
    void updateAOFactor(double _aoFactor);

    /**
     * @brief Sets the color bleeding factor.
     * @param _colorBleedingFactor value of the color bleeding factor.
     */
    void updateColorBleedingFactor(double _colorBleedingFactor);

    /**
     * @brief Sets the SAT size ratio.
     * @param _sizeRatio value of the SAT size ratio.
     */
    void updateSatSizeRatio(int _sizeRatio);

    /**
     * @brief Sets the SAT shells number.
     * @param _shellsNumber the number of shells used by the SAT.
     */
    void updateSatShellsNumber(int _shellsNumber);

    /**
     * @brief Sets the SAT shells radius.
     * @param _shellRadius the shells radius used by the SAT.
     */
    void updateSatShellRadius(int _shellRadius);

    /**
     * @brief Sets the SAT cone angle.
     * @param _coneAngle the cone angle size of the SAT. Cones ares used to compute soft shadows.
     */
    void updateSatConeAngle(int _coneAngle);

    /**
     * @brief Sets the SAT cone samples.
     * @param _nbConeSamples the cone sample number of the SAT. Cones ares used to compute soft shadows.
     */
    void updateSatConeSamples(int _nbConeSamples);

    /**
     * @brief Enables/disables the pre integration table.
     * @param _preintegration state of the pre integration.
     */
    void togglePreintegration(bool _preintegration);

    /**
     * @brief Enables/disables the ambient occlision.
     * @param _ambientOcclusion state of the ambient occlusion.
     */
    void toggleAmbientOcclusion(bool _ambientOcclusion);

    /**
     * @brief Enables/disables the color bleeding.
     * @param _colorBleeding state of the color bleeding.
     */
    void toggleColorBleeding(bool _colorBleeding);

    /**
     * @brief Enables/disables the volume self-shadowing.
     * @param _shadows state of soft shadows computation.
     */
    void toggleShadows(bool _shadows);

    /**
     * @brief Displays/Hides the widget.
     * @param _visible state of the widget visibility.
     */
    void toggleWidgets(bool _visible);

    /**
     * @brief Sets the focal distance.
     * @param _focalDistance value of the focal distance.
     */
    void setFocalDistance(int _focalDistance);

    /**
     * @brief Updates a bool parameter.
     * @param _val new value of the boolean parameter.
     * @param _key the key of the parameter. Following keys are accepted:
     *  - preIntegration: Toggles the pre-integration.
     *  @ref togglePreintegration(int) "togglePreintegration(int)"
     *  - ambientOcclusion: Toggles the ambient occlusion.
     *  @ref toggleAmbientOcclusion(int) "toggleAmbientOcclusion(int)"
     *  - colorBleeding: Toggles the color bleeding.
     *  @ref toggleColorBleeding(int) "toggleColorBleeding(int)"
     *  - shadows: Toggles soft shadows.
     *  @ref toggleShadows(int) "toggleShadows(int)"
     *  - widgets: Toggles the clipping box widget's visibility.
     *  @ref toggleWidgets(int) "toggleWidgets(int)"
     */
    void setBoolParameter(bool _val, std::string _key);

    /**
     * @brief Updates a int parameter.
     * @param _val New value of the boolean parameter.
     * @param _key the key of the parameter. Following keys are accepted:
     *  - sampling: Sets the number of volume samples used by the renderer. More samples yield more details but slow
     * down rendering.
     *  @ref updateSampling(int) "updateSampling(int)"
     *  - opacityCorrection: Sets the volume opacity correction factor.
     *  @ref updateOpacityCorrection(int) "updateOpacityCorrection(int)"
     *  - satSizeRatio: Sets the SAT ratio and computes it again with the new corresponding size.
     *  @ref updateSatSizeRatio(int) "updateSatSizeRatio(int)"
     *  - satShellsNumber: Sets the number of SAT shells and compute the SAT.
     *  @ref updateSatShellsNumber(int) "updateSatShellsNumber(int)"
     *  - satShellRadius: Sets the SAT shell radius and computes the SAT.
     *  @ref updateSatShellRadius(int) "updateSatShellRadius(int)"
     *  - satConeAngle: Sets the SAT cone angle and computes the SAT.
     *  @ref updateSatConeAngle(int) "updateSatConeAngle(int)"
     *  - satConeSamples: Sets the SAT cone samples number and computes the SAT.
     *  @ref updateSatConeSamples(int) "updateSatConeSamples(int)"
     */
    void setIntParameter(int _val, std::string _key);

    /**
     * @brief Updates a double parameter.
     * @param _val New value of the boolean parameter.
     * @param _key the key of the parameter. Following keys are accepted:
     *  - aoFactor: Sets the ambient occlusion factor and computes the SAT.
     *  @ref updateAOFactor(int) "updateAOFactor(int)"
     *  - colorBleedingFactor: Sets the color bleeding factor and computes the SAT.
     *  @ref updateColorBleedingFactor(int) "updateColorBleedingFactor(int)"
     */
    void setDoubleParameter(double _val, std::string _key);

    /// Sets the volume to be visible or not.
    void updateVisibility(bool _visibility);

    /// Creates widgets and connects its slots to interactor signals.
    void createWidget();

    /// Removes the widgets from the interactor and deletes it.
    void destroyWidget();

    /// Computes the volume illumination and applies it to the ray tracing renderer.
    void updateVolumeIllumination();

    /// Updates or creates the illumination volume according to the given VR effect.
    void toggleVREffect(VREffectType _vrEffect);

    /// Updates the clipping box position from the inout clipping matrix.
    void updateClippingBox();

    /// Updates the inout clipping matrix from the clipping box positions.
    void updateClippingTM3D();

    /// Helps to manage the optional volume TF.
    ::fwDataTools::helper::TransferFunction m_helperVolumeTF;

    /// Implements a simple GPU ray-tracing renderer.
    ::fwRenderOgre::vr::RayTracingVolumeRenderer* m_volumeRenderer { nullptr };

    /// Contains the GPU 3D Image texture.
    ::Ogre::TexturePtr m_3DOgreTexture;

    /// Contains the buffering texture for the 3D image.
    ::Ogre::TexturePtr m_bufferingTexture;

    /// Prevents the service from accessing the textures while they are swapped.
    std::mutex m_bufferSwapMutex;

    /// Fills the incoming image texture in a parallel thread.
    std::unique_ptr< ::fwRenderOgre::IGraphicsWorker > m_bufferingWorker;

    /// Contains the GPU TF texture used for rendering.
    ::fwRenderOgre::TransferFunction::sptr m_gpuVolumeTF;

    /// Contains the pre-integration table.
    ::fwRenderOgre::vr::PreIntegrationTable m_preIntegrationTable;

    /// Stores the scene manager.
    ::Ogre::SceneManager* m_sceneManager { nullptr };

    /// Stores the scene node of the volume.
    ::Ogre::SceneNode* m_volumeSceneNode { nullptr };

    /// Stores the camera used for rendering.
    ::Ogre::Camera* m_camera { nullptr };

    /// Stores the widgets used for clipping.
    std::shared_ptr< ::fwRenderOgre::interactor::ClippingBoxInteractor > m_widget;

    /// Stores the priority of the widget interactor.
    int m_widgetPriority { 2 };

    /// Stores the sampling rate.
    std::uint16_t m_nbSamples { 512 };

    /// Enables the pre-integration usage.
    bool m_preIntegratedRendering { false };

    /// Enables background buffering for dynamic images.
    bool m_dynamic { false };

    /// Sets usage of ambient occlusion.
    bool m_ambientOcclusion { false };

    /// Sets usage of color bleeding.
    bool m_colorBleeding { false };

    /// Sets usage of soft shadows.
    bool m_shadows { false };

    /// Toggles widget visibility.
    bool m_widgetVisibilty { true };

    /// Contains the ilumination volume used to render shadows and ambient occlusion.
    std::shared_ptr< ::fwRenderOgre::vr::IllumAmbientOcclusionSAT> m_ambientOcclusionSAT;

    /// Stores the ratio used to determine the size of the SAT regarding of the associated image size.
    float m_satSizeRatio {0.25f };

    /// Stores the number of shells used to compute the volume illumination from the SAT.
    int m_satShells { 4 };

    /// Stores the radius of the shells used to compute the volume illumination from the SAT.
    int m_satShellRadius { 4 };

    /// Stores the angle used to define the soft shadows cones.
    float m_satConeAngle { 0.1f };

    /// Number of samples along the soft shadows cones.
    int m_satConeSamples { 50 };

    /// Stores the factor parameter used to weight the ambient occlusion.
    double m_aoFactor { 1. };

    /// Stores the factor parameter used to weight the color bleeding.
    double m_colorBleedingFactor { 1. };

    /// Enables whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera { true };

};

} // visuOgreAdaptor
