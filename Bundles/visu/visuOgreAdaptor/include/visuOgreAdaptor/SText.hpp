/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/Text.hpp>

#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreVector.h>

#include <memory>

namespace visuOgreAdaptor
{

/**
 * @brief Adaptor displaying a text object in the center or along the window's borders.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="::visuOgreAdaptor::SText">
        <in key="object" uid="..." />
        <config layer="default" fontSize="32" hAlign="right" vAlign="bottom" color="#ff3396" />
    </service>
   @endcode

 * @subsection Input Input:
 * - \b object (optional) : "GenericField" object that can be casted to a text string.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory) : not really used but it is needed to be bound to a render service.
 * - \b color (optional, default=#ffffffff): the color and opacity of the text.
 * - \b fontSize (optional, default=32): font size in inches.
 * - \b hAlign (optional, values=left|center|right, default="left"): horizontal alignment.
 * - \b vAlign (optional, values=top|center|bottom, default="bottom"): vertical alignment.
 */
class VISUOGREADAPTOR_CLASS_API SText : public ::fwRenderOgre::IAdaptor
{
public:
    fwCoreServiceClassDefinitionsMacro((SText)(::fwRenderOgre::IAdaptor))

    /// Constructor.
    VISUOGREADAPTOR_API SText() noexcept;

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SText() noexcept override;

protected:

    /// Configures the service.
    VISUOGREADAPTOR_API virtual void configuring() final;

    /// Creates the text object and adds it to the scene.
    VISUOGREADAPTOR_API virtual void starting() final;

    /// Updates the displayed text from the input object.
    VISUOGREADAPTOR_API virtual void updating() final;

    /// Removes the text from the ogre scene and deletes it.
    VISUOGREADAPTOR_API virtual void stopping() final;

private:

    /// Sets the the text string.
    void setText(const std::string& str);

    /// Updates the displayed text from the input object.
    void updateText();

    /// Computes the position of text object relatively to the screen according to the alignment.
    void updatePositionFromAlignment();

    /// Displays stats in the overlay.
    ::fwRenderOgre::Text* m_text { nullptr };

    /// The text's color.
    ::Ogre::ColourValue m_textColor;

    /// The text's height, expressed as a fraction of the viewport height.
    /// FIXME: should be computed from the font size and the screen's physical size.
    const float m_textHeight = 0.03f;

    /// Font size in inches.
    unsigned int m_fontSize;

    /// Vertical alignment type (top, center or bottom).
    std::string m_verticalAlignment;

    /// Horizontal alignment type (left, center or right).
    std::string m_horizontalAlignment;

    /// Displayed message.
    std::string m_textString;

};

} //namespace visuOgreAdaptor
