/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include "modules/viz/scene3d/config.hpp"

#include <viz/scene3d/IAdaptor.hpp>
#include <viz/scene3d/Text.hpp>

#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreVector.h>

#include <memory>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays a text object in the center or along the window borders.
 *
 * @section Slots Slots
 * -\b setText(std::string): sets the text to be displayed.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::SText">
        <in key="object" uid="..." />
        <config layer="default" fontSource="DejaVuSans.ttf" fontSize="32"
                hAlign="right" vAlign="bottom" color="#ff3396" />
    </service>
   @endcode
 *
 * Alternatively, you can set the text in the service's configuration :
 *
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::SText">
        <text>Hello World!</text>
        <config layer="default" fontSource="DejaVuSans.ttf" fontSize="32"
                hAlign="right" vAlign="bottom" color="#ff3396" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b object (optional): "GenericField" object that can be casted to a text string.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): not really used but it is needed to be bound to a render service.
 * - \b color (optional, hexadecimal, default=#FFFFFF): the color and opacity of the text.
 * - \b fontSource (optional, string, default=DejaVuSans.ttf): TrueType font (*.ttf) source file.
 * - \b fontSize (optional, usinged int, default=16): font size in points.
 * - \b hAlign (optional, left|center|right, default="left"): horizontal alignment.
 * - \b vAlign (optional, top|center|bottom, default="bottom"): vertical alignment.
 * - \b text (optional, string, default=""): text to display, only available when no input is set.
 * - \b x (optional, float, default=0.): horizontal coordinate relatively to the screen.
 * - \b y (optional, float, default=0.): vertical coordinate relatively to the screen.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SText final : public sight::viz::scene3d::IAdaptor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SText, ::sight::viz::scene3d::IAdaptor)

    /// Creates the adaptor.
    MODULE_VIZ_SCENE3D_API SText() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~SText() noexcept override;

protected:

    /// Configures the service.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Creates the text object and adds it to the scene.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Object::s_OBJECT_INPUT of s_POINTLIST_INPUT to s_UPDATE_SLOT
     */
    MODULE_VIZ_SCENE3D_API KeyConnectionsMap getAutoConnections() const override;

    /// Updates the displayed text from the input object.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /// Removes the text from the ogre scene and deletes it.
    MODULE_VIZ_SCENE3D_API void stopping() override;

private:

    /// Defines the text string.
    void setText(std::string str);

    /// Updates the displayed text from the input object.
    void updateText();

    /// Computes the position of text object relatively to the screen according to the alignment.
    void updatePositionFromAlignment();

    /// Contains the displayed stats in the overlay.
    sight::viz::scene3d::Text* m_text { nullptr };

    /// Defines the text's color.
    ::Ogre::ColourValue m_textColor;

    /// Defines the font size in points.
    size_t m_fontSize { 16 };

    /// Defines the position of the text.
    ::Ogre::Vector2 m_position { 0.f, 0.f };

    /// Defines the vertical alignment type (top, center or bottom).
    std::string m_verticalAlignment;

    /// Defines the horizontal alignment type (left, center or right).
    std::string m_horizontalAlignment;

    /// Defines the displayed message.
    std::string m_textString;

    /// Defines the TrueType font source file.
    std::string m_fontSource {"DejaVuSans.ttf"};

};

} // namespace sight::module::viz::scene3d::adaptor.
