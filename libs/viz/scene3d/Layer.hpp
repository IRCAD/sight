/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "viz/scene3d/config.hpp"

#include <core/com/HasSignals.hpp>
#include <core/com/HasSlots.hpp>
#include <core/com/helper/SigSlotConnection.hpp>
#include <core/com/Slot.hpp>
#include <core/thread/Worker.hpp>

#include <viz/scene3d/compositor/ChainManager.hpp>
#include <viz/scene3d/compositor/Core.hpp>
#include <viz/scene3d/compositor/listener/AutoStereo.hpp>
#include <viz/scene3d/compositor/types.hpp>
#include <viz/scene3d/interactor/IInteractor.hpp>
#include <viz/scene3d/IWindowInteractor.hpp>

#include <OGRE/Ogre.h>
#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreViewport.h>

#include <cstdint>
#include <map>
#include <vector>

namespace sight::data
{

class Color;

} // namespace sight::data

namespace sight::viz::scene3d
{

class SRender;
class IAdaptor;
class ILight;

} // namespace sight::viz::scene3d

namespace Ogre
{

class Camera;

} // namespace Ogre

namespace sight::viz::scene3d
{

/**
 * @brief Allows to render multiple scenes in the same render window with viewports.
 */
class VIZ_SCENE3D_CLASS_API Layer :
    public core::BaseObject,
    public core::com::HasSignals,
    public core::com::HasSlots
{
public:

    /// Generates default methods.
    SIGHT_DECLARE_CLASS(Layer, viz::scene3d::Layer, new Layer);

    SIGHT_ALLOW_SHARED_FROM_THIS()

    /// Defines the extrinsic x Intrinsic camera calibrations.
    using CameraCalibrationsType = std::vector<Ogre::Matrix4>;

    /// Defines the viewport parameters relatively to the screen: left, top, width ,height.
    using ViewportConfigType = std::tuple<float, float, float, float>;

    /// Contains the signal sent when the layer is initialized.
    VIZ_SCENE3D_API static const core::com::Signals::SignalKeyType s_INIT_LAYER_SIG;
    typedef core::com::Signal<void (viz::scene3d::Layer::sptr)> InitLayerSignalType;

    /// Contains the signal sent when the layer is resized.
    VIZ_SCENE3D_API static const core::com::Signals::SignalKeyType s_RESIZE_LAYER_SIG;
    typedef core::com::Signal<void (int, int)> ResizeLayerSignalType;

    /// Contains signals sent when the camera is modified.
    VIZ_SCENE3D_API static const core::com::Signals::SignalKeyType s_CAMERA_RANGE_UPDATED_SIG;
    typedef core::com::Signal<void ()> CameraUpdatedSignalType;

    typedef core::com::Slot<void (viz::scene3d::IWindowInteractor::InteractionInfo)> InteractionSlotType;
    typedef core::com::Slot<void ()> DestroySlotType;

    /// Contains the slot name that request the picker to do a ray cast according to the passed position.
    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_INTERACTION_SLOT;

    /// Contains the slot name that request the reset of camera.
    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_RESET_CAMERA_SLOT;

    /// Defines the default camera name.
    VIZ_SCENE3D_API static const std::string s_DEFAULT_CAMERA_NAME;

    /// Defines the default light name.
    VIZ_SCENE3D_API static const std::string s_DEFAULT_LIGHT_NAME;

    /// Defines the default camera node name.
    VIZ_SCENE3D_API static const std::string s_DEFAULT_CAMERA_NODE_NAME;

    /// Initializes signals and slots.
    VIZ_SCENE3D_API Layer();

    /// Destoyres Ogre resources.
    VIZ_SCENE3D_API ~Layer() override = default;

    /// Gets the render window containing this layer.
    VIZ_SCENE3D_API Ogre::RenderTarget* getRenderTarget() const;

    /// Sets the render window containing this layer.
    VIZ_SCENE3D_API void setRenderTarget(Ogre::RenderTarget* _renderTarget);

    /// Set the associated scene manager ID of this viewport
    VIZ_SCENE3D_API void setID(const std::string& id);

    /// @returns the name of this layer.
    VIZ_SCENE3D_API std::string getName() const;

    /// @returns the ID of this layer.
    VIZ_SCENE3D_API const std::string& getLayerID() const;

    /// @returns the scene manager associated to this viewport. Creates if it does not exists.
    VIZ_SCENE3D_API Ogre::SceneManager* getSceneManager() const;

    /// Creates the scene.
    VIZ_SCENE3D_API void createScene();

    /// Destroys the scene.
    VIZ_SCENE3D_API void destroyScene();

    /// @returns true if the scene is created.
    VIZ_SCENE3D_API bool isSceneCreated() const;

    /// Adds a disabled compositor name to the ChainManager.
    VIZ_SCENE3D_API void addAvailableCompositor(std::string compositorName);

    /// Enables/Disables a compositor according to the isEnabled flag.
    VIZ_SCENE3D_API void updateCompositorState(std::string compositorName, bool isEnabled);

    /// Places and align camera's focal with the world boundingBox.
    VIZ_SCENE3D_API void resetCameraCoordinates();

    /// Computes camera's focal with the world boundingBox.
    VIZ_SCENE3D_API void computeCameraParameters();

    /// Resets the camera clipping range (near and far).
    VIZ_SCENE3D_API void resetCameraClippingRange() const;

    /// Appends a new interactor with the given priority. Interactors with higher priorities are executed first.
    VIZ_SCENE3D_API void addInteractor(
        const viz::scene3d::interactor::IInteractor::sptr& _interactor,
        int _priority = 0
    );

    /// Removes the given interactor.
    VIZ_SCENE3D_API void removeInteractor(const viz::scene3d::interactor::IInteractor::sptr& _interactor);

    /// @return the order of the layer.
    VIZ_SCENE3D_API int getOrder() const;

    /// Sets the order of the layer.
    VIZ_SCENE3D_API void setOrder(int _order);

    /// Sets the worker used by slots.
    VIZ_SCENE3D_API void setWorker(const core::thread::Worker::sptr& _worker);

    /// @returns the render service.
    VIZ_SCENE3D_API SPTR(viz::scene3d::SRender) getRenderService() const;

    /// Sets the render service.
    VIZ_SCENE3D_API void setRenderService(const SPTR(viz::scene3d::SRender)& _service);

    /// Requests render.
    VIZ_SCENE3D_API void requestRender();

    /// Sets stereoscopic rendering.
    VIZ_SCENE3D_API void setStereoMode(compositor::Core::StereoModeType mode);

    /// Sets background color : specific to background Layer.
    VIZ_SCENE3D_API void setBackgroundColor(std::string topColor, std::string botColor);

    /// Sets background scale : specific to background Layer.
    VIZ_SCENE3D_API void setBackgroundScale(float topScale, float botScale);

    /// Sets background scale : specific to background Layer.
    VIZ_SCENE3D_API void setBackgroundMaterial(const std::string& background);

    /// Sets if this layer need a layer's 3D scene.
    VIZ_SCENE3D_API void setCoreCompositorEnabled(
        bool enabled,
        std::string transparencyTechnique           = "",
        std::string numPeels                        = "",
        compositor::Core::StereoModeType stereoMode =
        compositor::Core::StereoModeType::NONE
    );

    /// Sets if this layer has a configured compositor chain.
    VIZ_SCENE3D_API void setCompositorChainEnabled(const std::string& compositorChain);

    /// Sets the viewport parameters for this layer: left, top, width, height.
    VIZ_SCENE3D_API void setViewportConfig(const ViewportConfigType& _vpCfg);

    /// @returns true if this layer needs a layer's 3D scene.
    VIZ_SCENE3D_API bool isCoreCompositorEnabled() const;

    /// @returns true if there is an XML configured compositor chain.
    VIZ_SCENE3D_API bool isCompositorChainEnabled() const;

    /// @returns true if stereoscopic rendering is enabled.
    VIZ_SCENE3D_API bool is3D() const;

    /// @returns true if the layer is initialized.
    VIZ_SCENE3D_API bool initialized() const;

    /// @returns the stereoscopic mode.
    VIZ_SCENE3D_API compositor::Core::StereoModeType getStereoMode() const;

    /// @returns the compositor chain.
    VIZ_SCENE3D_API viz::scene3d::compositor::ChainManager::CompositorChainType getCompositorChain() const;

    /// @returns the list of adaptors in the chain manager.
    VIZ_SCENE3D_API service::IHasServices::ServiceVector getRegisteredAdaptors() const;

    /// @returns the viewport.
    VIZ_SCENE3D_API Ogre::Viewport* getViewport() const;

    /// @returns the default camera.
    VIZ_SCENE3D_API Ogre::Camera* getDefaultCamera() const;

    /// Gets the projection matrix used to define nth viewpoint. The index must be lower than the number of viewpoints.
    VIZ_SCENE3D_API Ogre::Matrix4 getCameraProjMat(std::uint8_t cameraIdx) const;

    /// @returns the number of cameras (viewpoints) used by this layer. Defined by the stereo mode.
    VIZ_SCENE3D_API std::uint8_t numCameras() const;

    /// Sets default light flag.
    VIZ_SCENE3D_API void setHasDefaultLight(bool hasDefaultLight);

    /// @returns the number of lights adaptors used in this layer.
    VIZ_SCENE3D_API int getLightsNumber() const;

    /// @returns the light adaptors used in this layer.
    VIZ_SCENE3D_API std::vector<SPTR(viz::scene3d::ILight)> getLightAdaptors() const;

    /// @returns the computed bounding box of the scene.
    VIZ_SCENE3D_API Ogre::AxisAlignedBox computeWorldBoundingBox() const;

    /// @returns the OIT selected.
    VIZ_SCENE3D_API compositor::transparencyTechnique getTransparencyTechnique();

    /// @returns the number of peels computed by Depth Peeling or x2 Dual Depth Peeling.
    VIZ_SCENE3D_API int getTransparencyDepth();

    /// Sets the OIT desired. Deactivate OIT compositor.
    VIZ_SCENE3D_API bool setTransparencyTechnique(compositor::transparencyTechnique technique);

    /// Sets the number of peels computed by Depth Peeling or x2 Dual Depth Peeling. Deactivate OIT compositor.
    VIZ_SCENE3D_API void setTransparencyDepth(int depth);

    /// Sets the camera calibrations for stereo rendering.
    VIZ_SCENE3D_API void setCameraCalibrations(const CameraCalibrationsType& calibrations);

    /// @returns true if a specified light is the default light in the layer.
    VIZ_SCENE3D_API bool isDefaultLight(const CSPTR(viz::scene3d::ILight)& /*_light*/) const;

    /// Removes the default light in the layer.
    VIZ_SCENE3D_API void removeDefaultLight();

    /// Cancels interaction for all interactors with a lower priority than the one calling this.
    VIZ_SCENE3D_API void cancelFurtherInteraction();

    /// Need to be enable if using SCamera with orthographic mode.
    VIZ_SCENE3D_API void setOrthographicCamera(bool _ortho);
    /// Returns value of setOrthographicCamera.
    VIZ_SCENE3D_API bool isOrthographicCameraForce() const;

private:

    /// Slot: interacts with the scene.
    void interaction(viz::scene3d::IWindowInteractor::InteractionInfo /*info*/);

    /// Compute the scene size, notably in order to determine the zoom scale
    float computeSceneLength(const Ogre::AxisAlignedBox& worldBoundingBox);

    /// Setups default compositor for a layer's 3D scene.
    void setupCore();

    /// Stops and starts all adaptors belonging to this layer. Subadaptors are expected to be managed by their parent.
    void restartAdaptors();

    /// Calls a function on all interactors and deletes the ones that expired.
    void forAllInteractors(const std::function<void(const interactor::IInteractor::sptr&)>&& _f);

    /// Contains the Ogre scene manager of this viewport.
    Ogre::SceneManager* m_sceneManager {nullptr};

    /// Contains the Ogre render window containing this viewport.
    Ogre::RenderTarget* m_renderTarget {nullptr};

    /// Defines stereoscopic rendering mode.
    compositor::Core::StereoModeType m_stereoMode {compositor::Core::StereoModeType::NONE};

    /// Stores configured compositor chain raw string if there is a one.
    std::string m_rawCompositorChain;

    /// Contains the Ogre default compositor for this layer.
    SPTR(viz::scene3d::compositor::Core) m_coreCompositor {nullptr};

    /// Contains the Ogre default compositor default transparency technique.
    viz::scene3d::compositor::transparencyTechnique m_transparencyTechnique {viz::scene3d::compositor::DEFAULT};

    /// Defines the number of peels used by OIT.
    int m_numPeels {8};

    /// Manages the list of available compositors.
    /// The names are associated to a boolean value which indicates whether the compositor is enabled or not.
    viz::scene3d::compositor::ChainManager::uptr m_compositorChainManager;

    /// Defines the Z order of this viewport.
    int m_order {1};

    /// Defines the top background color : specific to background Layer.
    std::string m_topColor {"#333333"};

    /// Defines the bottom background color : specific to background Layer.
    std::string m_bottomColor {"#333333"};

    /// Defines the top background scale : specific to background Layer.
    float m_topScale {0.F};

    /// Defines the bottom background scale : specific to background Layer.
    float m_bottomScale {1.F};

    /// Defines the bottom background scale : specific to background Layer.
    std::string m_backgroundMaterial {};

    /// Contains the Ogre camera.
    Ogre::Camera* m_camera {nullptr};

    /// Stores the list of interactors, sorted by priority.
    std::multimap<int, viz::scene3d::interactor::IInteractor::wptr, std::greater<> > m_interactors;

    /// Handles flag cancelling all further interaction when enabled.
    bool m_cancelFurtherInteraction {false};

    /// Handles all connections.
    core::com::helper::SigSlotConnection m_connections;

    /// Contains the render service which this layer is attached.
    WPTR(viz::scene3d::SRender) m_renderService;

    /// Defines the layer identifier as referenced in SRender.
    std::string m_id;

    /// Enables default compositor's widgets (gui displays before scene creation).
    bool m_hasCoreCompositor {false};

    /// Indicates if a compositor chain is attached to the layer.
    bool m_hasCompositorChain {false};

    /// Indicates if the scene has been created.
    bool m_sceneCreated {false};

    /// Indicates if the scene has a default light.
    bool m_hasDefaultLight {true};

    /// Contains the abstract light used to set the default light.
    SPTR(viz::scene3d::ILight) m_lightAdaptor {nullptr};

    /// Contains the diffuse color of the default light.
    SPTR(data::Color) m_defaultLightDiffuseColor {nullptr};

    /// Contains the specular color of the specular light.
    SPTR(data::Color) m_defaultLightSpecularColor {nullptr};

    /// Defines the camera listener class used to pass the projection matrix for autostereo shaders.
    struct LayerCameraListener;
    LayerCameraListener* m_cameraListener {nullptr};

    /// Contains the autostereo listener.
    compositor::listener::AutoStereoCompositorListener* m_autostereoListener {nullptr};

    /// Holds pairs of intrinsic/extrinsic calibrations for stereo cameras.
    CameraCalibrationsType m_stereoCameraCalibration;

    /// Defines viewport parameters: left, top, width, height.
    ViewportConfigType m_viewportCfg {0.F, 0.F, 1.F, 1.F};

    /// True when we are using dedicated SCamera adaptor with orthographic projection.
    bool m_cameraOrthographic {false};
};

} // namespace sight::viz::scene3d.
