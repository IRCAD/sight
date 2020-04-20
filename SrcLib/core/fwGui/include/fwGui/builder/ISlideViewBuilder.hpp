/************************************************************************
 *
 * Copyright (C) 2016-2020 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "fwGui/config.hpp"
#include "fwGui/container/fwContainer.hpp"
#include "fwGui/GuiBaseObject.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

namespace fwGui
{

namespace builder
{

/**
 * @brief Defines the interface class for the slide view builder.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <slideView vAlign="top" hAlign="left" widht="50%" height="15" opacity="1.0" >
        <styleSheet>color: blue; background-color: yellow</styleSheet>
    </slideView>
   @endcode
 *
 * @subsection Configuration Configuration
 * - \b hAlign (optional, left/right, default=left): horizontal alignment of the widget.
 * - \b vAlign (optional, top/bottom, default=top): vertical alignment of the widget.
 * - \b hOffset (optional, int, default=0): horizontal offset of the widget (px or %).
 * - \b vOffset (optional, int, default=0): vertical offset of the widget (px or %).
 * - \b width (optional, int, default=100%): width of the widget (px or %).
 * - \b height (optional, int, default=100%): height of the widget (px or %).
 * - \b opacity (optional, float, default=1.0) : widget opacity.
 * - \b animatable (optional, bool, default=false): defines if the slide bar must use animation or not.
 * - \b animatableAlignment (optional, , default=): defines the animation direction.
 * - \b styleSheet (optional, string, default=""): style of the widget (see Qt style sheets
 * http://doc.qt.io/qt-5/stylesheet-examples.html).
 */
class FWGUI_CLASS_API ISlideViewBuilder : public ::fwGui::GuiBaseObject
{
public:

    fwCoreClassMacro(ISlideViewBuilder, ::fwGui::GuiBaseObject)

    FWGUI_API const static std::string REGISTRY_KEY;

    /// Defines the horizontal alignment of the widget.
    enum HAlignment
    {
        RIGHT,
        LEFT
    };

    /// Defines the vertical alignment of the widget.
    enum VAlignment
    {
        TOP,
        BOTTOM
    };

    /// Defines animation direction.
    enum AnimatableAlignment
    {
        RIGHT_ANIMATION,
        LEFT_ANIMATION,
        TOP_ANIMATION,
        BOTTOM_ANIMATION
    };

    /// Creates the slide view builder.
    FWGUI_API ISlideViewBuilder();

    /// Destroyes the builder.
    FWGUI_API virtual ~ISlideViewBuilder();

    /// Returns the generic container.
    FWGUI_API virtual ::fwGui::container::fwContainer::sptr getContainer() const;

    /// Parses the configuration.
    FWGUI_API virtual void initialize(::fwRuntime::ConfigurationElement::sptr _config);

    /// Creates the container.
    FWGUI_API virtual void createContainer(::fwGui::container::fwContainer::sptr _parent) = 0;

    /// Destoyes the container.
    FWGUI_API virtual void destroyContainer() = 0;

protected:

    /// Contains the generic container of the slide view.
    ::fwGui::container::fwContainer::sptr m_container;

    /// Defines the horizontal alignment.
    HAlignment m_hAlignment { LEFT };

    /// Defines the vertical alignment.
    VAlignment m_vAlignment { TOP };

    /// Defines the width of the widget.
    int m_width { 100 };

    /// Defines if the width must be read a percent.
    bool m_percentWidth { true };

    /// Defines the height of the widget.
    int m_height { 100 };

    /// Defines if the height must be read a percent.
    bool m_percentHeight { true };

    /// Defines the horizontal offset of the widget.
    int m_hOffset { 0 };

    /// Defines if the horizontal offset must be read a percent.
    bool m_percentHOffset { false };

    /// Defines the vertical offset of the widget.
    int m_vOffset { 0 };

    /// Defines if the vertical offset must be read a percent.
    bool m_percentVOffset { false };

    /// Defines the widget opacity.
    double m_opacity { 1. };

    /// Defines if the widget animation is enabled.
    bool m_animatable { false };

    /// Defines the animation direction.
    AnimatableAlignment m_animatableAlignment { TOP_ANIMATION };

    /// Defines the additional style sheet of the widget.
    std::string m_styleSheet;

};

} // namespace builder.
} // namespace fwGui.
