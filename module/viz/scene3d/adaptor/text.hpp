/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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
#include <OGRE/OgreVector.h>

#include <memory>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays a text object in the center or along the window borders.
 *
 * @section Slots Slots
 * -\b set_text(std::string): sets the text to be displayed.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::text">
        <in key="object" uid="..." />
        <config fontSize="32" hAlign="right" vAlign="bottom" color="#ff3396" />
    </service>
   @endcode
 *
 * Alternatively, you can set the text in the service's configuration :
 *
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::text">
        <text>Hello World!</text>
        <config fontSize="32" hAlign="right" vAlign="bottom" color="#ff3396" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b object (optional): "GenericField" object that can be casted to a text string.
 *
 * @subsection Configuration Configuration:
 * - \b color (optional, hexadecimal, default=#FFFFFF): the color and opacity of the text.
 * - \b fontSource (optional, string, default=DejaVuSans.ttf): true_t font (*.ttf) source file.
 * - \b fontSize (optional, unsigned int, default=16): font size in points.
 * - \b hAlign (optional, left|center|right, default="left"): horizontal alignment.
 * - \b vAlign (optional, top|center|bottom, default="bottom"): vertical alignment.
 * - \b text (optional, string, default=""): text to display, only available when no input is set.
 * - \b x (optional, float, default=0.): horizontal coordinate relatively to the screen.
 * - \b y (optional, float, default=0.): vertical coordinate relatively to the screen.
 */
class text final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(text, sight::viz::scene3d::adaptor);

    /// Creates the adaptor.
    text() noexcept;

    /// Destroys the adaptor.
    ~text() noexcept final = default;

protected:

    /// Configures the service.
    void configuring() final;

    /// Creates the text object and adds it to the scene.
    void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::object::s_OBJECT_INPUT of s_POINTLIST_INPUT to service::slots::UPDATE
     */
    connections_t auto_connections() const final;

    /// Updates the displayed text from the input object.
    void updating() final;

    /// Removes the text from the ogre scene and deletes it.
    void stopping() final;

private:

    /// Defines the text string.
    void set_text(std::string _str);

    // cspell:ignore RRGGBB RRGGBBAA
    ///Slot: Updates the text color (#RRGGBB or #RRGGBBAA)
    void set_color(std::string _color);

    /// Updates the displayed text from the input object.
    void update_text();

    /// Contains the displayed stats in the overlay.
    sight::viz::scene3d::text::sptr m_text;

    /// Defines the text's color.
    std::string m_text_color {"#FFFFFF"};

    /// Defines the font size in points.
    std::size_t m_font_size {12};

    /// Defines the position of the text.
    Ogre::Vector2 m_position {0.F, 0.F};

    /// Defines the vertical alignment type (top, center or bottom).
    std::string m_vertical_alignment;

    /// Defines the horizontal alignment type (left, center or right).
    std::string m_horizontal_alignment;

    /// Defines the displayed message.
    std::string m_text_string;

    /// Defines the true_t font source file.
    std::string m_font_source {"DejaVuSans.ttf"};

    static constexpr std::string_view OBJECT_INPUT = "object";
    sight::data::ptr<sight::data::object, sight::data::access::in> m_object {this, OBJECT_INPUT};
};

} // namespace sight::module::viz::scene3d::adaptor.
