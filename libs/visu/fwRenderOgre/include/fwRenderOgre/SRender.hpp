/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/IRenderWindowInteractorManager.hpp"
#include "fwRenderOgre/Layer.hpp"
#include "fwRenderOgre/overlay/ViewportListener.hpp"
#include "fwRenderOgre/picker/IPicker.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Composite.hpp>

#include <fwRender/IRender.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/helper/Config.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/Overlay/OgreOverlay.h>

#include <map>
#include <tuple>

namespace fwRenderOgre
{

class IAdaptor;
class Layer;

/**
 * @brief The generic scene service shows adaptors in a 3D Ogre scene.
 *
 * @section Signals Signals
 *  - \b compositorUpdated(): sent when the compositor chain is modified.
 *  - \b fullscreenSet(bool): sent when fullscreen is enabled/disabled.
 *
 * @section Slots Slots
 *  - \b computeCameraParameters(): resets the camera position to visualize the whole scene.
 *  - \b computeCameraClipping(): recomputes the camera's clipping range when the scene is updated.
 *  - \b requestRender(): request the service to repaint the scene.
 *  - \b disableFullscreen(): switches to windowed rendering if fullscreen is enabled.
 *  - \b enableFullscreen(int): switches fullscreen rendering on the given screen.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::fwRenderOgre::SRender" autoconnect="yes">
        <scene renderMode="auto">
            <layer id="..." depth="1">
                <viewport hOffset="0.03" vOffset="0.03" width="0.3" height="0.3" hAlign="right" vAlign="top"/>
            </layer>
            <layer id="..." depth="2" compositors="Invert;Laplace;Posterize" defaultLight="no" overlays="..."/>

            <adaptor uid="..."/>
            <adaptor uid="..."/>
        </scene>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b offScreen [::fwData::Image] (optional, unused by default): If used, render the scene in an image
 * and not in a window.
 *
 * @subsection Configuration Configuration
 *  - \b scene
 *    - \b renderMode (optional): 'auto' (only when something has changed), 'always' (render continuously) or 'sync'
 *        (only when the slot "requestRender" is called). Default is 'auto'.
 *    - \b width (optional, "1280" by default): width for off-screen rendering
 *    - \b height (optional, "720" by default): height for off-screen rendering
 *  - \b layer : mandatory, defines the scene's layer
 *    - \b viewport (optional):
 *      - \b hAlign (optional, values=left|center|right, default=left): defines the horizontal origin of the viewport.
 *      - \b vAlign (optional, values=top|center|bottom, default=top): defines the vertical origin of the viewport.
 *      - \b hOffset (optional, float, default=0.f): horizontal offset from the origin relatively to the window.
 *      - \b vOffset (optional, float, default=0.f): vertical offset from the origin relatively to the window.
 *      - \b width (optional, float, default=1.f): viewport width relatively to the window.
 *      - \b height (optional, float, default=1.f): viewport height relatively to the window.
 *    - \b id (mandatory): the identifier of the layer
 *    - \b depth (mandatory): the depth of the layer, starting from 1
 *    - \b transparency (optional): the transparency technique to use: DepthPeeling, DualDepthPeeling,
 *                                  WeightedBlended, HybridTransparency or CelShadingDepthPeeling.
 *    - \b numPeels (optional): number of peels for the selected transparency technique.
 *                              Not used for WeightedBlended OIT
 *    - \b compositors (optional): defines the default compositor chain. The compositors are separated by semicolons
 *    - \b fullscreen (optional, yes/no, default="no"): Show the scene in full screen.
 *    - \b stereoMode (optional, yes/no, default="no"): Sets the mode used for stereoscopic 3D rendering,
 *                                          available modes are "AutoStereo5", "AutoStereo8" and "no".
 *    - \b defaultLight (optional, yes/no, default="yes"): Sets if a default light is created in the layer.
 *    - \b overlays (optional): list of overlay names (separated by semicolons) rendered on top of the layer's viewport.
 *  - \b adaptor
 *    - \b uid (mandatory): the identifier of the adaptor
 */
class FWRENDEROGRE_CLASS_API SRender : public ::fwRender::IRender

{
public:
    fwCoreServiceMacro(SRender, ::fwRender::IRender)

    FWRENDEROGRE_API SRender() noexcept;
    FWRENDEROGRE_API virtual ~SRender() noexcept;

    enum class RenderMode
    {
        ALWAYS,
        AUTO,
        SYNC
    };

    typedef std::string AdaptorIdType;
    typedef std::string OgreObjectIdType;
    typedef std::string SceneIdType;

    /// Actives layouts in the scene.
    typedef std::map< SceneIdType, SPTR(::fwRenderOgre::Layer) > LayerMapType;

    FWRENDEROGRE_API static const std::string s_OGREBACKGROUNDID;

    /**
     * @name Signals API
     * @{
     */
    /// Signal: sent when the compositor chain has been modified.
    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_COMPOSITOR_UPDATED_SIG;
    typedef ::fwCom::Signal<void (std::string, bool, ::fwRenderOgre::Layer::sptr)> CompositorUpdatedSignalType;

    /// Signal: sent when fullscreen was enabled/disabled.
    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_FULLSCREEN_SET_SIG;
    using FullscreenSetSignalType = ::fwCom::Signal< void(bool) >;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    typedef ::fwCom::Slot< void () > ComputeCameraOrigSlotType;
    /// Slot: Computes the parameters to reset the camera.
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_COMPUTE_CAMERA_ORIG_SLOT;

    typedef ::fwCom::Slot< void () > ComputeCameraClippingSlotType;
    /// Slot: Computes the parameters to reset the camera.
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_COMPUTE_CAMERA_CLIPPING_SLOT;

    /// Slot: Request the picker to do a ray cast according to the passed position.
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_DO_RAY_CAST_SLOT;

    typedef ::fwCom::Slot< void () > RequestRenderSlotType;
    /// Slot: Request a rendering.
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_REQUEST_RENDER_SLOT;

    /// Slot: Disables fullscreen rendering if it was enabled.
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_DISABLE_FULLSCREEN;

    /// Slot: Enables fullscreen rendering on a specific screen.
    FWRENDEROGRE_API static const ::fwCom::Slots::SlotKeyType s_ENABLE_FULLSCREEN;

    /** @} */

    /// Sets this render service as the current OpenGL context.
    FWRENDEROGRE_API void makeCurrent();

    /// Requests a render from the Ogre render engine.
    FWRENDEROGRE_API void requestRender();

    /// Returns true if the scene is shown on screen.
    FWRENDEROGRE_API bool isShownOnScreen();

    /// Returns the scene manager corresponding to the sceneID.
    FWRENDEROGRE_API ::Ogre::SceneManager* getSceneManager(const ::std::string& sceneID);

    /// Returns the layer corresponding to the sceneID.
    FWRENDEROGRE_API ::fwRenderOgre::Layer::sptr getLayer(const ::std::string& sceneID);

    /// Returns this render layers.
    FWRENDEROGRE_API LayerMapType getLayers();

    /// Returns m_interactorManager.
    FWRENDEROGRE_API ::fwRenderOgre::IRenderWindowInteractorManager::sptr getInteractorManager() const;

    /// Resets camera parameters with the actual global bounding box.
    FWRENDEROGRE_API void resetCameraCoordinates(const std::string& _layerId);

    /// Computes camera parameters with the actual global bounding box.
    FWRENDEROGRE_API void computeCameraClipping();

    template<class T>
    std::vector<SPTR(T)> getAdaptors() const;

    /// Returns the rendering mode
    RenderMode getRenderMode() const;

protected:

    /// Renders the scene.
    FWRENDEROGRE_API void render();

    /// Configures the adaptors and the connections
    FWRENDEROGRE_API virtual void starting() override;
    /// Stops all the adaptors
    FWRENDEROGRE_API virtual void stopping() override;

    ///Configures the adaptor
    FWRENDEROGRE_API virtual void configuring() override;

    /// Does nothing.
    FWRENDEROGRE_API void updating() override;

private:

    /// Configuration element shared pointer
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Configure background layer of the scene
    void configureBackgroundLayer( const ConfigType& _cfg );
    /// Configure each layer of the scene
    void configureLayer(const ConfigType& _cfg );

    /// Retrieves the viewport parameters from the configuration.
    static Layer::ViewportConfigType configureLayerViewport(const ::fwServices::IService::ConfigType& _cfg);

    /// Render the scene in fullscreen on the screen with the given index.
    void enableFullscreen(int screen);

    /// Switch back to windowed rendering if fullscreen is on.
    void disableFullscreen();

    /// Contains all the layers of the scene
    LayerMapType m_layers;

    /// Signal/ Slot connection
    ::fwCom::helper::SigSlotConnection m_connections;

    /// Signal sent when fullscreen is enabled/disabled.
    FullscreenSetSignalType::sptr m_fullscreenSetSig;

    /// Ogre window interactor manager
    ::fwRenderOgre::IRenderWindowInteractorManager::sptr m_interactorManager;

    /// Maps viewports to their overlays. Needed by the viewport listener.
    overlay::ViewportListener::ViewportOverlaysMapType m_viewportOverlaysMap;

    /// Listens for render target updates for all viewports and enables the required overlays.
    overlay::ViewportListener m_viewportListener { m_viewportOverlaysMap };

    /// Ogre root
    ::Ogre::Root* m_ogreRoot { nullptr };

    /// How the rendering is triggered ?
    RenderMode m_renderMode { RenderMode::AUTO };

    /// True if the render window is in fullscreen.
    bool m_fullscreen { false };

    /// Width for off-screen rendering
    unsigned int m_width { 0 };

    /// Height for off-screen rendering
    unsigned int m_height { 0 };

    /// If true, scene is rendered off-screen
    bool m_offScreen { false };

    /// If true and doing offscreen rendering, the scene will be rendered upside down.
    bool m_flip { false };
};

//-----------------------------------------------------------------------------

template<class T>
std::vector<SPTR(T)> SRender::getAdaptors() const
{
    auto servicesVector = ::fwServices::OSR::getServices("::fwRenderOgre::IAdaptor");
    std::vector<SPTR(T)> resultVector;

    for(auto& sceneAdaptor : servicesVector)
    {
        SPTR(T) adaptor = T::dynamicCast(sceneAdaptor);
        if( adaptor )
        {
            if( adaptor->getRenderService() == this->getConstSptr())
            {
                resultVector.push_back(adaptor);
            }
        }
    }

    return resultVector;
}

//------------------------------------------------------------------------------

inline SRender::RenderMode SRender::getRenderMode() const
{
    return m_renderMode;
}

//-----------------------------------------------------------------------------

}
