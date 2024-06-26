/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include <sight/viz/qt3d/config.hpp>

#include "viz/qt3d/render.hpp"

#include <service/base.hpp>

namespace sight::viz::qt3d
{

/**
 * @brief Interface providing behavior of Qt3D adaptor services.
 *
 * @section Slots Slots
 * - \b update_visibility(bool): sets whether the adaptor is shown or not.
 * - \b toggle_visibility(): toggle whether the adaptor is shown or not.
 * - \b show(): shows the adaptor.
 * - \b hide(): hides the hide.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="..." >
        <config visible="true" />
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class SIGHT_VIZ_QT3D_CLASS_API adaptor : public service::base
{
public:

    SIGHT_DECLARE_CLASS(adaptor, sight::service::base);

    /// Gets the render service using this adaptor.
    SIGHT_VIZ_QT3D_API render::sptr render_service() const;

protected:

    SIGHT_VIZ_QT3D_API static const sight::core::com::slots::key_t UPDATE_VISIBILITY_SLOT;
    SIGHT_VIZ_QT3D_API static const sight::core::com::slots::key_t TOGGLE_VISIBILITY_SLOT;
    SIGHT_VIZ_QT3D_API static const sight::core::com::slots::key_t SHOW_SLOT;
    SIGHT_VIZ_QT3D_API static const sight::core::com::slots::key_t HIDE_SLOT;

    /// Creates the interface.
    SIGHT_VIZ_QT3D_API adaptor();

    /// Destroys the interface.
    SIGHT_VIZ_QT3D_API ~adaptor() override;

    /// Parses common adaptor parameters.
    SIGHT_VIZ_QT3D_API void configure_params();

    /// Registers the adaptor into its render service.
    SIGHT_VIZ_QT3D_API void initialize();

    /**
     * @brief SLOT: sets the visibility of the adaptor.
     * @param _is_visible the visibility status.
     * @see setVisible(bool)
     */
    SIGHT_VIZ_QT3D_API void update_visibility(bool _is_visible);

    /// SLOT: toggles the visibility of the adaptor.
    SIGHT_VIZ_QT3D_API void toggle_visibility();

    /// SLOT: shows the adaptor.
    SIGHT_VIZ_QT3D_API void show();

    /// SLOT: hides the adaptor.
    SIGHT_VIZ_QT3D_API void hide();

    /// Sets the visibility of the adaptor.
    SIGHT_VIZ_QT3D_API virtual void set_visible(bool _visible);

    /// Contains the render service this adaptor is attached to.
    viz::qt3d::render::wptr m_render_service;

    /// Enables the adaptor visibility.
    bool m_visible {true};
};

} // namespace sight::viz::qt3d.
