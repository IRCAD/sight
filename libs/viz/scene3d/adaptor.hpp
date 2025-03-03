/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include <core/updater.hpp>

#include <data/boolean.hpp>

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
    public service::has_services,
    public sight::core::updater<16>
{
friend class render;

public:

    /// Helper function to generate unique identifiers for children objects
    std::string gen_id(const std::string_view& _name) const
    {
        return sight::core::id::join(this->get_id(), _name);
    }

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
     * @param _visible the visibility status.
     * @see setVisible(bool)
     */
    SIGHT_VIZ_SCENE3D_API void update_visibility(bool _visible);

    /**
     * @brief SLOT: Applies the current visibility state to the graphical objects.
     */
    SIGHT_VIZ_SCENE3D_API void apply_visibility();

    /// Returns the visibility of the adaptor.
    SIGHT_VIZ_SCENE3D_API bool visible() const;

protected:

    struct slots
    {
        static inline const core::com::slots::key_t APPLY_VISIBILITY  = "apply_visibility";
        static inline const core::com::slots::key_t UPDATE_VISIBILITY = "update_visibility";
        static inline const core::com::slots::key_t TOGGLE_VISIBILITY = "toggle_visibility";
        static inline const core::com::slots::key_t SHOW              = "show";
        static inline const core::com::slots::key_t HIDE              = "hide";
        static inline const core::com::slots::key_t LAZY_UPDATE       = "lazy_update";
    };

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
    SIGHT_VIZ_SCENE3D_API void init();

    /// Unregisters the adaptor from its render service.
    SIGHT_VIZ_SCENE3D_API void deinit();

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

    /// Connects the properties signals, this must be explicitly called by children classes.
    SIGHT_VIZ_SCENE3D_API service::connections_t auto_connections() const override;

    /// Calls updating when the update is required
    SIGHT_VIZ_SCENE3D_API void do_update() final;

    /// Defines the layer ID:
    std::string m_layer_id;

    /// TEMP: configured layer ID until we remove it
    std::string m_cfg_layer_id;

    /// Contains the t=render service which this adaptor is attached.
    viz::scene3d::render::wptr m_render_service;

private:

    /// Ensure visibility changes are applied when rendering is requested.
    bool m_visibility_applied {true};

    /// Enables the adaptor visibility.
    sight::data::property<sight::data::boolean> m_visible {this, "visible", true};
};

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d.
