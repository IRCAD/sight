/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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
#include "fwGui/container/fwMenuBar.hpp"
#include "fwGui/GuiBaseObject.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the interface class  for the menubar builder.
 */
class FWGUI_CLASS_API IMenuBarBuilder : public ::fwGui::GuiBaseObject
{
public:
    fwCoreClassMacro(IMenuBarBuilder, ::fwGui::GuiBaseObject)

    typedef std::string RegistryKeyType;

    FWGUI_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    FWGUI_API IMenuBarBuilder();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IMenuBarBuilder();

    /**
     * @brief Returns the builded menu bar.
     */
    FWGUI_API virtual ::fwGui::container::fwMenuBar::sptr getMenuBar();

    /**
     * @brief Configure the layout before creation.
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief Instantiate layout with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    FWGUI_API virtual void createMenuBar( ::fwGui::container::fwContainer::sptr parent ) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUI_API virtual void destroyMenuBar() = 0;

protected:

    /// MenuBar.
    ::fwGui::container::fwMenuBar::sptr m_menuBar;

    /// Background color. Use an empty string to use the default background color, else, set an hexadecimal value.
    std::string m_backgroundColor;

};

} // namespace builder
} // namespace fwGui
