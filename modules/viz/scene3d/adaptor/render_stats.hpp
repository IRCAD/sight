/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/text.hpp>

#include <OGRE/OgreColourValue.h>

#include <memory>

namespace sight::module::viz::scene3d::adaptor
{

class PostWindowRenderListener;

/**
 * @brief This adaptor displays rendering statistics in the window overlay.
 *
 * Displays the last FPS and the triangle count.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="renderStatsAdaptor" type="sight::module::viz::scene3d::adaptor::render_stats">
        <config color="#ff0000" fontSize="12"/>
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b color (optional, hexadecimal, default=#FFFFFF): stats' text color
 * - \b fontSize (optional, unsigned int, default=12): stats font size in points.
 */
class render_stats final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(render_stats, sight::viz::scene3d::adaptor);

    /// Creates the adaptor.
    render_stats() noexcept;

    /// Destroys the adaptor.
    ~render_stats() noexcept final;

protected:

    /// Configures stats color and size.
    void configuring() final;

    /// Initializes the overlay text displaying the stats.
    void starting() final;

    /// Does nothing.
    void updating() final;

    /// Destroys the overlay text.
    void stopping() final;

private:

    friend class PostWindowRenderListener;

    /// Contains the listener to frame update events and updates the stats text.
    std::unique_ptr<PostWindowRenderListener> m_listener;

    /// Contains the displays stats in the overlay.
    sight::viz::scene3d::text::sptr m_stats_text;

    /// Defines the text's color.
    Ogre::ColourValue m_text_color;

    /// Defines the text's font height in points.
    std::size_t m_font_size {12};
};

} // namespace sight::module::viz::scene3d::adaptor.
