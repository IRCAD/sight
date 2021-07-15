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

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/IWindowInteractor.hpp"
#include "viz/scene3d/Layer.hpp"
#include "viz/scene3d/overlay/ViewportListener.hpp"
#include "viz/scene3d/picker/IPicker.hpp"
#include "viz/scene3d/Utils.hpp"

#include <core/com/helper/SigSlotConnection.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>
#include <core/runtime/ConfigurationElement.hpp>

#include <data/Composite.hpp>

#include <service/helper/Config.hpp>
#include <service/registry/ObjectService.hpp>

#include <viz/base/IRender.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/Overlay/OgreOverlay.h>

#include <map>
#include <tuple>

namespace sight::viz::scene3d
{

class IAdaptor;
class Layer;

/**
 * @brief The generic scene service shows adaptors in a 3D Ogre scene.
 *
 * @section Signals Signals
 * - \b compositorUpdated(): sent when the compositor chain is modified.
 * - \b fullscreenSet(bool): sent when fullscreen is enabled/disabled.
 *
 * @section Slots Slots
 * - \b computeCameraParameters(): resets the camera position to visualize the whole scene.
 * - \b resetCameras(): resets all layers camera.
 * - \b computeCameraClipping(): recomputes the camera's clipping range when the scene is updated.
 * - \b requestRender(): request the service to repaint the scene.
 * - \b disableFullscreen(): switches to windowed rendering if fullscreen is enabled.
 * - \b enableFullscreen(int): switches fullscreen rendering on the given screen.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::viz::scene3d::SRender" autoConnect="true">
        <scene renderMode="auto">
            <background topColor="#000000" bottomColor="#FFFFFF" topScale="0.7" bottomScale="1.0"/>

            <layer id="..." order="1">
                <viewport hOffset="0.03" vOffset="0.03" width="0.3" height="0.3" hAlign="right" vAlign="top"/>
            </layer>
            <layer id="..." order="2" compositors="Invert;Laplace;Posterize" defaultLight="false" overlays="..."/>

            <adaptor uid="..."/>
            <adaptor uid="..."/>
        </scene>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b offScreen [sight::data::Image] (optional, unused by default): If used, render the scene in an image
 *      and not in a window.
 *
 * @subsection Configuration Configuration
 *  - \b scene (mandatory)
 *      - \b renderMode (optional, auto/sync, default=auto): 'auto' (only when something has changed), or 'sync'
 *           (only when the slot "requestRender" is called).
 *      - \b width (optional, int, default=1280): width for off-screen rendering.
 *      - \b height (optional, int, default=720): height for off-screen rendering.
 *  - \b background (optional): defines the scene background color.
 *      - \b color (optional, hexadecimal, default=#000000): color of the background, can't be used with topColor and
 *           bottomColor.
 *      - \b topColor (optional, hexadecimal, default=#000000): top color of the background.
 *      - \b bottomColor (optional, hexadecimal, default=#000000): top color of the background.
 *      - \b topScale (optional, float, default=0): top background scale.
 *      - \b bottomScale (optional, float, default=1): bottom background scale.
 *  - \b layer (mandatory): defines the scene's layer.
 *      - \b viewport (optional):
 *          - \b hAlign (optional, left|center|right, default=left): defines the horizontal origin of the viewport.
 *          - \b vAlign (optional, top|center|bottom, default=top): defines the vertical origin of the viewport.
 *          - \b hOffset (optional, float, default=0.f): horizontal offset from the origin relatively to the window.
 *          - \b vOffset (optional, float, default=0.f): vertical offset from the origin relatively to the window.
 *          - \b width (optional, float, default=1.f): viewport width relatively to the window.
 *          - \b height (optional, float, default=1.f): viewport height relatively to the window.
 *    - \b id (mandatory, string): the identifier of the layer
 *    - @deprecated- \b depth (mandatory, int): deprecated, please use order instead.
 *    - \b order (mandatory, int): layer order to stack, starting from 1. The layer with the lowest order will be
 *         rendered first, and so the layer with the highest order will be displayed on top.
 *    - \b transparency (optional, DepthPeeling/DualDepthPeeling/WeightedBlended/HybridTransparency/
 *         CelShadingDepthPeeling, default=""): the transparency technique to use.
 *    - \b numPeels (optional, string, default=""): number of peels for the selected transparency technique.
 *         Not used for WeightedBlended OIT
 *    - \b compositors (optional, string): defines the default compositor chain. Compositors are separated by semicolons
 *    - \b fullscreen (optional, bool, default=false): Show the scene in full screen.
 *    - \b stereoMode (optional, false/AutoStereo5/AutoStereo8/Stereo, default=false): Sets the mode used for
 * stereoscopic 3D
 *         rendering.
 *    - \b defaultLight (optional, true/false, default=true): Sets if a default light is created in the layer.
 *    - \b overlays (optional, string): list of overlay names (separated by semicolons) rendered on top of the layer's
 *         viewport.
 *  - \b adaptor
 *    - \b uid (mandatory): the identifier of the adaptor.
 */
class VIZ_SCENE3D_CLASS_API SRender final : public viz::base::IRender
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SRender, viz::base::IRender);

    /// Represents all possible render modes.
    enum class RenderMode
    {
        AUTO,
        SYNC
    };

    /// Defines the type of apdators ID.
    typedef std::string AdaptorIdType;

    /// Defines the type of object ID.
    typedef std::string OgreObjectIdType;

    /// Defines the type of scene ID.
    typedef std::string SceneIdType;

    /// Defines actives layouts in the scene.
    typedef std::map<SceneIdType, SPTR(viz::scene3d::Layer)> LayerMapType;

    /// Contains the signal sent when the compositor chain has been modified.
    VIZ_SCENE3D_API static const core::com::Signals::SignalKeyType s_COMPOSITOR_UPDATED_SIG;
    typedef core::com::Signal<void (std::string, bool, viz::scene3d::Layer::sptr)> CompositorUpdatedSignalType;

    /// Contains the signal sent when fullscreen was enabled/disabled.
    VIZ_SCENE3D_API static const core::com::Signals::SignalKeyType s_FULLSCREEN_SET_SIG;
    using FullscreenSetSignalType = core::com::Signal<void (bool)>;

    /// Contains the slot name that computes the parameters to reset the camera.
    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_COMPUTE_CAMERA_ORIG_SLOT;

    /// Contains the slot name that resets all layers camera.
    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_RESET_CAMERAS_SLOT;

    /// Contains the slot name that computes the parameters to reset the camera.
    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_COMPUTE_CAMERA_CLIPPING_SLOT;

    /// Contains the slot name that request the picker to do a ray cast according to the passed position.
    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_DO_RAY_CAST_SLOT;

    /// Contains the slot name that requests a rendering.
    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_REQUEST_RENDER_SLOT;

    /// Contains the slot name that disables fullscreen rendering if it was enabled.
    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_DISABLE_FULLSCREEN;

    /// Contains the slot name that enables fullscreen rendering on a specific screen.
    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_ENABLE_FULLSCREEN;

    /// Defines the layer ID of the background.
    VIZ_SCENE3D_API static const std::string s_OGREBACKGROUNDID;

    /// Initialiazes slots.
    VIZ_SCENE3D_API SRender() noexcept;

    /// Destroys the service.
    VIZ_SCENE3D_API ~SRender() noexcept override;

    /// Sets this render service as the current OpenGL context.
    VIZ_SCENE3D_API void makeCurrent();

    /// Requests a render from the Ogre render engine.
    VIZ_SCENE3D_API void requestRender();

    /// @returns true if the scene is shown on screen.
    VIZ_SCENE3D_API bool isShownOnScreen();

    /// @returns the scene manager corresponding to the sceneID.
    VIZ_SCENE3D_API ::Ogre::SceneManager* getSceneManager(const ::std::string& sceneID);

    /// @returns the layer corresponding to the sceneID.
    VIZ_SCENE3D_API viz::scene3d::Layer::sptr getLayer(const ::std::string& sceneID);

    /// @returns this render layers.
    VIZ_SCENE3D_API LayerMapType getLayers();

    /// @returns m_interactorManager.
    VIZ_SCENE3D_API viz::scene3d::IWindowInteractor::sptr getInteractorManager() const;

    /// Resets camera parameters with the actual global bounding box.
    VIZ_SCENE3D_API void resetCameraCoordinates(const std::string& _layerId);

    /// Resets all layers camera parameters with the actual global bounding box.
    VIZ_SCENE3D_API void resetCameras();

    /// Computes camera parameters with the actual global bounding box.
    VIZ_SCENE3D_API void computeCameraClipping();

    template<class T>
    std::vector<SPTR(T)> getAdaptors() const;

    /// Returns the rendering mode
    RenderMode getRenderMode() const;

protected:

    /// Configures adaptors and connections.
    VIZ_SCENE3D_API void starting() override;

    /// Stops all adaptors
    VIZ_SCENE3D_API void stopping() override;

    /// Configures the adaptor.
    VIZ_SCENE3D_API void configuring() override;

    /// Does nothing.
    VIZ_SCENE3D_API void updating() override;

private:

    /// Renders the scene.
    void render();

    /// Configures background layer of the scene.
    void configureBackgroundLayer(const ConfigType& _cfg);

    /// Configures each layer of the scene.
    void configureLayer(const ConfigType& _cfg);

    /// Retrieves the viewport parameters from the configuration.
    static Layer::ViewportConfigType configureLayerViewport(const service::IService::ConfigType& _cfg);

    /**
     * @brief Renders the scene in fullscreen on the screen with the given index.
     * @param screen the index of the screen where the fullscreen is enabled.
     */
    void enableFullscreen(int _screen);

    /// Switchs back to windowed rendering if fullscreen is on.
    void disableFullscreen();

    /// Contains all the layers of the scene.
    LayerMapType m_layers;

    /// Contains the signal sent when fullscreen is enabled/disabled.
    FullscreenSetSignalType::sptr m_fullscreenSetSig;

    /// Contains the Ogre window interactor manager.
    viz::scene3d::IWindowInteractor::sptr m_interactorManager;

    /// Maps viewports to their overlays. Needed by the viewport listener.
    overlay::ViewportListener::ViewportOverlaysMapType m_viewportOverlaysMap;

    /// Listens for render target updates for all viewports and enables the required overlays.
    overlay::ViewportListener m_viewportListener {m_viewportOverlaysMap};

    /// Contains the Ogre root.
    ::Ogre::Root* m_ogreRoot {nullptr};

    /// Defines how the rendering is triggered.
    RenderMode m_renderMode {RenderMode::AUTO};

    /// Defines if the render window is in fullscreen.
    bool m_fullscreen {false};

    /// Defines the width for off-screen rendering.
    unsigned int m_width {0};

    /// Defines the height for off-screen rendering.
    unsigned int m_height {0};

    /// Defines if the scene is rendered off-screen.
    bool m_offScreen {false};

    /// Defines if the scene will be rendered upside down.
    /// @warning the scene must be rendered off-screen.
    bool m_flip {false};
};

//-----------------------------------------------------------------------------

template<class T>
std::vector<SPTR(T)> SRender::getAdaptors() const
{
    auto servicesVector = service::OSR::getServices("::sight::viz::scene3d::IAdaptor");
    std::vector<SPTR(T)> resultVector;

    for(auto& sceneAdaptor : servicesVector)
    {
        SPTR(T) adaptor = T::dynamicCast(sceneAdaptor);
        if(adaptor)
        {
            if(adaptor->getRenderService() == this->getConstSptr())
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

} // namespace sight::viz
