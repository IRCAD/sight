/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "modules/viz/ogre/config.hpp"

#include <viz/ogre/IAdaptor.hpp>
#include <viz/ogre/Layer.hpp>

namespace sight::modules::viz::ogre::adaptor
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
    <service type="::sight::modules::viz::ogre::adaptor::SResizeViewport" >
        <config layer="default" hOffset="0" vOffset="0" width="1" height="1" hAlign="left" vAlign="top"/>
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): layer whose viewport is resized
 * - \b hAlign (optional, left|center|right, default=left): defines the horizontal origin of the new viewport.
 * - \b vAlign (optional, top|center|bottom, default=top): defines the vertical origin of the new viewport.
 * - \b hOffset (optional, float, default=0.f): horizontal offset from the origin relatively to the window.
 * - \b vOffset (optional, float, default=0.f): vertical offset from the origin relatively to the window.
 * - \b width (optional, float, default=1.f): new viewport width relatively to the window.
 * - \b height (optional, float, default=1.f): new viewport height relatively to the window.
 */
class MODULE_VIZ_OGRE_CLASS_API SResizeViewport final : public sight::viz::ogre::IAdaptor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SResizeViewport, sight::viz::ogre::IAdaptor)

    /// Initializes slots.
    MODULE_VIZ_OGRE_API SResizeViewport() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_OGRE_API ~SResizeViewport() noexcept override;

protected:

    /// Configures the new viewport's dimensions.
    MODULE_VIZ_OGRE_API void configuring() override;

    /// Starts the adaptor, records the initial viewport dimensions.
    MODULE_VIZ_OGRE_API void starting() override;

    /// Updates the adaptor. Unused here.
    MODULE_VIZ_OGRE_API void updating() noexcept override;

    /// Stops the adaptor. Unused here.
    MODULE_VIZ_OGRE_API void stopping() noexcept override;

private:

    /// Switches to the initial/new viewport depending on the input.
    void resizeViewport(bool _resize);

    /// Defines the configured viewports dimensions on the layer.
    void resize();

    /// Defines the original viewports dimensions on the layer.
    void revert();

    /// Defines the initial viewport dimensions.
    sight::viz::ogre::Layer::ViewportConfigType m_previousViewportDimensions { 0.f, 0.f, 1.f, 1.f };

    /// Defines the configured viewport dimensions.
    sight::viz::ogre::Layer::ViewportConfigType m_newViewportDimensions { 0.f, 0.f, 1.f, 1.f };

};

} // namespace sight::modules::viz::ogre::adaptor.
