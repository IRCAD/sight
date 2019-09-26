/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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
 * @brief   Defines the interface class for the slide view builder.
 *
 * Example of configuration
 * @code{.xml}
    <slideView align="top" size="200" opacity="1.0">
        <styleSheet>color: blue; background-color: yellow</styleSheet>
    </slideView>
   @endcode
 *      - \b align (optional, default=left) : defines the widget side ('left', 'right', 'top' or 'bottom')
 *      - \b size (optional, default=200) : defines the widget width or height (width if side is 'left' or 'right' and
 *           height if side is 'top' or 'bottom')
 *      - \b opacity (optional, default=1.0) : defines the widget opacity
 *      - \b styleSheet (optional) : defines the style of the widget (see Qt style sheets
 *           http://doc.qt.io/qt-5/stylesheet-examples.html )
 */
class FWGUI_CLASS_API ISlideViewBuilder : public ::fwGui::GuiBaseObject
{
public:
    fwCoreClassMacro(ISlideViewBuilder, ::fwGui::GuiBaseObject);

    typedef std::string RegistryKeyType;

    FWGUI_API const static RegistryKeyType REGISTRY_KEY;

    enum Aligment
    {
        TOP,
        BOTTOM,
        RIGHT,
        LEFT
    };

    /// Constructor. Do nothing.
    FWGUI_API ISlideViewBuilder();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~ISlideViewBuilder();

    /**
     * @brief Returns the builded tool bar.
     */
    FWGUI_API virtual ::fwGui::container::fwContainer::sptr getContainer() const;

    /**
     * @brief Initialize the tool bar.
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief Instantiate layout with parent toolBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent toolBar must be instanced.
     */
    FWGUI_API virtual void createContainer( ::fwGui::container::fwContainer::sptr parent ) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUI_API virtual void destroyContainer() = 0;

protected:

    ::fwGui::container::fwContainer::sptr m_container;

    int m_size;
    double m_opacity;
    Aligment m_aligment;
    std::string m_styleSheet;
};

} // namespace builder
} // namespace fwGui
