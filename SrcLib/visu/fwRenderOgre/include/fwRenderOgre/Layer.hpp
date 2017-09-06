/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_LAYER_HPP__
#define __FWRENDEROGRE_LAYER_HPP__

#include "fwRenderOgre/compositor/ChainManager.hpp"
#include "fwRenderOgre/compositor/Core.hpp"
#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/interactor/IInteractor.hpp"
#include "fwRenderOgre/interactor/IMovementInteractor.hpp"
#include "fwRenderOgre/interactor/IPickerInteractor.hpp"
#include "fwRenderOgre/IRenderWindowInteractorManager.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/HasSlots.hpp>
#include <fwCom/Slot.hpp>

#include <fwThread/Worker.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreViewport.h>

#include <cstdint>
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
 * @brief   Allows to render multiple scenes in the same render window with viewports
 */
class FWRENDEROGRE_CLASS_API Layer : public ::fwCore::BaseObject,
                                     public ::fwCom::HasSignals,
                                     public ::fwCom::HasSlots

{
public:

    enum class StereoModeType : std::uint8_t
    {
        NONE,
        AUTOSTEREO_5,
        AUTOSTEREO_8,
    };

    fwCoreClassDefinitionsWithFactoryMacro( (Layer)(::fwRenderOgre::Layer), (()), new Layer);
    fwCoreAllowSharedFromThis();

    /**@name Signals API
     * @{
     */
    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_INIT_LAYER_SIG;
    typedef ::fwCom::Signal<void (::fwRenderOgre::Layer::sptr)> InitLayerSignalType;

    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_RESIZE_LAYER_SIG;
    typedef ::fwCom::Signal<void (int, int)> ResizeLayerSignalType;

    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_COMPOSITOR_UPDATED_SIG;
    typedef ::fwCom::Signal<void (std::string, bool, ::fwRenderOgre::Layer::sptr)> CompositorUpdatedSignalType;

    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_STEREO_MODE_CHANGED_SIG;
    typedef ::fwCom::Signal<void (StereoModeType)> StereoModeChangedSignalType;

    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_CAMERA_UPDATED_SIG;
    typedef ::fwCom::Signal<void ()> CameraUpdatedSignalType;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    typedef ::fwCom::Slot< void (::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo) > InteractionSlotType;
    typedef ::fwCom::Slot< void () > DestroySlotType;

    /// Slot: Request the picker to do a ray cast according to the passed position
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_INTERACTION_SLOT;

    /// Slot: Request the reset of camera
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_RESET_CAMERA_SLOT;

    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_USE_CELSHADING_SLOT;
    /** @} */

    FWRENDEROGRE_API Layer();
    FWRENDEROGRE_API virtual ~Layer();

    /**
     * @brief setRenderWindow
     * Set the render window containing this layer
     */
    FWRENDEROGRE_API void setRenderWindow(::Ogre::RenderWindow* renderWindow);
    /**
     * @brief setID
     * Set the associated scene manager ID of this viewport
     */
    FWRENDEROGRE_API void setID(const std::string& id);
    FWRENDEROGRE_API const std::string getName() const;
    FWRENDEROGRE_API const std::string& getLayerID() const;

    /// Get the scene manager associated to this viewport.
    FWRENDEROGRE_API ::Ogre::SceneManager* getSceneManager() const;

    /// Create the scene.
    FWRENDEROGRE_API void createScene();
    /// Destroy the scene.
    FWRENDEROGRE_API void destroyScene();
    /// True if the scene is created
    FWRENDEROGRE_API bool isSceneCreated() const;

    /// Add a disabled compositor name to the ChainManager.
    FWRENDEROGRE_API void addAvailableCompositor(std::string compositorName);

    /// Enables/Disables a compositor according to the isEnabled flag.
    FWRENDEROGRE_API void updateCompositorState(std::string compositorName, bool isEnabled);

    /// Place and align camera's focal with the world boundingBox.
    FWRENDEROGRE_API void resetCameraCoordinates() const;

    /// Compute camera's focal with the world boundingBox.
    FWRENDEROGRE_API void computeCameraParameters() const;

    /// Reset the camera clipping range (near and far).
    FWRENDEROGRE_API void resetCameraClippingRange() const;

    /// Reset the camera clipping range (near and far).
    FWRENDEROGRE_API void resetCameraClippingRange(const ::Ogre::AxisAlignedBox& worldCoordBoundingBox) const;

    /**
     * @brief Returns Ogre interactor.
     */
    FWRENDEROGRE_API virtual ::fwRenderOgre::interactor::IInteractor::sptr getInteractor();

    /**
     * @brief set Ogre Movement interactor.
     */
    FWRENDEROGRE_API virtual void setMoveInteractor(::fwRenderOgre::interactor::IMovementInteractor::sptr interactor);

    /**
     * @brief set Ogre Select interactor.
     */
    FWRENDEROGRE_API virtual void setSelectInteractor(::fwRenderOgre::interactor::IPickerInteractor::sptr interactor);

    /**
     * @brief get Ogre Movement interactor.
     */
    FWRENDEROGRE_API virtual ::fwRenderOgre::interactor::IMovementInteractor::sptr getMoveInteractor();

    /**
     * @brief get Ogre Select interactor.
     */
    FWRENDEROGRE_API virtual ::fwRenderOgre::interactor::IPickerInteractor::sptr getSelectInteractor();

    /// Returns m_depth.
    FWRENDEROGRE_API int getDepth() const;
    /// Sets m_depth.
    FWRENDEROGRE_API void setDepth(int depth);

    /// Sets the worker for slots.
    FWRENDEROGRE_API void setWorker( const ::fwThread::Worker::sptr& _worker);

    /// Gets the render service.
    FWRENDEROGRE_API SPTR(::fwRenderOgre::SRender) getRenderService() const;

    /// Sets the render service.
    FWRENDEROGRE_API void setRenderService( const SPTR(::fwRenderOgre::SRender)& _service );

    FWRENDEROGRE_API void doRayCast(int x, int y, int width, int height);

    FWRENDEROGRE_API ::fwRenderOgre::interactor::IInteractor getInteractor(std::string type);

    /// Requests render.
    FWRENDEROGRE_API void requestRender();

    /// Sets stereoscopic rendering.
    FWRENDEROGRE_API void setStereoMode(StereoModeType mode);

    /// Sets background color : specific to background Layer.
    FWRENDEROGRE_API void setBackgroundColor(std::string topColor, std::string botColor);

    /// Sets background scale : specific to background Layer.
    FWRENDEROGRE_API void setBackgroundScale(float topScale, float botScale);

    /// Sets if this layer need a layer's 3D scene.
    FWRENDEROGRE_API void setCoreCompositorEnabled(bool enabled, std::string transparencyTechnique = "",
                                                   std::string numPeels = "");

    /// Sets if this layer has a configured compositor chain.
    FWRENDEROGRE_API void setCompositorChainEnabled(const std::string& compositorChain);

    /// Gets if this layer needs a layer's 3D scene.
    FWRENDEROGRE_API bool isCoreCompositorEnabled();

    /// Gets if there is an XML configured compositor chain.
    FWRENDEROGRE_API bool isCompositorChainEnabled();

    /// Checks if stereoscopic rendering is enabled.
    FWRENDEROGRE_API bool is3D() const;

    /// Gets stereoscopic mode
    FWRENDEROGRE_API StereoModeType getStereoMode() const;

    /// Checks if this layer has a default compositor.
    FWRENDEROGRE_API ::fwRenderOgre::compositor::Core::sptr getCoreCompositor();

    FWRENDEROGRE_API ::fwRenderOgre::compositor::ChainManager::CompositorChainType getCompositorChain() const;

    /// Returns the list of adaptors in the chain manager.
    FWRENDEROGRE_API ::fwServices::IHasServices::ServiceVector getRegisteredAdaptors() const;

    FWRENDEROGRE_API ::Ogre::Viewport* getViewport() const;

    FWRENDEROGRE_API ::Ogre::Camera* getDefaultCamera() const;

    /// Default light flag setter.
    FWRENDEROGRE_API void setHasDefaultLight(bool hasDefaultLight);

    /// Returns the number of lights adaptors used in this layer.
    FWRENDEROGRE_API int getLightsNumber() const;

    /// Returns the light adaptors used in this layer.
    FWRENDEROGRE_API std::vector<SPTR(::fwRenderOgre::ILight)> getLightAdaptors() const;

    FWRENDEROGRE_API static const std::string DEFAULT_CAMERA_NAME;

    FWRENDEROGRE_API static const std::string DEFAULT_LIGHT_NAME;

private:
    /// Slot: Interact with the scene.
    void interaction(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo);

    /// Compute bounding box of the scene.
    ::Ogre::AxisAlignedBox computeWorldBoundingBox() const;

    /// Setups default compositor for a layer's 3D scene.
    void setupCore();

    /// For a list of semicolon-separated words, returns a vector of these words.
    std::vector< std::string > trimSemicolons(std::string input);

    /// Ogre scene manager of this viewport.
    ::Ogre::SceneManager* m_sceneManager;

    /// Ogre render window containing this viewport.
    ::Ogre::RenderWindow* m_renderWindow;

    /// Ogre viewport representing this layer.
    ::Ogre::Viewport* m_viewport;

    /// Boolean used to set stereoscopic rendering.
    StereoModeType m_stereoMode;

    /// If there is a configured compositor chain, this attribute stores its raw string.
    std::string m_rawCompositorChain;

    /// Ogre default compositor for this layer.
    ::fwRenderOgre::compositor::Core::sptr m_coreCompositor;

    /// Ogre default compositor default transparency technique.
    ::fwRenderOgre::compositor::transparencyTechnique m_transparencyTechnique;

    int m_numPeels;

    /// Manages the list of available compositors.
    /// The names are associated to a boolean value which indicates whether the compositor is enabled or not.
    ::fwRenderOgre::compositor::ChainManager::uptr m_compositorChainManager;

    /// Z Depth of this viewport.
    int m_depth;

    /// Top background color : specific to background Layer.
    std::string m_topColor;
    /// Bottom background color : specific to background Layer.
    std::string m_bottomColor;

    /// Top background scale : specific to background Layer.
    float m_topScale;
    /// Bottom background scale : specific to background Layer.
    float m_bottomScale;

    /// Camera
    ::Ogre::Camera* m_camera;

    /// Ogre movement interactor
    ::fwRenderOgre::interactor::IMovementInteractor::sptr m_moveInteractor;

    /// Ogre picker interactor
    ::fwRenderOgre::interactor::IPickerInteractor::sptr m_selectInteractor;

    ///Connection service, needed for slot/signal association.
    ::fwCom::helper::SigSlotConnection m_connections;

    /// Render service which this layer is attached.
    WPTR(::fwRenderOgre::SRender) m_renderService;

    /// Layer identifier as referenced in SRender.
    std::string m_id;

    /// This boolean enables default compositor's widgets (gui displays before scene creation).
    bool m_hasCoreCompositor;

    /// Indicates if a compositor chain is attached to the layer.
    bool m_hasCompositorChain;

    /// Indicates if the scene has been created.
    bool m_sceneCreated;

    /// Indicates if the scene has a default light.
    bool m_hasDefaultLight;

    /// Abstract light used to set the default light.
    SPTR(::fwRenderOgre::ILight) m_lightAdaptor;

    SPTR(::fwData::TransformationMatrix3D) m_defaultLightTransform;

    /// Diffuse color of the default light.
    SPTR(::fwData::Color) m_defaultLightDiffuseColor;

    /// Specular color of the specular light.
    SPTR(::fwData::Color) m_defaultLightSpecularColor;

    /// Camera listener class used to pass the projection matrix for autostereo shaders.
    struct LayerCameraListener;
    LayerCameraListener* m_cameraListener;
};

}

#endif // __FWRENDEROGRE_LAYER_HPP__
