/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <sight/viz/scene3d/config.hpp>

#include "viz/scene3d/render.hpp"

#include <service/base.hpp>
#include <service/has_services.hpp>

#include <OGRE/OgreSceneManager.h>

#include <string>

namespace sight::viz::scene3d
{

/**
 * @brief Interface providing behavior of Ogre adaptor services
 *
 * @section Slots Slots
 * - \b update_visibility(bool): sets whether the adaptor is shown or not.
 * - \b toggle_visibility(): toggle whether the adaptor is shown or not.
 * - \b show(): shows the adaptor.
 * - \b hide(): hides the hide.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="...">
        <config visible="true" />
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API adaptor :
    public service::base,
    public service::has_services
{
friend class render;

public:

    /// Configuration helper string that stores "config.<xmlattr>."
    SIGHT_VIZ_SCENE3D_API static const std::string CONFIG;

    /// Generates default methods.
    SIGHT_DECLARE_CLASS(adaptor, service::base);

    /// Sets the layer ID.
    SIGHT_VIZ_SCENE3D_API void set_layer_id(const std::string& _id);

    /// @returns the layer ID of this adaptor.
    SIGHT_VIZ_SCENE3D_API const std::string& layer_id() const;

    /// @returns the layer where this adaptor is attached.
    SIGHT_VIZ_SCENE3D_API viz::scene3d::layer::sptr layer() const;

    /// Sets the render service using this adaptor.
    SIGHT_VIZ_SCENE3D_API void set_render_service(render::sptr _service);

    /// @returns the render service using this adaptor
    SIGHT_VIZ_SCENE3D_API render::sptr render_service() const;

    /**
     * @brief SLOT: sets the visibility of the adaptor.
     * @param _is_visible the visibility status.
     * @see setVisible(bool)
     */
    SIGHT_VIZ_SCENE3D_API void update_visibility(bool _is_visible);

protected:

    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t UPDATE_VISIBILITY_SLOT;
    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t TOGGLE_VISIBILITY_SLOT;
    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t SHOW_SLOT;
    SIGHT_VIZ_SCENE3D_API static const core::com::slots::key_t HIDE_SLOT;

    /// Initializes slots.
    SIGHT_VIZ_SCENE3D_API adaptor() noexcept;

    /// Destroys the adaptor.
    SIGHT_VIZ_SCENE3D_API ~adaptor() noexcept override = default;

    /**
     * @brief Write information in a stream.
     *
     * This method is used by operator<<(std::ostream & _sstream, base& _service)
     * to avoid declaration of << by all services.
     */
    SIGHT_VIZ_SCENE3D_API void info(std::ostream& _sstream) override;

    /// Parses common adaptor parameters.
    SIGHT_VIZ_SCENE3D_API void configure_params();

    /// Registers the adaptor into its render service.
    SIGHT_VIZ_SCENE3D_API void initialize();

    /**
     * @brief Gets the Ogre SceneManager
     * @return Ogre default scene manager
     */
    SIGHT_VIZ_SCENE3D_API Ogre::SceneManager* get_scene_manager();

    /// Asks the render service to update rendering.
    SIGHT_VIZ_SCENE3D_API virtual void request_render();

    /// SLOT: toggles the visibility of the adaptor.
    SIGHT_VIZ_SCENE3D_API void toggle_visibility();

    /// SLOT: shows the adaptor.
    SIGHT_VIZ_SCENE3D_API void show();

    /// SLOT: hides the adaptor.
    SIGHT_VIZ_SCENE3D_API void hide();

    /// Sets the visibility of the adaptor.
    SIGHT_VIZ_SCENE3D_API virtual void set_visible(bool _visible);

    /// Defines the layer ID:
    std::string m_layer_id;

    /// TEMP: configured layer ID until we remove it
    std::string m_cfg_layer_id;

    /// Contains the t=render service which this adaptor is attached.
    viz::scene3d::render::wptr m_render_service;

    /// Enables the adaptor visibility.
    bool m_visible {true};

    /// Ensure visibility changes are applied when rendering is requested.
    bool m_visibility_applied {true};
};

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d.
