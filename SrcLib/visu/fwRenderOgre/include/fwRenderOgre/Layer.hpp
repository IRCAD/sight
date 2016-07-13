/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_LAYER_HPP__
#define __FWRENDEROGRE_LAYER_HPP__

#include <fwCom/HasSignals.hpp>
#include <fwCom/HasSlots.hpp>
#include <fwCom/Slot.hpp>

#include <fwRenderOgre/IRenderWindowInteractorManager.hpp>
#include <fwRenderOgre/interactor/IMovementInteractor.hpp>
#include <fwRenderOgre/interactor/IPickerInteractor.hpp>
#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/compositor/ChainManager.hpp>
#include <fwRenderOgre/compositor/Core.hpp>

#include <fwThread/Worker.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreViewport.h>

#include <vector>

#include "fwRenderOgre/config.hpp"

namespace fwRenderOgre
{
class SRender;
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

    /** @} */

    /**
     * @name Slots API
     * @{
     */
    typedef ::fwCom::Slot< void (::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo) > InteractionSlotType;
    typedef ::fwCom::Slot< void () > DestroySlotType;

    /// Slot: Request the picker to do a ray cast according to the passed position
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_INTERACTION_SLOT;

    /// Slot: Request the deletion of the scene manager
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_DESTROY_SLOT;

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
    FWRENDEROGRE_API const std::string& getID() const;

    /// Get the scene manager associated to this viewport
    FWRENDEROGRE_API ::Ogre::SceneManager* getSceneManager() const;

    /// Create the scene
    FWRENDEROGRE_API void createScene();

    /// Add a disabled compositor name to the ChainManager
    FWRENDEROGRE_API void addAvailableCompositor(std::string compositorName);

    /// Removes all available compositors from the ChainManager
    FWRENDEROGRE_API void clearAvailableCompositors();

    /// Enables/Disables a compositor according to the isEnabled flag
    FWRENDEROGRE_API void updateCompositorState(std::string compositorName, bool isEnabled);

    /// Place and align camera's focal with the boundingBox
    FWRENDEROGRE_API void resetCameraCoordinates() const;

    /// Reset the camera clipping range (near and far)
    FWRENDEROGRE_API void resetCameraClippingRange() const;

    /// Reset the camera clipping range (near and far)
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

    /// Returns m_depth
    FWRENDEROGRE_API int getDepth() const;
    /// Sets m_depth
    FWRENDEROGRE_API void setDepth(int depth);

    /// Sets the worker for slots
    FWRENDEROGRE_API void setWorker( const ::fwThread::Worker::sptr& _worker);

    /// Sets the render service
    FWRENDEROGRE_API void setRenderService( const SPTR(::fwRenderOgre::SRender)& _service );

    FWRENDEROGRE_API void doRayCast(int x, int y, int width, int height);

    FWRENDEROGRE_API ::fwRenderOgre::interactor::IInteractor getInteractor(std::string type);

    /// Requests render
    FWRENDEROGRE_API void requestRender();

    /// Sets stereoscopic rendering.
    FWRENDEROGRE_API void set3D(bool mode3D);

    /// Sets background color : specific to background Layer
    FWRENDEROGRE_API void setBackgroundColor(std::string topColor, std::string botColor);

    /// Sets background scale : specific to background Layer
    FWRENDEROGRE_API void setBackgroundScale(float topScale, float botScale);

    /// Sets if this layer need a layer's 3D scene
    FWRENDEROGRE_API void setCoreCompositorEnabled(bool enabled, std::string transparencyTechnique = "",
                                                   std::string numPeels = "");

    /// Sets if this layer has a configured compositor chain
    FWRENDEROGRE_API void setCompositorChainEnabled(bool hasCoreChain, std::string compositorChain);

    /// Gets if this layer needs a layer's 3D scene
    FWRENDEROGRE_API bool isCoreCompositorEnabled();

    /// Gets if there is an XML configured compositor chain
    FWRENDEROGRE_API bool isCompositorChainEnabled();

    /// Checks if stereoscopic rendering is enabled.
    FWRENDEROGRE_API bool is3D() const;

    /// Checks if this layer has a default compositor
    FWRENDEROGRE_API ::fwRenderOgre::compositor::Core::sptr getCoreCompositor();

    FWRENDEROGRE_API ::fwRenderOgre::compositor::ChainManager::CompositorChainType getCompositorChain();

    FWRENDEROGRE_API std::string getFinalChainCompositorName() const;

    FWRENDEROGRE_API ::Ogre::Viewport* getViewport() const;

    FWRENDEROGRE_API bool isSceneCreated() const;

private:
    /// Slot: Interact with the scene
    void interaction(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo);

    /// Slot: Destroy the scene
    void destroy();

    /// Compute bounding box of the scene
    ::Ogre::AxisAlignedBox computeCameraParameters() const;

    /// Setups default compositor for a layer's 3D scene
    void setupCore();

    /// For a list of semicolon-separated words, returns a vector of these words
    std::vector< std::string > trimSemicolons(std::string input);

    /// Ogre scene manager of this viewport
    ::Ogre::SceneManager* m_sceneManager;

    /// Ogre render window containing this viewport
    ::Ogre::RenderWindow* m_renderWindow;

    /// Ogre viewport representing this layer
    ::Ogre::Viewport* m_viewport;

    /// Boolean used to set stereoscopic rendering.
    bool m_is3D;

//    /// Number of viewports.
//    unsigned m_nbViewports;

//    /// Intermediate render targets for multi-view rendering.
//    std::vector< ::Ogre::TexturePtr > m_renderTargets;

//    /// Ogre viewports for multi-screen monitors.
//    std::vector< ::Ogre::Viewport * > m_multiViewports;

    /// This boolean enables default compositor's widgets (gui displays before scene creation)
    bool m_hasCoreCompositor;

    /// Indicates if a compositor chain is attached to the layer
    bool m_hasCompositorChain;

    /// Indicates if the scene has been created
    bool m_sceneCreated;

    /// If there is a configured compositor chain, this attribute stores its raw string
    std::string m_rawCompositorChain;

    /// Ogre default compositor for this layer
    ::fwRenderOgre::compositor::Core::sptr m_coreCompositor;

    /// Ogre default compositor default transparency technique
    transparencyTechnique m_transparencyTechnique;

    int m_numPeels;

    /// Manages the list of available compositors.
    /// The names are associated to a boolean value which indicates whether the compositor is enabled or not
    ::fwRenderOgre::compositor::ChainManager m_compositorChainManager;

    /// Z Depth of this viewport
    int m_depth;

    /// Top background color : specific to background Layer
    std::string m_topColor;
    /// Bottom background color : specific to background Layer
    std::string m_bottomColor;

    /// Top background scale : specific to background Layer
    float m_topScale;
    /// Bottom background scale : specific to background Layer
    float m_bottomScale;

    /// Camera
    ::Ogre::Camera* m_camera;

    /// Ogre movement interactor
    ::fwRenderOgre::interactor::IMovementInteractor::sptr m_moveInteractor;

    /// Ogre picker interactor
    ::fwRenderOgre::interactor::IPickerInteractor::sptr m_selectInteractor;

    ///Connection service, needed for slot/signal association
    ::fwServices::helper::SigSlotConnection m_connections;

    /// Render service which this layer is attached
    WPTR(::fwRenderOgre::SRender) m_renderService;
};

}

#endif // __FWRENDEROGRE_LAYER_HPP__
