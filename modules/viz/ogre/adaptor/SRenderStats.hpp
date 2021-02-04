/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include <OGRE/OgreColourValue.h>

#include <viz/ogre/IAdaptor.hpp>
#include <viz/ogre/Text.hpp>

#include <memory>

namespace sight::modules::viz::ogre::adaptor
{

class PostWindowRenderListener;

/**
 * @brief This adaptor displays rendering statistics in the window overlay.
 *
 * Displays the last FPS and the triangle count.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="renderStatsAdaptor" type="::sight::modules::viz::ogre::adaptor::SRenderStats">
        <config layer="default" color="#ff0000" fontSize="12"/>
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): not really used but it is needed to be bound to a render service.
 * - \b color (optional, hexadecimal, default=#FFFFFF): stats' text color
 * - \b fontSize (optional, unsigned int, default=12): stats font size in points.
 */
class MODULE_VIZ_OGRE_CLASS_API SRenderStats final : public sight::viz::ogre::IAdaptor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SRenderStats, ::sight::viz::ogre::IAdaptor)

    /// Creates the adaptor.
    MODULE_VIZ_OGRE_API SRenderStats() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_OGRE_API ~SRenderStats() noexcept override;

protected:

    /// Configures stats color and size.
    MODULE_VIZ_OGRE_API void configuring() override;

    /// Initializes the overlay text displaying the stats.
    MODULE_VIZ_OGRE_API void starting() override;

    /// Does nothing.
    MODULE_VIZ_OGRE_API void updating() override;

    /// Destroys the overlay text.
    MODULE_VIZ_OGRE_API void stopping() override;

private:

    friend class PostWindowRenderListener;

    /// Contains the listener to frame update events and updates the stats text.
    std::unique_ptr< PostWindowRenderListener > m_listener;

    /// Contains the displays stats in the overlay.
    sight::viz::ogre::Text* m_statsText { nullptr };

    /// Defines the text's color.
    ::Ogre::ColourValue m_textColor;

    /// Defines the text's font height in points.
    size_t m_fontSize { 12 };

    /// Defines the TrueType font source file.
    std::string m_fontSource { "DejaVuSans.ttf" };

};

} // namespace sight::modules::viz::ogre::adaptor.
