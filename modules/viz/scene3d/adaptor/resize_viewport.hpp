/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "modules/viz/scene3d/config.hpp"

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/layer.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor resizes and moves viewports.
 *
 * @section Slots Slots
 * - \b resize(): resizes the viewport to the new size.
 * - \b revert(): revert back to the initial viewport size.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::resize_viewport" >
        <config hOffset="0" vOffset="0" width="1" height="1" hAlign="left" vAlign="top"/>
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b hAlign (optional, left|center|right, default=left): defines the horizontal origin of the new viewport.
 * - \b vAlign (optional, top|center|bottom, default=top): defines the vertical origin of the new viewport.
 * - \b hOffset (optional, float, default=0.f): horizontal offset from the origin relatively to the window.
 * - \b vOffset (optional, float, default=0.f): vertical offset from the origin relatively to the window.
 * - \b width (optional, float, default=1.f): new viewport width relatively to the window.
 * - \b height (optional, float, default=1.f): new viewport height relatively to the window.
 */
class MODULE_VIZ_SCENE3D_CLASS_API resize_viewport final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(resize_viewport, sight::viz::scene3d::adaptor);

    /// Initializes slots.
    MODULE_VIZ_SCENE3D_API resize_viewport() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~resize_viewport() noexcept final = default;

protected:

    /// Configures the new viewport's dimensions.
    MODULE_VIZ_SCENE3D_API void configuring() final;

    /// Starts the adaptor, records the initial viewport dimensions.
    MODULE_VIZ_SCENE3D_API void starting() final;

    /// Updates the adaptor. Unused here.
    MODULE_VIZ_SCENE3D_API void updating() noexcept final;

    /// Stops the adaptor. Unused here.
    MODULE_VIZ_SCENE3D_API void stopping() noexcept final;

private:

    /// Switches to the initial/new viewport depending on the input.
    void resize(bool _resize);

    /// Defines the original viewports dimensions on the layer.
    void revert();

    /// Defines the initial viewport dimensions.
    sight::viz::scene3d::layer::viewport_config_t m_previous_viewport_dimensions {0.F, 0.F, 1.F, 1.F};

    /// Defines the configured viewport dimensions.
    sight::viz::scene3d::layer::viewport_config_t m_new_viewport_dimensions {0.F, 0.F, 1.F, 1.F};
};

} // namespace sight::module::viz::scene3d::adaptor.
