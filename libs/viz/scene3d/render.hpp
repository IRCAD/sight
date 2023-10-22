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
#include "viz/scene3d/layer.hpp"
#include "viz/scene3d/utils.hpp"
#include "viz/scene3d/window_interactor.hpp"

#include <data/image.hpp>

#include <viz/__/render.hpp>

#include <OGRE/OgreAxisAlignedBox.h>

#include <map>
#include <tuple>

namespace sight::viz::scene3d
{

class adaptor;
class layer;

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
 * - \b resetCamera_LAYER_ID(): "LAYER_ID" needs to be changed with your actual layer id. Resets the camera of the layer
 * specified by LAYER_ID.
 * - \b requestRender(): request the service to repaint the scene.
 * - \b disableFullscreen(): switches to windowed rendering if fullscreen is enabled.
 * - \b enableFullscreen(int): switches fullscreen rendering on the given screen.
 * - \b setManualMode(): switches to manual mode, the scene is rendered whenever the requestRender() slot is called.
 * - \b setAutoMode(): switches to auto mode, the scene is rendered whenever an adaptor decides it.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::viz::scene3d::render" auto_connect="true">
        <scene renderMode="auto">
            <background topColor="#000000" bottomColor="#FFFFFF" topScale="0.7" bottomScale="1.0"/>

            <layer id="..." order="1">
                <viewport hOffset="0.03" vOffset="0.03" width="0.3" height="0.3" hAlign="right" vAlign="top"/>
                <adaptor uid="..."/>
            </layer>
            <layer id="..." order="2" compositors="Invert;Laplace;Posterize" defaultLight="false" overlays="...">
                <adaptor uid="..."/>
            </layer>
        </scene>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b offScreen [sight::data::image] (optional, unused by default): If used, render the scene in an image
 *      and not in a window.
 *
 * @subsection Configuration Configuration
 *  - \b scene (mandatory)
 *      - \b renderMode (optional, auto/manual, default=auto): 'auto' (when any of the adaptor calls "requestRender",
 *           i.e. when its data has changed), or 'manual' (only when the slot "requestRender" is called). This can also
 *           be changed at runtime with setManualMode and setAutoMode slots.
 *      - \b width (optional, int, default=1280): width for off-screen rendering.
 *      - \b height (optional, int, default=720): height for off-screen rendering.
 *  - \b background (optional): defines the scene background color.
 *      - \b color (optional, hexadecimal, default=#000000): color of the background, can't be used with topColor and
 *           bottomColor.
 *      - \b topColor (optional, hexadecimal, default=#000000): top color of the background.
 *      - \b bottomColor (optional, hexadecimal, default=#000000): top color of the background.
 *      - \b topScale (optional, float, default=0): top background scale.
 *      - \b bottomScale (optional, float, default=1): bottom background scale.
 *      - \b material (optional): overrides the default gradient material. Provide the name of the Ogre material.
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
 *    - \b adaptor
 *      - \b uid (mandatory): the identifier of the adaptor.
 */
class VIZ_SCENE3D_CLASS_API render final : public viz::render
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(render, viz::render);

    /// Represents all possible render modes.
    enum class RenderMode : std::uint8_t
    {
        AUTO,
        MANUAL
    };

    struct signals
    {
        using key_t                      = sight::core::com::signals::key_t;
        using void_signal_t              = sight::core::com::signal<void ()>;
        using compositorUpdated_signal_t = core::com::signal<void (std::string, bool, viz::scene3d::layer::sptr)>;

        static inline const key_t FULLSCREEN_SET     = "fullscreenSet";
        static inline const key_t FULLSCREEN_UNSET   = "fullscreenUnset";
        static inline const key_t COMPOSITOR_UPDATED = "compositorUpdated";
    };

    /// Defines the type of adaptors ID.
    typedef std::string adaptor_id_t;

    /// Defines the type of object ID.
    typedef std::string ogre_object_id_t;

    /// Defines the type of scene ID.
    typedef std::string scene_id_t;

    /// Defines actives layouts in the scene.
    typedef std::map<scene_id_t, SPTR(viz::scene3d::layer)> layer_map_t;

    /// Contains the slot name that computes the parameters to reset the camera.
    VIZ_SCENE3D_API static const core::com::slots::key_t COMPUTE_CAMERA_ORIG_SLOT;

    /// Contains the slot name that resets all layers camera.
    VIZ_SCENE3D_API static const core::com::slots::key_t RESET_CAMERAS_SLOT;

    /// Contains the slot name that request the picker to do a ray cast according to the passed position.
    VIZ_SCENE3D_API static const core::com::slots::key_t DO_RAY_CAST_SLOT;

    /// Contains the slot name that requests a rendering.
    VIZ_SCENE3D_API static const core::com::slots::key_t REQUEST_RENDER_SLOT;

    /// Contains the slot name that disables fullscreen rendering if it was enabled.
    VIZ_SCENE3D_API static const core::com::slots::key_t DISABLE_FULLSCREEN;

    /// Contains the slot name that enables fullscreen rendering on a specific screen.
    VIZ_SCENE3D_API static const core::com::slots::key_t ENABLE_FULLSCREEN;

    /// Contains the slot name that enables the manual rendering mode.
    VIZ_SCENE3D_API static const core::com::slots::key_t SET_MANUAL_MODE;

    /// Contains the slot name that enables the automatic rendering mode.
    VIZ_SCENE3D_API static const core::com::slots::key_t SET_AUTO_MODE;

    /// Defines the layer ID of the background.
    VIZ_SCENE3D_API static const std::string s_OGREBACKGROUNDID;

    /// Initializes slots.
    VIZ_SCENE3D_API render() noexcept;

    /// Destroys the service.
    VIZ_SCENE3D_API ~render() noexcept override;

    /// Sets this render service as the current OpenGL context.
    VIZ_SCENE3D_API void makeCurrent();

    /// Requests a render from the Ogre render engine.
    VIZ_SCENE3D_API void requestRender();

    /// @returns true if the scene is shown on screen.
    VIZ_SCENE3D_API bool isShownOnScreen();

    /// @returns the scene manager corresponding to the sceneID.
    VIZ_SCENE3D_API Ogre::SceneManager* getSceneManager(const std::string& _scene_id);

    /// @returns the layer corresponding to the sceneID.
    VIZ_SCENE3D_API viz::scene3d::layer::sptr getLayer(const std::string& _scene_id);

    /// @returns this render layers.
    VIZ_SCENE3D_API layer_map_t getLayers();

    /// @returns m_interactorManager.
    VIZ_SCENE3D_API viz::scene3d::window_interactor::sptr getInteractorManager() const;

    /// Resets camera parameters with the actual global bounding box.
    VIZ_SCENE3D_API void resetCameraCoordinates(const std::string& _layer_id);

    /// Resets all layers camera parameters with the actual global bounding box.
    VIZ_SCENE3D_API void resetCameras();

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
    void paint();

    /// Configures background layer of the scene.
    void configureBackgroundLayer(const config_t& _cfg);

    /// Configures each layer of the scene.
    void configureLayer(const config_t& _cfg);

    /// Retrieves the viewport parameters from the configuration.
    static layer::viewport_config_t configureLayerViewport(const service::config_t& _cfg);

    /**
     * @brief Renders the scene in fullscreen on the screen with the given index.
     * @param screen the index of the screen where the fullscreen is enabled.
     */
    void enableFullscreen(int _screen);

    /// Switches back to windowed rendering if fullscreen is on.
    void disableFullscreen();

    /// Contains all the layers of the scene.
    layer_map_t m_layers;

    /// Contains the Ogre window interactor manager.
    viz::scene3d::window_interactor::sptr m_interactorManager;

    /// Contains the Ogre root.
    Ogre::Root* m_ogreRoot {nullptr};

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

    static constexpr std::string_view s_OFFSCREEN_INOUT = "offScreen";
    data::ptr<data::image, data::Access::inout> m_offScreenImage {this, s_OFFSCREEN_INOUT, false, true};
};

//-----------------------------------------------------------------------------

template<class T>
std::vector<SPTR(T)> render::getAdaptors() const
{
    auto services_vector = sight::service::get_services("sight::viz::scene3d::adaptor");
    std::vector<SPTR(T)> result_vector;

    for(const auto& scene_adaptor : services_vector)
    {
        SPTR(T) adaptor = std::dynamic_pointer_cast<T>(scene_adaptor);
        if(adaptor)
        {
            if(adaptor->getRenderService() == this->get_const_sptr())
            {
                result_vector.push_back(adaptor);
            }
        }
    }

    return result_vector;
}

//------------------------------------------------------------------------------

inline render::RenderMode render::getRenderMode() const
{
    return m_renderMode;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
