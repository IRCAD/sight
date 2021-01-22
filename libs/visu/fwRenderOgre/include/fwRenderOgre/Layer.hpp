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

#include "fwRenderOgre/compositor/ChainManager.hpp"
#include "fwRenderOgre/compositor/Core.hpp"
#include "fwRenderOgre/compositor/listener/AutoStereo.hpp"
#include "fwRenderOgre/compositor/types.hpp"
#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/interactor/IInteractor.hpp"
#include "fwRenderOgre/IRenderWindowInteractorManager.hpp"

#include <core/com/HasSignals.hpp>
#include <core/com/HasSlots.hpp>
#include <core/com/Slot.hpp>
#include <core/thread/Worker.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreVector2.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/Overlay/OgreOverlay.h>

#include <cstdint>
#include <map>
#include <vector>

namespace fwData
{
class Color;
class TransformationMatrix3D;
}

namespace fwRenderOgre
{
class SRender;
class IAdaptor;
class ILight;
}

namespace Ogre
{
class Camera;
}

namespace fwRenderOgre
{

/**
 * @brief Allows to render multiple scenes in the same render window with viewports.
 */
class FWRENDEROGRE_CLASS_API Layer :
    public core::BaseObject,
    public core::com::HasSignals,
    public core::com::HasSlots

{

public:

    /// Generates default methods.
    fwCoreClassMacro(Layer, ::fwRenderOgre::Layer, new Layer)

    fwCoreAllowSharedFromThis()

    /// Defines the extrinsic x Intrinsic camera calibrations.
    using CameraCalibrationsType = std::vector< ::Ogre::Matrix4 >;

    /// Defines the set of overlays enabled on a layer.
    using OverlaySetType = std::vector< ::Ogre::Overlay* >;

    /// Definest the viewport parameters relatively to the screen: left, top, width ,height.
    using ViewportConfigType = std::tuple<float, float, float, float>;

    /// Contains the signal sent when the layer is initialized.
    FWRENDEROGRE_API static const core::com::Signals::SignalKeyType s_INIT_LAYER_SIG;
    typedef core::com::Signal<void (::fwRenderOgre::Layer::sptr)> InitLayerSignalType;

    /// Contains the signal sent when the layer is resized.
    FWRENDEROGRE_API static const core::com::Signals::SignalKeyType s_RESIZE_LAYER_SIG;
    typedef core::com::Signal<void (int, int)> ResizeLayerSignalType;

    /// @deprecated A listener is now in place in SCamera instead.
    [[deprecated("Removed in sight 22.0.")]]
    FWRENDEROGRE_API static const core::com::Signals::SignalKeyType s_CAMERA_UPDATED_SIG;

    /// Contains signals sent when the camera is modified.
    FWRENDEROGRE_API static const core::com::Signals::SignalKeyType s_CAMERA_RANGE_UPDATED_SIG;
    typedef core::com::Signal<void ()> CameraUpdatedSignalType;

    typedef core::com::Slot< void (::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo) >
        InteractionSlotType;
    typedef core::com::Slot< void () > DestroySlotType;

    /// Contains the slot name that request the picker to do a ray cast according to the passed position.
    FWRENDEROGRE_API static const core::com::Slots::SlotKeyType s_INTERACTION_SLOT;

    /// Contains the slot name taht request the reset of camera.
    FWRENDEROGRE_API static const core::com::Slots::SlotKeyType s_RESET_CAMERA_SLOT;

    /// @deprecated please used s_DEFAULT_CAMERA_NAME.
    [[deprecated("Removed in sight 22.0.")]]
    FWRENDEROGRE_API static const std::string DEFAULT_CAMERA_NAME;

    /// @deprecated please used s_DEFAULT_LIGHT_NAME
    [[deprecated("Removed in sight 22.0.")]]
    FWRENDEROGRE_API static const std::string DEFAULT_LIGHT_NAME;

    /// Defines the default camera name.
    FWRENDEROGRE_API static const std::string s_DEFAULT_CAMERA_NAME;

    /// Defines the default light name.
    FWRENDEROGRE_API static const std::string s_DEFAULT_LIGHT_NAME;

    /// Defines the default camera node name.
    FWRENDEROGRE_API static const std::string s_DEFAULT_CAMERA_NODE_NAME;

    /// Initializes signals and slots.
    FWRENDEROGRE_API Layer();

    /// Destoyres Ogre resources.
    FWRENDEROGRE_API virtual ~Layer();

    /// Sets the render window containing this layer.
    FWRENDEROGRE_API void setRenderTarget(::Ogre::RenderTarget* _renderTarget);

    /// Set the associated scene manager ID of this viewport
    FWRENDEROGRE_API void setID(const std::string& id);

    /// @returns the name of this layer.
    FWRENDEROGRE_API const std::string getName() const;

    /// @returns the ID of this layer.
    FWRENDEROGRE_API const std::string& getLayerID() const;

    /// @returns the scene manager associated to this viewport.
    FWRENDEROGRE_API ::Ogre::SceneManager* getSceneManager() const;

    /// Creates the scene.
    FWRENDEROGRE_API void createScene();

    /// Destroys the scene.
    FWRENDEROGRE_API void destroyScene();

    /// @returns true if the scene is created.
    FWRENDEROGRE_API bool isSceneCreated() const;

    /// Adds a disabled compositor name to the ChainManager.
    FWRENDEROGRE_API void addAvailableCompositor(std::string compositorName);

    /// Enables/Disables a compositor according to the isEnabled flag.
    FWRENDEROGRE_API void updateCompositorState(std::string compositorName, bool isEnabled);

    /// Places and align camera's focal with the world boundingBox.
    FWRENDEROGRE_API void resetCameraCoordinates();

    /// Computes camera's focal with the world boundingBox.
    FWRENDEROGRE_API void computeCameraParameters();

    /// Resets the camera clipping range (near and far).
    FWRENDEROGRE_API void resetCameraClippingRange() const;

    /// Resets the camera clipping range (near and far).
    FWRENDEROGRE_API void resetCameraClippingRange(const ::Ogre::AxisAlignedBox& worldCoordBoundingBox) const;

    /// Appends a new interactor with the given priority. Interactors with higher priorities are executed first.
    FWRENDEROGRE_API void addInteractor(const ::fwRenderOgre::interactor::IInteractor::sptr& _interactor,
                                        int _priority = 0);

    /// Removes the given interactor.
    FWRENDEROGRE_API void removeInteractor(const ::fwRenderOgre::interactor::IInteractor::sptr& _interactor);

    /// @return the order of the layer.
    FWRENDEROGRE_API int getOrder() const;

    /// Sets the order of the layer.
    FWRENDEROGRE_API void setOrder(int _order);

    /// Sets the worker used by slots.
    FWRENDEROGRE_API void setWorker( const core::thread::Worker::sptr& _worker);

    /// @returns the render service.
    FWRENDEROGRE_API SPTR(::fwRenderOgre::SRender) getRenderService() const;

    /// Sets the render service.
    FWRENDEROGRE_API void setRenderService( const SPTR(::fwRenderOgre::SRender)& _service );

    /// Requests render.
    FWRENDEROGRE_API void requestRender();

    /// Sets stereoscopic rendering.
    FWRENDEROGRE_API void setStereoMode(compositorcore::StereoModeType mode);

    /// Sets background color : specific to background Layer.
    FWRENDEROGRE_API void setBackgroundColor(std::string topColor, std::string botColor);

    /// Sets background scale : specific to background Layer.
    FWRENDEROGRE_API void setBackgroundScale(float topScale, float botScale);

    /// Sets if this layer need a layer's 3D scene.
    FWRENDEROGRE_API void setCoreCompositorEnabled(bool enabled, std::string transparencyTechnique = "",
                                                   std::string numPeels                      = "",
                                                   compositorcore::StereoModeType stereoMode =
                                                       compositorcore::StereoModeType::NONE);

    /// Sets if this layer has a configured compositor chain.
    FWRENDEROGRE_API void setCompositorChainEnabled(const std::string& compositorChain);

    /// Sets the viewport parameters for this layer: left, top, width, height.
    FWRENDEROGRE_API void setViewportConfig(const ViewportConfigType& _vpCfg);

    /// @returns true if this layer needs a layer's 3D scene.
    FWRENDEROGRE_API bool isCoreCompositorEnabled();

    /// @returns true if there is an XML configured compositor chain.
    FWRENDEROGRE_API bool isCompositorChainEnabled();

    /// @returns true if stereoscopic rendering is enabled.
    FWRENDEROGRE_API bool is3D() const;

    /// @returns the stereoscopic mode.
    FWRENDEROGRE_API compositorcore::StereoModeType getStereoMode() const;

    /// @returns the compositor chain.
    FWRENDEROGRE_API ::fwRenderOgre::compositor::ChainManager::CompositorChainType getCompositorChain() const;

    /// @returns the list of adaptors in the chain manager.
    FWRENDEROGRE_API ::fwServices::IHasServices::ServiceVector getRegisteredAdaptors() const;

    /// @returns the viewport.
    FWRENDEROGRE_API ::Ogre::Viewport* getViewport() const;

    /// @returns the default camera.
    FWRENDEROGRE_API ::Ogre::Camera* getDefaultCamera() const;

    /// Gets the projection matrix used to define nth viewpoint. The index must be lower than the number of viewpoints.
    FWRENDEROGRE_API ::Ogre::Matrix4 getCameraProjMat(const std::uint8_t cameraIdx) const;

    /// @returns the number of cameras (viewpoints) used by this layer. Defined by the stereo mode.
    FWRENDEROGRE_API std::uint8_t getNumberOfCameras() const;

    /// Sets default light flag.
    FWRENDEROGRE_API void setHasDefaultLight(bool hasDefaultLight);

    /// @returns the number of lights adaptors used in this layer.
    FWRENDEROGRE_API int getLightsNumber() const;

    /// @returns the light adaptors used in this layer.
    FWRENDEROGRE_API std::vector<SPTR(::fwRenderOgre::ILight)> getLightAdaptors() const;

    /// @returns the computed bounding box of the scene.
    FWRENDEROGRE_API ::Ogre::AxisAlignedBox computeWorldBoundingBox() const;

    /// @returns the OIT selected.
    FWRENDEROGRE_API compositor::transparencyTechnique getTransparencyTechnique();

    /// @returns the number of peels computed by Depth Peeling or x2 Dual Depth Peeling.
    FWRENDEROGRE_API int getTransparencyDepth();

    /// Sets the OIT desired. Deactivate OIT compositor.
    FWRENDEROGRE_API bool setTransparencyTechnique(compositor::transparencyTechnique technique);

    /// Sets the number of peels computed by Depth Peeling or x2 Dual Depth Peeling. Deactivate OIT compositor.
    FWRENDEROGRE_API void setTransparencyDepth(int depth);

    /// Sets the camera calibrations for stereo rendering.
    FWRENDEROGRE_API void setCameraCalibrations(const CameraCalibrationsType& calibrations);

    /// @returns true if a specified light is the default light in the layer.
    FWRENDEROGRE_API bool isDefaultLight(const CSPTR(::fwRenderOgre::ILight)&) const;

    /// Removes the default light in the layer.
    FWRENDEROGRE_API void removeDefaultLight();

    /// @returns or create the overlay panel in which adaptors can render 2D text.
    FWRENDEROGRE_API ::Ogre::OverlayContainer* getOverlayTextPanel();

    /// Defines the overlay scripts to enable on this layer's viewport.
    FWRENDEROGRE_API void setEnabledOverlays(const std::vector<std::string>& _overlayScripts);

    /// @returns the overlays enabled on this layer's viewport.
    FWRENDEROGRE_API const OverlaySetType& getEnabledOverlays() const;

    /// Cancels interaction for all interactors with a lower priority than the one calling this.
    FWRENDEROGRE_API void cancelFurtherInteraction();

private:

    /// Slot: interacts with the scene.
    void interaction(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo);

    /// Setups default compositor for a layer's 3D scene.
    void setupCore();

    /// Stops and starts all adaptors belonging to this layer. Subadaptors are expected to be managed by their parent.
    void restartAdaptors();

    /// Calls a function on all interactors and deletes the ones that expired.
    void forAllInteractors(const std::function< void(const interactor::IInteractor::sptr&)>&& _f);

    /// Contains the Ogre scene manager of this viewport.
    ::Ogre::SceneManager* m_sceneManager { nullptr };

    /// Contains the Ogre render window containing this viewport.
    ::Ogre::RenderTarget* m_renderTarget { nullptr };

    /// Contains the Ogre viewport representing this layer.
    ::Ogre::Viewport* m_viewport { nullptr };

    /// Contains the overlay panel to which all the UI's text is attached.
    ::Ogre::OverlayContainer* m_overlayTextPanel { nullptr };

    /// Defines stereoscopic rendering mode.
    compositorcore::StereoModeType m_stereoMode { compositorcore::StereoModeType::NONE };

    /// Stores configured compositor chain raw string if there is a one.
    std::string m_rawCompositorChain { "" };

    /// Contains the Ogre default compositor for this layer.
    SPTR(::fwRenderOgre::compositor::Core) m_coreCompositor { nullptr };

    /// Contains the Ogre default compositor default transparency technique.
    ::fwRenderOgre::compositor::transparencyTechnique m_transparencyTechnique { ::fwRenderOgre::compositor::DEFAULT };

    /// Defines the number of peels used by OIT.
    int m_numPeels { 8 };

    /// Manages the list of available compositors.
    /// The names are associated to a boolean value which indicates whether the compositor is enabled or not.
    ::fwRenderOgre::compositor::ChainManager::uptr m_compositorChainManager;

    /// Defines the Z order of this viewport.
    int m_order { 1 };

    /// Defines the top background color : specific to background Layer.
    std::string m_topColor { "#333333" };

    /// Defines the bottom background color : specific to background Layer.
    std::string m_bottomColor { "#333333" };

    /// Defines the top background scale : specific to background Layer.
    float m_topScale { 0.f };

    /// Defines the bottom background scale : specific to background Layer.
    float m_bottomScale { 1.f };

    /// Contains the Ogre camera.
    ::Ogre::Camera* m_camera { nullptr };

    /// Stores the list of interactors, sorted by priority.
    std::multimap< int, ::fwRenderOgre::interactor::IInteractor::wptr, std::greater<int> > m_interactors;

    /// Handles flag cancelling all further interaction when enabled.
    bool m_cancelFurtherInteraction { false };

    /// Handles all connections.
    core::com::helper::SigSlotConnection m_connections;

    /// Contains the render service which this layer is attached.
    WPTR(::fwRenderOgre::SRender) m_renderService;

    /// Defines the layer identifier as referenced in SRender.
    std::string m_id { "" };

    /// Enables default compositor's widgets (gui displays before scene creation).
    bool m_hasCoreCompositor { false };

    /// Indicates if a compositor chain is attached to the layer.
    bool m_hasCompositorChain { false };

    /// Indicates if the scene has been created.
    bool m_sceneCreated { false };

    /// Indicates if the scene has a default light.
    bool m_hasDefaultLight { true };

    /// Contains the abstract light used to set the default light.
    SPTR(::fwRenderOgre::ILight) m_lightAdaptor { nullptr };

    /// Contains the diffuse color of the default light.
    SPTR(::fwData::Color) m_defaultLightDiffuseColor { nullptr };

    /// Contains the specular color of the specular light.
    SPTR(::fwData::Color) m_defaultLightSpecularColor { nullptr };

    /// Defines the camera listener class used to pass the projection matrix for autostereo shaders.
    struct LayerCameraListener;
    LayerCameraListener* m_cameraListener { nullptr };

    /// Contains the autostereo listener.
    compositor::listener::AutoStereoCompositorListener* m_autostereoListener { nullptr };

    /// Holds pairs of intrinsic/extrinsic calibrations for stereo cameras.
    CameraCalibrationsType m_stereoCameraCalibration;

    /// Stores names of the enabled overlay scripts.
    std::vector<std::string> m_overlayScripts;

    /// Overlays enabled on this layer's viewport.
    OverlaySetType m_enabledOverlays;

    /// Defines viewport parameters: left, top, width, height.
    ViewportConfigType m_viewportCfg { 0.f, 0.f, 1.f, 1.f};

    /// Defines the dpi number of the rendering screen.
    float m_dpi { 96.f };

};

} // namespace fwRenderOgre.
